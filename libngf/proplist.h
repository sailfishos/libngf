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

#ifndef NGF_PROPLIST_H
#define NGF_PROPLIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum _NgfProplistType {
    NGF_PROPLIST_VALUE_TYPE_STRING = 0,
    NGF_PROPLIST_VALUE_TYPE_INTEGER,
    NGF_PROPLIST_VALUE_TYPE_UNSIGNED,
    NGF_PROPLIST_VALUE_TYPE_BOOLEAN,
    NGF_PROPLIST_VALUE_TYPE_INVALID
} NgfProplistType;

/** Internal property list instance. */
typedef struct  _NgfProplist NgfProplist;

/** Property list callback for iterating over each entry. */
typedef void    (*NgfProplistCallback) (const char *key, const void *value, void *userdata);

/** Extended iteration callback with type information. */
typedef void    (*NgfProplistExtendedCallback) (const char *key, const void *value, NgfProplistType type, void *userdata);

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

void            ngf_proplist_set_as_integer (NgfProplist *proplist, const char *key, int32_t value);

/**
 * Get integer value from the property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @param integer_value Value for the key if key exists in proplist
 * @return success Return 1 if getting value was successful, 0 if failed.
 */

int             ngf_proplist_get_as_integer (NgfProplist *proplist, const char *key, int32_t *integer_value);

/**
 * Set a unsigned integer value to property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @param value Value for the key
 */

void            ngf_proplist_set_as_unsigned (NgfProplist *proplist, const char *key, uint32_t value);

/**
 * Get unsigned integer value from the property list.
 * @param proplist NgfProplist
 * @param key Key name
 * @param unsigned_value Value for the key if key exists in proplist
 * @return success Return 1 if getting value was successful, 0 if failed.
 */

int             ngf_proplist_get_as_unsigned (NgfProplist *proplist, const char *key, uint32_t *unsigned_value);

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
 * @param boolean_value 1 for TRUE, 0 for FALSE.
 * @return success Return 1 if getting value was successful, 0 if failed.
 */

int             ngf_proplist_get_as_boolean (NgfProplist *proplist, const char *key, int *boolean_value);

/**
 * Get value type of the property.
 * @param proplist NgfProplist
 * @param key Key name
 * @return Value type or NULL if no such key.
 */

NgfProplistType  ngf_proplist_get_value_type (NgfProplist *proplist, const char *key);

/**
 * Parse integer value.
 * @param value Value to parse.
 * @param integer_value Parsed integer value.
 * @return 1 on success, 0 if failed to parse.
 */

int             ngf_proplist_parse_integer (const char *value, int32_t *integer_value);

/**
 * Parse unsigned integer value.
 * @param value Value to parse.
 * @param integer_value Parsed integer value.
 * @return 1 on success, 0 if failed to parse.
 */

int             ngf_proplist_parse_unsigned (const char *value, uint32_t *unsigned_value);

/**
 * Parse boolean value.
 * @param value Value to parse.
 * @param boolean_value Parsed boolean value, 1 for TRUE 0 for FALSE.
 * @return 1 on success, 0 if failed to parse.
 */

int             ngf_proplist_parse_boolean (const char *value, int *boolean_value);

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
