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

#include "list_p.h"
#include "proplist.h"

#define MAX_KEY_LENGTH 32
#define MAX_VALUE_LENGTH 512

typedef struct _PropEntry PropEntry;

struct _PropEntry
{
    LIST_INIT (PropEntry)

    char *key;
    char *value;
};

struct _NgfProplist
{
    PropEntry *entries;
    size_t num_entries;
};

NgfProplist*
ngf_proplist_new ()
{
    NgfProplist *proplist = NULL;

    proplist = (NgfProplist*) malloc (sizeof (NgfProplist));
    if (proplist == NULL)
        return NULL;

    memset (proplist, 0, sizeof (NgfProplist));
}

static void
_free_item (PropEntry *entry, void *userdata)
{
    (void) userdata;

    free (entry->key);
    free (entry->value);
    free (entry);
}

void ngf_proplist_free (NgfProplist *proplist)
{
    if (proplist == NULL)
        return;

    LIST_FOREACH (proplist->entries, _free_item, NULL);
    free (proplist);
}

void
ngf_proplist_sets (NgfProplist *proplist,
                   const char *key,
                   const char *value)
{
    PropEntry *item = NULL;

    if (proplist == NULL || key == NULL || value == NULL)
        return;

    item = (PropEntry*) malloc (sizeof (PropEntry));
    if (item == NULL)
        return;

    item->key = strndup (key, (size_t) MAX_KEY_LENGTH);
    item->value = strndup (value, (size_t) MAX_VALUE_LENGTH);
    item->next = NULL;

    LIST_APPEND (proplist->entries, item);
}

const char*
ngf_proplist_gets (NgfProplist *proplist,
                   const char *key)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL)
        return NULL;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0)
            return (const char*) iter->value;
    }

    return NULL;
}

void
ngf_proplist_foreach (NgfProplist *proplist,
                      NgfProplistCallback callback,
                      void *userdata)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || callback == NULL)
        return;

    for (iter = proplist->entries; iter; iter = iter->next)
        callback (iter->key, iter->value, userdata);
}

const char**
ngf_proplist_get_keys (NgfProplist *proplist)
{
    PropEntry *iter = NULL;
    const char **keys = NULL;
    size_t num_keys = 0, i = 0;

    if (proplist == NULL)
        return NULL;

    for (iter = proplist->entries; iter; iter = iter->next)
        num_keys++;

    if (num_keys > 0) {
        keys = (const char**) malloc (sizeof (const char*) * (num_keys + 1));
        for (iter = proplist->entries; iter; iter = iter->next, i++)
            keys[i] = iter->key;
        keys[i] = NULL;

        return keys;
    }

    return NULL;
}

void
ngf_proplist_free_keys (const char **keys)
{
    if (keys == NULL)
        return;

    free (keys);
}
