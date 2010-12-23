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

#ifndef LIBNGF_H
#define LIBNGF_H

/** Ringtone event */
#define NGF_EVENT_RINGTONE      "ringtone"

/** Clock alarm */
#define NGF_EVENT_CLOCK         "clock"

/** Calendar alarm */
#define NGF_EVENT_CALENDAR      "calendar"

/** SMS event */
#define NGF_EVENT_SMS           "sms"

/** Chat event */
#define NGF_EVENT_CHAT          "chat"

/** Email event */
#define NGF_EVENT_EMAIL         "email"

#ifdef __cplusplus
extern "C" {
#endif

#include <libngf/client.h>
#include <libngf/proplist.h>

#ifdef __cplusplus
}
#endif

#endif /* LIBNGF_H */
