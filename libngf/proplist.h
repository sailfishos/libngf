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

#ifndef NGF_PROPLIST_H
#define NGF_PROPLIST_H

/** Internal property list instance. */
typedef struct  _NgfProplist NgfProplist;

/** Property list callback for iterating over each entry. */
typedef void    (*NgfProplistCallback) (const char *key, const char *value, void *userdata);

/**
 * Create a new property list instance.
 * @return NgfProplist or NULL if no memory.
 */

NgfProplist*    ngf_proplist_new ();

/**
 * Free property list.
 * @param proplist NgfProplist, if NULL nothing done.
 */

void            ngf_proplist_free (NgfProplist *proplist);

/**
 * Set a string value to property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @param value Value for the key
 */

void            ngf_proplist_sets (NgfProplist *proplist, const char *key, const char *value);

/**
 * Get a string value from property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @return Key value or NULL if not found.
 */

const char*     ngf_proplist_gets (NgfProplist *proplist, const char *key);

/**
 * Iterate over each entry in the property list.
 * @param proplist NgfProplist
 * @param callback Callback function type of NgfProplistCallback
 * @param userdata User data
 */

void            ngf_proplist_foreach (NgfProplist *proplist, NgfProplistCallback callback, void *userdata);

/**
 * Get a list of all keys in the property list.
 * @param proplist NgfProplist
 * @return NULL terminated list of keys in the property list or NULL if no keys.
 * @code
 * const char **keys = NULL, **iter = NULL, *key = NULL;
 *
 * keys = ngf_proplist_get_keys (proplist);
 * for (iter = keys; iter; iter++) {
 *     key = keys[i];
 *     ...
 * }
 * @endcode
 */

const char**    ngf_proplist_get_keys (NgfProplist *proplist);

/**
 * Free a list of property keys.
 * @param keys Array of keys provided by ngf_proplist_get_keys
 */

void            ngf_proplist_free_keys (const char **keys);

#endif /* NGF_PROPLIST_H */
