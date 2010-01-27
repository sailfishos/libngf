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

#ifndef NGF_CLIENT_H
#define NGF_CLIENT_H

#include <stdint.h>
#include <libngf/proplist.h>

typedef enum _NgfTransport
{
    /** DBus transport is the only supported transport currently. */
    NGF_TRANSPORT_DBUS,

    /** Reserved for internal use. */
    NGF_TRANSPORT_INTERNAL
} NgfTransport;

typedef enum _NgfEventState
{
    /** Event is completed when the event has been played or cancelled by higher priority event. */
    NGF_EVENT_COMPLETED = 0,

    /** Event fails when we are unable to get resources for it or we just can't play it. */
    NGF_EVENT_FAILED
} NgfEventState;

/** Internal client structure. */
typedef struct _NgfClient NgfClient;

/** Event state callback for receiving event completion status (failed, completed) */
typedef void (*NgfCallback) (NgfClient *client, uint32_t id, NgfEventState state, void *userdata);

/**
 * Create a client instance to play events.
 *
 * @param transport NgfTransport. Currently only NGF_TRANSPORT_DBUS supported.
 * @param ... Variable arguments passed to transports.
 * @return NgfClient instance or NULL on error.
 *
 * @code
 * DBusConnection *conn = dbus_bus_get (DBUS_BUS_SYSTEM, NULL);
 * dbus_connection_setup_with_g_main (conn, NULL);
 *
 * NgfClient *client = ngf_client_create (NGF_TRANSPORT_DBUS, conn);
 * if (!client) {
 *     fprintf (stderr, "Failed to create client!");
 * }
 * @endcode
 */

NgfClient* ngf_client_create (NgfTransport transport, ...);

/**
 * Free the clients resources.
 *
 * @param client NgfClient instance
 */

void ngf_client_destroy (NgfClient *client);

/**
 * Set a callback to receive event completion updates.
 *
 * @param client NgfClient instance
 * @param callback Callback function, @see NgfCallback
 * @param userdata Userdata
 */

void ngf_client_set_callback (NgfClient *client,
                              NgfCallback callback,
                              void *userdata);

/**
 * Play event with optional properties.
 *
 * @param client NgfClient instance
 * @param event Event identifier
 * @param proplist NgfProplist or NULL.
 * @return Id of the event
 *
 * @code
 * NgfProplist *p = NULL;
 * uint32_t id = 0;
 *
 * p = ngf_proplist_new ();
 * ngf_proplist_sets (p, "audio", "/usr/share/sounds/beep.wav");
 *
 * id = ngf_client_play_event (client, "my.event", p);
 * if (id == 0) {
 *     fprintf (stderr, "Failed to play event!");
 * }
 *
 * ngf_proplist_free (p);
 * @endcode
 */

uint32_t ngf_client_play_event (NgfClient *client,
                                const char *event,
                                NgfProplist *proplist);

/**
 * Stop a currently active event.
 *
 * @param client NgfClient instance
 * @param id Event id. If no such event, nothing is done.
 */

void ngf_client_stop_event (NgfClient *client,
                            uint32_t id);

#endif /* NGF_CLIENT_H */
