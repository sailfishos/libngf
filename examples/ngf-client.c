#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <libngf/client.h>

#define MAX_BUF_SIZE 512

typedef struct _TestClient
{
    GMainLoop       *loop;
    GIOChannel      *channel;
    guint           event_source;

    DBusConnection  *connection;
    NgfClient       *client;
} TestClient;

static char*
copy_str (char *str, size_t length)
{
    if (str == NULL || length <= 0)
        return NULL;

    char *p = (char*) malloc (sizeof (char) * (length + 1));
    if (p == NULL)
        return NULL;

    p[length] = '\0';
    strncpy (p, str, length);
    return p;
}

static char*
skip_whitespace (char *str)
{
    char *p = str;
    while (*p != '\0' && (*p == ' ' || *p == '\t'))
        p++;
    return p;
}

static char*
get_str (char *buffer, char delim, char **str)
{
    char *p = NULL, *start = NULL;
    size_t length = 0;
    gboolean expect_quote = FALSE;

    if ((p = skip_whitespace (buffer)) == NULL)
        return NULL;

    start = p;
    if (*p == '"') {
        expect_quote = TRUE;
        start = ++p;
    }

    while (*p != '\0' && *p != '\n') {
        if ((!expect_quote && (*p == ' ' || *p == delim)) || (expect_quote && *p == '"'))
            break;

        p++;
    }

    length = (size_t) (p - start);
    *str = copy_str (start, length);

    if (*p == '"' || *p == delim)
        p++;

    return p;
}

static void
parse_command_play (TestClient *c, char *buf)
{
    char *advance = buf;
    char *event_id = NULL, *key = NULL, *value = NULL;
    NgfProplist *p = NULL;

    /* First parameter is the event */
    advance = get_str (buf, ' ', &event_id);
    if (event_id == NULL) {
        g_print ("Usage: play [EVENT_NAME] [KEY:VALUE], returns [ID]*\n");
        return;
    }

    p = ngf_proplist_new ();

    /* Then we have variable amount of key-value pairs */
    while (1) {
        advance = get_str (advance, ':', &key);
        if (key == NULL)
            break;

        advance = get_str (advance, ' ', &value);
        if (value == NULL) {
            free (key);
            break;
        }

        ngf_proplist_sets (p, key, value);

        free (key);
        free (value);
    }

    uint32_t id = ngf_client_play_event (c->client, event_id, p);
    g_print ("PLAY (id=%d, event_id=%s)\n", id, event_id);

    ngf_proplist_free (p);
    free (event_id);
}

static void
parse_command_stop (TestClient *c, char *buf)
{
    char *advance = buf;
    char *str = NULL;
    uint32_t id = 0;

    advance = get_str (buf, ' ', &str);
    if (str == NULL) {
        g_print ("Usage: stop [ID]\n");
        return;
    }

    id = atoi (str);
    free (str);

    ngf_client_stop_event (c->client, id);
    g_print ("STOP (id=%d)\n", id);
}

static void
parse_input (TestClient *c, char *buf, size_t max_bytes)
{
    char *advance = buf;
    char *str = NULL;

    advance = get_str (buf, ' ', &str);
    if (str == NULL)
        return;

    if (strncmp (str, "play", 4) == 0)
        parse_command_play (c, advance);
    else if (strncmp (str, "stop", 4) == 0)
        parse_command_stop (c, advance);
    else if (strncmp (str, "quit", 4) == 0)
        g_main_loop_quit (c->loop);
    else
        g_print ("Unrecognized command: %s\n", str);

    free (str);
    g_print ("> ");
}

static gboolean
input_cb (GIOChannel *channel, GIOCondition cond, gpointer userdata)
{
    TestClient *c = (TestClient*) userdata;

    char buf[MAX_BUF_SIZE + 1];
    ssize_t bytes_read = 0;

    if (cond == G_IO_IN) {
        bytes_read = read (0, buf, MAX_BUF_SIZE);
        buf[bytes_read] = '\0';

        if (bytes_read > 0) {
            parse_input (c, buf, bytes_read);
        }

        return TRUE;
    }

    return FALSE;
}

static void
client_callback_cb (NgfClient *client, uint32_t event_id, NgfEventState state, void *userdata)
{
    (void) client;
    (void) userdata;

    switch (state) {
        case NGF_EVENT_COMPLETED:
            g_print ("\nCompleted (event_id=%d)\n", event_id);
            break;
        case NGF_EVENT_FAILED:
            g_print ("\nFailed (event_id=%d)\n", event_id);
            break;
        case NGF_EVENT_BUSY:
            g_print ("\nBusy (event_id=%d)\n", event_id);
            break;
        case NGF_EVENT_LONG:
            g_print ("\nLong (event_id=%d)\n", event_id);
            break;
        case NGF_EVENT_SHORT:
            g_print ("\nShort (event_id=%d)\n", event_id);
            break;

        default:
            g_print ("\nUnknown (event_id=%d)\n", event_id);
            break;
    }

    g_print ("> ");
}

static gboolean
application_create (TestClient *c)
{
    static GIOCondition cond = G_IO_IN | G_IO_HUP | G_IO_ERR;

    c->loop = g_main_loop_new (NULL, 0);

    /* Setup input channel for reading from stdin. */

    if (fcntl (0, F_SETFL, O_NONBLOCK) == -1)
        return FALSE;

    if ((c->channel = g_io_channel_unix_new (0)) == NULL)
        return FALSE;

    c->event_source = g_io_add_watch (c->channel, cond, input_cb, c);

    g_print ("simple ngf client\n");
    g_print ("> ");

    /* Setup DBus and client */

    c->connection = dbus_bus_get (DBUS_BUS_SESSION, NULL);
    dbus_connection_setup_with_g_main (c->connection, NULL);

    if ((c->client = ngf_client_create (NGF_TRANSPORT_DBUS, c->connection)) == NULL)
        return FALSE;

    ngf_client_set_callback (c->client, client_callback_cb, c);

    return TRUE;
}

static void
application_destroy (TestClient *c)
{
    if (c->event_source > 0)
        g_source_remove (c->event_source);

    if (c->channel)
        g_io_channel_unref (c->channel);

    if (c->client)
        ngf_client_destroy (c->client);

    if (c->connection)
        dbus_connection_unref (c->connection);

    if (c->loop)
        g_main_loop_unref (c->loop);
}

static void
application_run (TestClient *c)
{
    g_main_loop_run (c->loop);
}

int
main (int argc, char *argv[])
{
    TestClient *c = NULL;

    c = g_new0 (TestClient, 1);
    if (!application_create (c))
        return 1;

    application_run (c);
    application_destroy (c);

    g_free (c);

    return 0;
}
