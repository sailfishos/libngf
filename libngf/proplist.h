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

#ifdef __cplusplus
extern "C" {
#endif

/** Internal property list instance. */
typedef struct  _NgfProplist NgfProplist;

/** Property list callback for iterating over each entry. */
typedef void    (*NgfProplistCallback) (const char *key, const char *value, void *userdata);

/** Extended iteration callback with type information. */
typedef void    (*NgfProplistExtendedCallback) (const char *key, const char *value, const char *type, void *userdata);

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
 * Set a integer value to property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @param value Value for the key
 */

void            ngf_proplist_set_as_integer (NgfProplist *proplist, const char *key, int value);

/**
 * Get integer value from the property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @return value Value for the key or 0 if failed.
 */

int             ngf_proplist_get_as_integer (NgfProplist *proplist, const char *key);

/**
 * Set a boolean value to property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @param value Value for the key
 */

void            ngf_proplist_set_as_boolean (NgfProplist *proplist, const char *key, int value);

/**
 * Get a boolean value from the property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @return 1 for TRUE, 0 for FALSE or failed.
 */

int             ngf_proplist_get_as_boolean (NgfProplist *proplist, const char *key);

/**
 * Get value type of the property.
 * @param proplist NgfProplist
 * @param key Key name
 * @return Value type or NULL if no such key.
 */

const char*     ngf_proplist_get_value_type (NgfProplist *proplist, const char *key);

/**
 * Parse integer value.
 * @param value Value to parse.
 * @return Integer or 0 if unable to parse.
 */

int             ngf_proplist_parse_integer (const char *value);

/**
 * Parse boolean value.
 * @param value Value to parse.
 * @return 1 for TRUE, 0 for FALSE or unable to parse.
 */

int             ngf_proplist_parse_boolean (const char *value);

/**
 * Iterate over each entry in the property list.
 * @param proplist NgfProplist
 * @param callback Callback function type of NgfProplistCallback
 * @param userdata User data
 */

void            ngf_proplist_foreach (NgfProplist *proplist, NgfProplistCallback callback, void *userdata);

/**
 * Iterate over each entry in the property list and supply a value type.
 * @param proplist NgfProplist
 * @param callback NgfProplistExtendedCallback
 * @param userdata User data
 */

void            ngf_proplist_foreach_extended (NgfProplist *proplist, NgfProplistExtendedCallback callback, void *userdata);

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

#ifdef __cplusplus
}
#endif

#endif /* NGF_PROPLIST_H */
