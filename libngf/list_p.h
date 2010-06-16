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
