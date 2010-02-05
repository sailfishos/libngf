/*
 * libngf - Non-graphical feedback library
 * This file is part of libngf.
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * Contact: Xun Chen <xun.chen@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved.
 * Copying, including reproducing, storing, adapting or translating,
 * any or all of this material requires the prior written consent of
 * Nokia Corporation. This material also contains confidential
 * information which may not be disclosed to others without the prior
 * written consent of Nokia.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

#include "list_p.h"
#include "proplist.h"
#include "client.h"

/** DBus name for NGF */
#define NGF_DBUS_NAME               "com.nokia.NonGraphicFeedback1"

/** DBus path for NGF */
#define NGF_DBUS_PATH               "/com/nokia/NonGraphicFeedback1"

/** DBus interface for NGF */
#define NGF_DBUS_IFACE              "com.nokia.NonGraphicFeedback1"

/** DBus method for playing event */
#define NGF_DBUS_METHOD_PLAY        "Play"

/** DBus method for stopping event */
#define NGF_DBUS_METHOD_STOP        "Stop"

/** DBus method call that is sent to us when the event state changes */
#define NGF_DBUS_INTERNAL_STATUS    "Status"


typedef struct _NgfReply NgfReply;
typedef struct _NgfEvent NgfEvent;

struct _NgfReply
{
    LIST_INIT (NgfReply)

    DBusPendingCall *pending;
    uint32_t        event_id;
    int             stop_set;
};

struct _NgfEvent
{
    LIST_INIT (NgfEvent)

    uint32_t    event_id;
    uint32_t    policy_id;
};

struct _NgfClient
{
    DBusConnection  *connection;
    NgfCallback     callback;
    void            *userdata;
    uint32_t        play_id;

    NgfReply        *pending_replies;
    NgfEvent        *active_events;
};

static void
_send_stop_event (DBusConnection *connection,
                  uint32_t policy_id)
{
    DBusMessage *msg = NULL;
    DBusMessageIter sub;
    dbus_uint32_t serial = 0;

    if ((msg = dbus_message_new_method_call (NGF_DBUS_NAME,
                                             NGF_DBUS_PATH,
                                             NGF_DBUS_IFACE,
                                             NGF_DBUS_METHOD_STOP)) == NULL)
    {
        return;
    }

    dbus_message_iter_init_append (msg, &sub);
    dbus_message_iter_append_basic (&sub, DBUS_TYPE_UINT32, &policy_id);

    dbus_connection_send (connection, msg, &serial);
    dbus_message_unref (msg);
}

static void
_pending_play_reply (DBusPendingCall *pending,
                     void *userdata)
{
    NgfClient *client = (NgfClient*) userdata;

    NgfReply *reply_iter = NULL, *reply = NULL;
    NgfEvent *event = NULL;

    DBusMessage *msg = NULL;
    DBusMessageIter iter;
    uint32_t policy_id = 0;

    for (reply_iter = client->pending_replies; reply_iter; reply_iter = reply_iter->next) {
        if (reply_iter->pending == pending) {
            reply = reply_iter;
            break;
        }
    }

    if (reply == NULL)
        goto done;

    msg = dbus_pending_call_steal_reply (pending);
    dbus_message_iter_init (msg, &iter);
    if (dbus_message_iter_get_arg_type (&iter) != DBUS_TYPE_UINT32)
        goto done;

    dbus_message_iter_get_basic (&iter, &policy_id);

    if (policy_id > 0) {
        if (reply->stop_set) {
            _send_stop_event (client->connection, policy_id);
            goto done;
        }

        event = (NgfEvent*) malloc (sizeof (NgfEvent));
        memset (event, 0, sizeof (NgfEvent));

        event->event_id = reply->event_id;
        event->policy_id = policy_id;

        LIST_APPEND (client->active_events, event);
    }

done:
    if (reply) {
        LIST_REMOVE (client->pending_replies, reply);
        free (reply);
    }

    dbus_pending_call_unref (pending);
}

