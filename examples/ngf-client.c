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
print_help ()
{
    g_print ("Available commands:\n");
    g_print ("      play [EVENT_NAME] [KEY:VALUE], returns [ID]*\n");
    g_print ("      stop [ID]\n");
    g_print ("      pause [ID]\n");
    g_print ("      resume [ID]\n\n");
    g_print ("Examples:\n");
    g_print ("      play ringtone media.audio:(boolean)true\n");
    g_print ("      stop 1\n");
    g_print ("      play sms media.audio:(boolean)true\n");
}

static void
parse_command_play (TestClient *c, char *buf)
{
    char *advance = buf;
    char *event = NULL, *key = NULL, *value = NULL, *ptr = NULL, *type = NULL;
    NgfProplist *p = NULL;

    /* First parameter is the event */
    advance = get_str (buf, ' ', &event);
    if (event == NULL) {
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

        /* Parse the value, it may contain optional type within parenthesis. */

        g_print ("key=%s ", key);

        if (value[0] == '(') {
            ptr = get_str ((value + 1), ')', &type);

            if (strncmp (type, "string", 6) == 0) {
                ngf_proplist_sets (p, key, ptr);
                g_print ("value=string:%s\n", ptr);
            }
            else if (strncmp (type, "integer", 7) == 0) {
                int32_t value;
                if (ngf_proplist_parse_integer (ptr, &value)) {
                    ngf_proplist_set_as_integer (p, key, value);
                    g_print ("value=integer:%d\n", value);
                }
            }
            else if (strncmp (type, "unsigned", 8) == 0) {
                uint32_t value;
                if (ngf_proplist_parse_unsigned (ptr, &value)) {
                    ngf_proplist_set_as_unsigned (p, key, value);
                    g_print ("value=unsigned:%u\n", value);
                }
            }
            else if (strncmp (type, "boolean", 7) == 0) {
                int value;
                if (ngf_proplist_parse_boolean (ptr, &value)) {
                    ngf_proplist_set_as_boolean (p, key, value);
                    g_print ("value=boolean:%s\n", value ? "TRUE" : "FALSE");
                }
            } else
                g_print ("value type unknown. skipping.\n");

            free (type);
        }
        else {
            ngf_proplist_sets (p, key, value);
            g_print ("value=string:%s\n", ptr);
        }

        free (key);
        free (value);
    }

    uint32_t event_id = ngf_client_play_event (c->client, event, p);
    g_print ("PLAY (event=%s, event_id=%u)\n", event, event_id);

    ngf_proplist_free (p);
    free (event);
}

static void
parse_command_stop (TestClient *c, char *buf)
{
    char *advance = buf;
    char *str = NULL;
    uint32_t id = 0;

    advance = get_str (buf, ' ', &str);
    (void) advance;

    if (str == NULL) {
        g_print ("Usage: stop [ID]\n");
        return;
    }

    id = atoi (str);
    free (str);

    ngf_client_stop_event (c->client, id);
    g_print ("STOP (event_id=%u)\n", id);
}

static void
parse_command_pause (TestClient *c, char *buf)
{
    char *advance = buf;
    char *str = NULL;
    uint32_t id = 0;

    advance = get_str (buf, ' ', &str);
    (void) advance;

    if (str == NULL) {
        g_print ("Usage: pause [ID]\n");
        return;
    }

    id = atoi (str);
    free (str);

    ngf_client_pause_event (c->client, id);
    g_print ("PAUSE (event_id=%u)\n", id);
}

static void
parse_command_resume (TestClient *c, char *buf)
{
    char *advance = buf;
    char *str = NULL;
    uint32_t id = 0;

    advance = get_str (buf, ' ', &str);
    (void) advance;

    if (str == NULL) {
        g_print ("Usage: resume [ID]\n");
        return;
    }

    id = atoi (str);
    free (str);

    ngf_client_resume_event (c->client, id);
    g_print ("RESUME (event_id=%u)\n", id);
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
    else if (strncmp (str, "pause", 5) == 0)
        parse_command_pause (c, advance);
    else if (strncmp (str, "resume", 6) == 0)
        parse_command_resume (c, advance);
    else if (strncmp (str, "quit", 4) == 0)
        g_main_loop_quit (c->loop);
    else if (strncmp (str, "help", 4) == 0)
        print_help ();
    else
        g_print ("Unrecognized command: %s   (try \"help\")\n", str);

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
        if (bytes_read > 0) {
            buf[bytes_read] = '\0';
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
        case NGF_EVENT_FAILED:
            g_print ("\nFailed (event_id=%d)\n", event_id);
            break;
        case NGF_EVENT_COMPLETED:
            g_print ("\nCompleted (event_id=%d)\n", event_id);
            break;
        case NGF_EVENT_PLAYING:
            g_print ("\nPlaying (event_id=%d)\n", event_id);
            break;
        case NGF_EVENT_PAUSED:
            g_print ("\nPaused (event_id=%d)\n", event_id);
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

    c->connection = dbus_bus_get (DBUS_BUS_SYSTEM, NULL);
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
