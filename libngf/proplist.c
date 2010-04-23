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

#define VALUE_TYPE_STRING "string"
#define VALUE_TYPE_INTEGER "integer"
#define VALUE_TYPE_BOOLEAN "boolean"

#define MAX_KEY_LENGTH 32
#define MAX_VALUE_LENGTH 512

typedef struct _PropEntry PropEntry;

struct _PropEntry
{
    LIST_INIT (PropEntry)

    char *key;
    char *value;
    char *type;
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

    item->key   = strndup (key, (size_t) MAX_KEY_LENGTH);
    item->value = strndup (value, (size_t) MAX_VALUE_LENGTH);
    item->type  = VALUE_TYPE_STRING;
    item->next  = NULL;

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

static char*
_string_from_integer (int value)
{
    char   *result      = NULL;
    size_t  result_size = 0;

    char value_buf[MAX_VALUE_LENGTH + 1];
    memset (&value_buf, 0, sizeof (char) * (MAX_VALUE_LENGTH + 1));

    snprintf (value_buf, MAX_VALUE_LENGTH, "%u", value);
    result_size = strlen (value_buf);

    if ((result = (char*) malloc (sizeof (char) * (result_size + 1))) == NULL)
        return NULL;

    strncpy (result, value_buf, result_size);
    result[result_size] = '\0';
    return result;
}

void
ngf_proplist_set_as_integer (NgfProplist *proplist,
                             const char *key,
                             int value)
{
    PropEntry *item = NULL;

    if (proplist == NULL || key == NULL)
        return;

    item = (PropEntry*) malloc (sizeof (PropEntry));
    if (item == NULL)
        return;

    item->key   = strndup (key, (size_t) MAX_KEY_LENGTH);
    item->value = _string_from_integer (value);
    item->type  = VALUE_TYPE_INTEGER;
    item->next  = NULL;

    LIST_APPEND (proplist->entries, item);
}

int
ngf_proplist_get_as_integer (NgfProplist *proplist,
                             const char *key)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL)
        return 0;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0 && strncmp (iter->type, VALUE_TYPE_INTEGER, 7) == 0)
            return ngf_proplist_parse_integer (iter->value);
    }

    return 0;
}

void
ngf_proplist_set_as_boolean (NgfProplist *proplist,
                             const char *key,
                             int value)
{
    PropEntry *item = NULL;

    if (proplist == NULL || key == NULL)
        return;

    item = (PropEntry*) malloc (sizeof (PropEntry));
    if (item == NULL)
        return;

    item->key   = strndup (key, (size_t) MAX_KEY_LENGTH);
    item->value = strndup (value > 0 ? "TRUE" : "FALSE", 5);
    item->type  = VALUE_TYPE_BOOLEAN;
    item->next  = NULL;

    LIST_APPEND (proplist->entries, item);
}

int
ngf_proplist_get_as_boolean (NgfProplist *proplist,
                             const char *key)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL)
        return 0;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0 && strncmp (iter->type, VALUE_TYPE_BOOLEAN, 7) == 0)
            return ngf_proplist_parse_boolean (iter->value);
    }

    return 0;
}

const char*
ngf_proplist_get_value_type (NgfProplist *proplist,
                             const char *key)
{
    PropEntry *iter = NULL;

    if (proplist == NULL || key == NULL)
        return NULL;

    for (iter = proplist->entries; iter; iter = iter->next) {
        if (strncmp (iter->key, key, (size_t) MAX_KEY_LENGTH) == 0)
            return (const char*) iter->type;
    }

    return NULL;
}

int
ngf_proplist_parse_integer (const char *value)
{
    int result = 0;

    if (value == NULL)
        return 0;

    result = strtol (value, NULL, 10);
    return result;
}

int
ngf_proplist_parse_boolean (const char *value)
{
    if (value == NULL)
        return 0;

    if (strncmp (value, "TRUE", 4) == 0 ||
        strncmp (value, "true", 4) == 0 ||
        strncmp (value, "True", 4) == 0)
        return 1;

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
