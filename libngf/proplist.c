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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include "list_p.h"
#include "proplist.h"

#define VALUE_TYPE_STRING "string"
#define VALUE_TYPE_INTEGER "integer"
#define VALUE_TYPE_UNSIGNED "unsigned"
#define VALUE_TYPE_BOOLEAN "boolean"

#define MAX_KEY_LENGTH 32
#define MAX_VALUE_LENGTH 512

typedef struct _PropEntry PropEntry;

struct _PropEntry
{
    LIST_INIT (PropEntry)

    char *key;
    void *value;
    NgfProplistType type;
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
    return proplist;
}

static void
_free_item (PropEntry *entry, void *userdata)
{
    (void) userdata;

    if (!entry)
        return;

    if (entry->key)
        free (entry->key);
    if (entry->value)
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

int
ngf_proplist_sets (NgfProplist *proplist,
                   const char *key,
                   const char *value)
{
    PropEntry *item = NULL;

    if (proplist == NULL || key == NULL || value == NULL)
        goto error;

    item = (PropEntry*) malloc (sizeof (PropEntry));
    if (item == NULL)
        goto error;

    if ((item->key = strndup (key, (size_t) MAX_KEY_LENGTH)) == NULL)
        goto error;
    if ((item->value = strndup (value, (size_t) MAX_VALUE_LENGTH)) == NULL)
        goto error;
    item->type  = NGF_PROPLIST_VALUE_TYPE_STRING;
    item->next  = NULL;

    LIST_APPEND (proplist->entries, item);
    return 1;

error:
    if (item && item->key)
        free (item->key);
    if (item && item->value)
        free (item->value);
    if (item)
        free (item);
    return 0;
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

int
ngf_proplist_set_as_integer (NgfProplist *proplist,
                             const char *key,
                             int32_t value)
{
    PropEntry *item = NULL;
    int32_t *data;

    if (proplist == NULL || key == NULL)
        goto error;

    item = (PropEntry*) malloc (sizeof (PropEntry));
    if (item == NULL)
        goto error;

    if ((item->key = strndup (key, (size_t) MAX_KEY_LENGTH)) == NULL)
        goto error;
    if ((data = malloc (sizeof(int32_t))) == NULL)
        goto error;
    *data = value;
    item->value = data;
    item->type  = NGF_PROPLIST_VALUE_TYPE_INTEGER;
    item->next  = NULL;

    LIST_APPEND (proplist->entries, item);
    return 1;

error:
    if (item && item->key)
        free (item->key);
    if (data)
        free (data);
    if (item)
        free (item);
    return 0;
}

int
ngf_proplist_set_as_unsigned (NgfProplist *proplist,
                              const char *key,
                              uint32_t value)
{
    PropEntry *item = NULL;
    uint32_t *data;

    if (proplist == NULL || key == NULL)
        goto error;

    item = (PropEntry*) malloc (sizeof (PropEntry));
    if (item == NULL)
        goto error;

    if ((item->key = strndup (key, (size_t) MAX_KEY_LENGTH)) == NULL)
        goto error;
    if ((data = malloc (sizeof(uint32_t))) == NULL)
        goto error;
    *data = value;
    item->value = data;
    item->type  = NGF_PROPLIST_VALUE_TYPE_UNSIGNED;
    item->next  = NULL;

    LIST_APPEND (proplist->entries, item);
    return 1;

error:
    if (item && item->key)
        free (item->key);
    if (data)
        free (data);
    if (item)
        free (item);
    return 0;
}

int
ngf_proplist_get_as_integer (NgfProplist *proplist,
                             const char *key,
                             int32_t *integer_value)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL || integer_value == NULL)
        return 0;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0
            && iter->type == NGF_PROPLIST_VALUE_TYPE_INTEGER) {
            *integer_value = *(const int32_t*) iter->value;
            return 1;
        }
    }

    return 0;
}

int
ngf_proplist_get_as_unsigned (NgfProplist *proplist,
                              const char *key,
                              uint32_t *unsigned_value)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL || unsigned_value == NULL)
        return 0;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0
            && iter->type == NGF_PROPLIST_VALUE_TYPE_UNSIGNED) {
            *unsigned_value = *(const uint32_t*) iter->value;
            return 1;
        }
    }

    return 0;
}

int
ngf_proplist_set_as_boolean (NgfProplist *proplist,
                             const char *key,
                             int value)
{
    PropEntry *item = NULL;
    int *data;

    if (proplist == NULL || key == NULL)
        goto error;

    item = (PropEntry*) malloc (sizeof (PropEntry));
    if (item == NULL)
        goto error;

    if ((item->key = strndup (key, (size_t) MAX_KEY_LENGTH)) == NULL)
        goto error;
    if ((data = malloc (sizeof(int))) == NULL)
        goto error;
    *data = value > 0 ? 1 : 0;
    item->value = data;
    item->type  = NGF_PROPLIST_VALUE_TYPE_BOOLEAN;
    item->next  = NULL;

    LIST_APPEND (proplist->entries, item);
    return 1;

error:
    if (item && item->key)
        free (item->key);
    if (data)
        free (data);
    if (item)
        free (item);
    return 0;
}

int
ngf_proplist_get_as_boolean (NgfProplist *proplist,
                             const char *key,
                             int *boolean_value)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL || boolean_value == NULL)
        return 0;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0
            && iter->type == NGF_PROPLIST_VALUE_TYPE_BOOLEAN) {
            *boolean_value = *(const int*) iter->value;
            return 1;
        }
    }

    return 0;
}

NgfProplistType
ngf_proplist_get_value_type (NgfProplist *proplist,
                             const char *key)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL)
        return NGF_PROPLIST_VALUE_TYPE_INVALID;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0)
            return iter->type;
    }

    return NGF_PROPLIST_VALUE_TYPE_INVALID;
}

int
ngf_proplist_parse_integer (const char *value, int32_t *integer_value)
{
    char *endptr = NULL;

    if (value == NULL || value[0] == '\0' || integer_value == NULL)
        return 0;

    errno = 0;
    *integer_value = strtol (value, &endptr, 10);
    if (errno != 0 || endptr[0] != '\0')
        return 0;

    return 1;
}

int
ngf_proplist_parse_unsigned (const char *value, uint32_t *unsigned_value)
{
    char *endptr = NULL;

    if (value == NULL || value[0] == '\0' || unsigned_value == NULL)
        return 0;

    errno = 0;
    *unsigned_value = strtoul (value, &endptr, 10);
    if (errno != 0 || endptr[0] != '\0')
        return 0;

    return 1;
}

int
ngf_proplist_parse_boolean (const char *value, int *boolean_value)
{
    if (value == NULL || boolean_value == NULL)
        return 0;

    if (strncmp (value, "1", 1) == 0 || strncasecmp (value, "true", 4) == 0) {

        *boolean_value = 1;
        return 1;
    } else if (strncmp (value, "0", 1) == 0 || strncasecmp (value, "false", 5) == 0) {

        *boolean_value = 0;
        return 1;
    }

    return 0;
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

void
ngf_proplist_foreach_extended (NgfProplist *proplist,
                               NgfProplistExtendedCallback callback,
                               void *userdata)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || callback == NULL)
        return;

    for (iter = proplist->entries; iter; iter = iter->next)
        callback (iter->key, iter->value, iter->type, userdata);
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