static DBusHandlerResult
_message_filter_cb (DBusConnection *connection,
                    DBusMessage *msg,
                    void *userdata)
{
    NgfClient *client = (NgfClient*) userdata;

    DBusError error;
    NgfEvent *event = NULL;
    uint32_t policy_id = 0;
    uint32_t state = 0;
    int success = 0;

    if (!dbus_message_is_method_call (msg, NGF_DBUS_IFACE, NGF_DBUS_INTERNAL_STATUS))
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    dbus_error_init (&error);
    success = dbus_message_get_args (msg, &error,
        DBUS_TYPE_UINT32, &policy_id, DBUS_TYPE_UINT32, &state, DBUS_TYPE_INVALID);

    if (!success) {
        dbus_error_free (&error);
        return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
    }

    /* Try to find a matching policy id from the active events. */

    for (event = client->active_events; event; event = event->next) {
        if (event->policy_id == policy_id) {

            /* Trigger the callback, if specified, and remove the event from
               active events. */

            if (client->callback)
                client->callback (client, event->event_id, state, client->userdata);

             LIST_REMOVE (client->active_events, event);
            free (event);
            break;
        }
    }

    return DBUS_HANDLER_RESULT_HANDLED;

failed:
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

NgfClient*
ngf_client_create (NgfTransport transport,
                   ...)
{
    NgfClient *c = NULL;
    va_list transport_args;

    c = (NgfClient*) malloc (sizeof (NgfClient));
    if (c == NULL)
        goto failed;

    memset (c, 0, sizeof (NgfClient));

    va_start (transport_args, transport);

    c->connection = va_arg (transport_args, DBusConnection*);
    if (!c->connection)
        goto failed;

    va_end (transport_args);

    dbus_connection_add_filter (c->connection, _message_filter_cb, c, NULL);
    return c;

failed:
    ngf_client_destroy (c);
    return NULL;
}

static void
_stop_active_event (NgfEvent *event, void *userdata)
{
    NgfClient *client = (NgfClient*) userdata;
    _send_stop_event (client->connection, event->policy_id);
}

static void
_free_active_event (NgfEvent *event, void *userdata)
{
    free (event);
}

static void
_free_pending_reply (NgfReply *reply, void *userdata)
{
    free (reply);
}

void
ngf_client_destroy (NgfClient *client)
{
    NgfReply *reply = NULL;
    NgfEvent *event = NULL;

    if (client == NULL)
        return;

    /* Free and stop any active event. */
    LIST_FOREACH (client->active_events, _stop_active_event, client);
    LIST_FOREACH (client->active_events, _free_active_event, client);

    /* Free any pending replies. */
    LIST_FOREACH (client->pending_replies, _free_pending_reply, client);

    if (client->connection) {
        dbus_connection_remove_filter (client->connection, _message_filter_cb, client);
        client->connection = NULL;
    }

    free (client);
}

void
ngf_client_set_callback (NgfClient *client,
                         NgfCallback callback,
                         void *userdata)
{
    client->callback = callback;
    client->userdata = userdata;
}

static void
_append_property (const char *key,
                  const char *value,
                  void *userdata)
{
    DBusMessageIter *iter = (DBusMessageIter*) userdata;
    DBusMessageIter sub, ssub;

    dbus_message_iter_open_container (iter, DBUS_TYPE_DICT_ENTRY, 0, &sub);
    dbus_message_iter_append_basic (&sub, DBUS_TYPE_STRING, &key);

    dbus_message_iter_open_container (&sub, DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &ssub);
    dbus_message_iter_append_basic (&ssub, DBUS_TYPE_STRING, &value);
    dbus_message_iter_close_container(&sub, &ssub);

    dbus_message_iter_close_container (iter, &sub);
}

uint32_t
ngf_client_play_event (NgfClient *client,
                       const char *event,
                       NgfProplist *proplist)
{
    DBusPendingCall *pending = NULL;
    DBusMessage *msg = NULL;
    NgfReply *reply = NULL;
    const char *key = NULL, *value = NULL;

    DBusMessageIter iter, sub, ssub;
    uint32_t event_id = 0;

    if (client == NULL || event == NULL)
        return 0;

    event_id = ++client->play_id;

    /* Send the actual message to the service. */

    if ((msg = dbus_message_new_method_call (NGF_DBUS_NAME,
                                             NGF_DBUS_PATH,
                                             NGF_DBUS_IFACE,
                                             NGF_DBUS_METHOD_PLAY)) == NULL)
    {
        return 0;
    }

    dbus_message_iter_init_append (msg, &iter);
    dbus_message_iter_append_basic (&iter, DBUS_TYPE_STRING, &event);

    /* Append all properties from the property list */
    dbus_message_iter_open_container (&iter, DBUS_TYPE_ARRAY, "{sv}", &sub);
    ngf_proplist_foreach (proplist, _append_property, &sub);
    dbus_message_iter_close_container (&iter, &sub);

    dbus_connection_send_with_reply (client->connection, msg, &pending, -1);
    dbus_message_unref (msg);

    if (pending == NULL)
        return 0;

    reply = (NgfReply*) malloc (sizeof (NgfReply));
    memset (reply, 0, sizeof (NgfReply));

    reply->pending = pending;
    reply->event_id = event_id;

    LIST_APPEND (client->pending_replies, reply);

    dbus_pending_call_set_notify (pending, _pending_play_reply, client, NULL);

    return event_id;
}

void
ngf_client_stop_event (NgfClient *client,
                       uint32_t id)
{
    NgfEvent *event = NULL;
    NgfReply *reply = NULL;

    if (client == NULL)
        return;

    /* First, go through the active events */

    event = client->active_events;
    while (event) {
        if (event->event_id == id) {
            _stop_active_event (event, client);
            LIST_REMOVE (client->active_events, event);
            _free_active_event (event, client);
            break;
        }

        event = event->next;
    }

    /* Look the event id from the pending replies */

    reply = client->pending_replies;
    while (reply) {
        if (reply->event_id == id) {
            reply->stop_set = TRUE;
            break;
        }

        reply = reply->next;
    }
}
