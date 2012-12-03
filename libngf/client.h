/*
 * libngf - Non-graphical feedback library
 *
 * Copyright (C) 2010 Nokia Corporation. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef NGF_CLIENT_H
#define NGF_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

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
    /** Event fails when we are unable to get resources for it or we just can't play it. */
    NGF_EVENT_FAILED    = 0,

    /** Event is completed when the event has been played or cancelled by higher priority event. */
    NGF_EVENT_COMPLETED = 1,

    /** Event is in playing state when playback is successfully started or continued. */
    NGF_EVENT_PLAYING   = 2,

    /** Event is in paused state when pause is called. */
    NGF_EVENT_PAUSED    = 3,

    /** Event is busy, because there is a more higher priority event playing. */
    NGF_EVENT_BUSY      = (1 << 2),

    /** Event will be played using a long tone */
    NGF_EVENT_LONG      = (1 << 3),

    /** Event will be played using a short tone */
    NGF_EVENT_SHORT     = (1 << 4)
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
 * Stop an active event.
 *
 * @param client NgfClient instance
 * @param id Event id. If no such event, nothing is done.
 */

void ngf_client_stop_event (NgfClient *client,
                            uint32_t id);

/**
 * Pause active event.
 *
 * @param client NgfClient instance
 * @param id Event id.
 */

void ngf_client_pause_event (NgfClient *client,
                             uint32_t id);

/**
 * Resume paused event.
 *
 * @param client NgfClient instance
 * @param id Event id.
 */

void ngf_client_resume_event (NgfClient *client,
                              uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* NGF_CLIENT_H */
