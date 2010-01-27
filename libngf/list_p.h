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

#ifndef NGF_LIST_H
#define NGF_LIST_H

#define LIST_INIT(list_type) list_type *next;

#define LIST_APPEND(list, item)                     \
    {                                               \
        typeof (item) _item = list;                 \
        if (_item == NULL) {                        \
            list = item;                            \
        }                                           \
        else {                                      \
            while (_item->next)                     \
                _item = _item->next;                \
            _item->next = item;                     \
        }                                           \
    }

#define LIST_REMOVE(list, item)                     \
    {                                               \
        typeof (item) _item = list;                 \
        typeof (item) _prev = NULL;                 \
        while (_item) {                             \
            if (_item == item) {                    \
                if (_prev)                          \
                    _prev->next = _item->next;      \
                else                                \
                    list = _item->next;             \
            }                                       \
            _prev = _item;                          \
            _item = _item->next;                    \
        }                                           \
    }

#define LIST_FOREACH(list, cb, data)                \
    {                                               \
        typeof (list) _item = list;                 \
        typeof (list) _next = list;                 \
        while (_item) {                             \
            _next = _item->next;                    \
            cb((typeof (list))_item, data);         \
            _item = _next;                          \
        }                                           \
    }

#endif /* NGF_LIST_H */
