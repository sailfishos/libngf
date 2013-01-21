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

#include <values.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <check.h>
#include <libngf/proplist.h>

START_TEST (test_set_get)
{
    NgfProplist *proplist = NULL;
    const char *res = NULL;

    proplist = ngf_proplist_new ();
    fail_unless (proplist != NULL);

    res = ngf_proplist_gets (proplist, "my.string");
    fail_unless (res == NULL);

    ngf_proplist_sets (proplist, "my.string", "my.value");
    res = ngf_proplist_gets (proplist, "my.string");
    fail_unless (res != NULL);
    fail_unless (strcmp (res, "my.value") == 0);

    ngf_proplist_free (proplist);
}
END_TEST

START_TEST (test_parse_values)
{
    int result = 0;

    char max_int[255];
    memset (&max_int, 0, sizeof (char) * 255);
    snprintf (max_int, 255, "%d", INT32_MAX);

    char min_int[255];
    memset (&min_int, 0, sizeof (char) * 255);
    snprintf (min_int, 255, "%d", INT32_MIN);

    char max_uint[255];
    memset (&max_uint, 0, sizeof (char) * 255);
    snprintf (max_uint, 255, "%u", UINT32_MAX);

    char min_uint[5];
    memset (&min_uint, 0, sizeof (char) * 5);
    snprintf (min_uint, 5, "%u", 0);

    /* integer */
    int32_t int_value;
    fail_unless (ngf_proplist_parse_integer (NULL, NULL) == 0);
    fail_unless (ngf_proplist_parse_integer ("", &int_value) == 0);
    fail_unless (ngf_proplist_parse_integer ("0", &int_value) == 1 && int_value == 0);
    fail_unless (ngf_proplist_parse_integer ("1", &int_value) == 1 && int_value == 1);
    fail_unless (ngf_proplist_parse_integer ("555", &int_value) == 1 && int_value == 555);
    fail_unless (ngf_proplist_parse_integer ("-555", &int_value) == 1 && int_value == -555);
    fail_unless (ngf_proplist_parse_integer (max_int, &int_value) == 1 && int_value == INT32_MAX);
    fail_unless (ngf_proplist_parse_integer (min_int, &int_value) == 1 && int_value == INT32_MIN);
    fail_unless (ngf_proplist_parse_integer ("hello", &int_value) == 0);

    /* unsigned integer */
    uint32_t uint_value;
    fail_unless (ngf_proplist_parse_unsigned (NULL, NULL) == 0);
    fail_unless (ngf_proplist_parse_unsigned ("", &uint_value) == 0);
    fail_unless (ngf_proplist_parse_unsigned ("1", &uint_value) == 1 && uint_value == 1);
    fail_unless (ngf_proplist_parse_unsigned ("555", &uint_value) == 1 && uint_value == 555);
    fail_unless (ngf_proplist_parse_unsigned (max_uint, &uint_value) == 1 && uint_value == UINT32_MAX);
    fail_unless (ngf_proplist_parse_unsigned (min_uint, &uint_value) == 1 && uint_value == 0);
    fail_unless (ngf_proplist_parse_unsigned ("hello", &uint_value) == 0);

    /* boolean */
    int bool_value;
    fail_unless (ngf_proplist_parse_boolean (NULL, NULL) == 0);
    fail_unless (ngf_proplist_parse_boolean ("", &bool_value) == 0);
    fail_unless (ngf_proplist_parse_boolean ("TRUE", &bool_value) == 1 && bool_value == 1);
    fail_unless (ngf_proplist_parse_boolean ("true", &bool_value) == 1 && bool_value == 1);
    fail_unless (ngf_proplist_parse_boolean ("True", &bool_value) == 1 && bool_value == 1);
    fail_unless (ngf_proplist_parse_boolean ("1", &bool_value) == 1 && bool_value == 1);
    fail_unless (ngf_proplist_parse_boolean ("false", &bool_value) == 1 && bool_value == 0);
    fail_unless (ngf_proplist_parse_boolean ("FalsE", &bool_value) == 1 && bool_value == 0);
    fail_unless (ngf_proplist_parse_boolean ("FALSE", &bool_value) == 1 && bool_value == 0);
    fail_unless (ngf_proplist_parse_boolean ("random", &bool_value) == 0);
    fail_unless (ngf_proplist_parse_boolean ("5", &bool_value) == 0);
}
END_TEST

START_TEST (test_limits)
{
    NgfProplist *proplist = NULL;
    const char *res = NULL;
    char *str_buf = NULL;

    /* Test upper limit, expect 256 bytes for max value */
    str_buf = malloc (sizeof (char) * 65536);
    fail_unless (str_buf != NULL);

    memset (str_buf, 'a', sizeof (char) * 65536);
    str_buf[65535] = '\0';

    proplist = ngf_proplist_new ();
    fail_unless (proplist != NULL);

    ngf_proplist_sets (proplist, "big.value", str_buf);
    res = ngf_proplist_gets (proplist, "big.value");

    fail_unless (res != NULL);
    fail_unless (strlen (res) == 512, "Maximum size does not match");

    free (str_buf);

    /* Test lower limit */
    ngf_proplist_sets (proplist, "empty.value", "");
    res = ngf_proplist_gets (proplist, "empty.value");
    fail_unless (res != NULL);
    fail_unless (strcmp (res, "") == 0);

    ngf_proplist_sets (proplist, "null.value", NULL);
    res = ngf_proplist_gets (proplist, "null.value");
    fail_unless (res == NULL);

    ngf_proplist_free (proplist);
}
END_TEST

static void
_proplist_cb (const char *key, const void *value, void *userdata)
{
    int *num_recognized = (int*) userdata;

    const char *string_value = NULL;
    uint32_t unsigned_value;
    int32_t integer_value;
    int boolean_value;

    if (strcmp (key, "first") == 0) {
        string_value = (const char*) value;
        if (strcmp (value, "1") == 0)
            (*num_recognized)++;
    }

    else if (strcmp (key, "second") == 0) {
        integer_value = *(const int32_t*) value;
        if (integer_value == -5)
            (*num_recognized)++;
    }

    else if (strcmp (key, "third") == 0) {
        unsigned_value = *(const uint32_t*) value;
        if (unsigned_value == 9)
            (*num_recognized)++;
    }

    else if (strcmp (key, "fourth") == 0) {
        boolean_value = *(const int*) value;
        if (boolean_value == 1)
            (*num_recognized)++;
    }
}

START_TEST (test_foreach)
{
    NgfProplist *proplist = NULL;
    int num_recognized = 0;

    proplist = ngf_proplist_new ();
    fail_unless (proplist != NULL);

    fail_unless (ngf_proplist_sets (proplist, "first", "1") == 1);
    fail_unless (ngf_proplist_set_as_integer (proplist, "second", -5) == 1);
    fail_unless (ngf_proplist_set_as_unsigned (proplist, "third", 9) == 1);
    fail_unless (ngf_proplist_set_as_boolean (proplist, "fourth", 1) == 1);

    ngf_proplist_foreach (proplist, _proplist_cb, &num_recognized);
    fail_unless (num_recognized == 4);

    ngf_proplist_free (proplist);
}
END_TEST

static void
_extended_cb (const char *key, const void *value, NgfProplistType type, void *userdata)
{
    int *num_recognized = (int*) userdata;

    if (strncmp (key, "string.1", 8) == 0) {
        fail_unless (type == NGF_PROPLIST_VALUE_TYPE_STRING);
        fail_unless (strncmp ((const char*) value, "string value", 12) == 0);

        (*num_recognized)++;
    }

    if (strncmp (key, "integer.1", 9) == 0) {
        int32_t int_value;
        fail_unless (type == NGF_PROPLIST_VALUE_TYPE_INTEGER);
        int_value = *(const int32_t*) value;
        fail_unless (int_value == -555);

        (*num_recognized)++;
    }

    if (strncmp (key, "unsigned.1", 10) == 0) {
        uint32_t uint_value;
        fail_unless (type == NGF_PROPLIST_VALUE_TYPE_UNSIGNED);
        uint_value = *(const uint32_t*) value;
        fail_unless (uint_value == 555);

        (*num_recognized)++;
    }

    if (strncmp (key, "boolean.1", 9) == 0) {
        int bool_value;
        fail_unless (type == NGF_PROPLIST_VALUE_TYPE_BOOLEAN);
        bool_value = *(const int*) value;
        fail_unless (bool_value == 1);

        (*num_recognized)++;
    }
}

START_TEST (test_foreach_extended)
{
    NgfProplist *proplist = NULL;
    int num_recognized = 0;

    proplist = ngf_proplist_new ();
    fail_unless (proplist != NULL);

    ngf_proplist_sets (proplist, "string.1", "string value");
    fail_unless (ngf_proplist_get_value_type (proplist, "string.1") == NGF_PROPLIST_VALUE_TYPE_STRING);

    ngf_proplist_set_as_integer (proplist, "integer.1", -555);
    fail_unless (ngf_proplist_get_value_type (proplist, "integer.1") == NGF_PROPLIST_VALUE_TYPE_INTEGER);

    ngf_proplist_set_as_unsigned (proplist, "unsigned.1", 555);
    fail_unless (ngf_proplist_get_value_type (proplist, "unsigned.1") == NGF_PROPLIST_VALUE_TYPE_UNSIGNED);

    ngf_proplist_set_as_boolean (proplist, "boolean.1", 1);
    fail_unless (ngf_proplist_get_value_type (proplist, "boolean.1") == NGF_PROPLIST_VALUE_TYPE_BOOLEAN);

    ngf_proplist_foreach_extended (proplist, _extended_cb, &num_recognized);
    fail_unless (num_recognized == 4);

    ngf_proplist_free (proplist);
}
END_TEST

START_TEST (test_get_keys)
{
    NgfProplist *proplist = NULL;
    const char **keys = NULL, **k = NULL;

    proplist = ngf_proplist_new ();
    fail_unless (proplist != NULL);

    ngf_proplist_sets (proplist, "first", "1");
    ngf_proplist_sets (proplist, "second", "2");
    ngf_proplist_sets (proplist, "third", "3");

    keys = ngf_proplist_get_keys (proplist);
    fail_unless (keys != NULL);

    k = keys;
    fail_unless (strcmp (*k, "first") == 0);

    k++;
    fail_unless (strcmp (*k, "second") == 0);

    k++;
    fail_unless (strcmp (*k, "third") == 0);

    k++;
    fail_unless (*k == NULL);

    ngf_proplist_free_keys (keys);
    ngf_proplist_free (proplist);
}
END_TEST

int
main (int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    int num_failed = 0;

    Suite *s = NULL;
    TCase *tc = NULL;
    SRunner *sr = NULL;

    s = suite_create ("Property list");
    tc = tcase_create ("Setting and getting values");
    tcase_add_test (tc, test_set_get);
    suite_add_tcase (s, tc);

    tc = tcase_create ("Parse integer, unsigned and boolean values");
    tcase_add_test (tc, test_parse_values);
    suite_add_tcase (s, tc);

    tc = tcase_create ("Minimum and maximum limits");
    tcase_add_test (tc, test_limits);
    suite_add_tcase (s, tc);

    tc = tcase_create ("Foreach iteration of property list");
    tcase_add_test (tc, test_foreach);
    suite_add_tcase (s, tc);

    tc = tcase_create ("Extended foreach iteration of property list");
    tcase_add_test (tc, test_foreach_extended);
    suite_add_tcase (s, tc);

    tc = tcase_create ("Get list of keys");
    tcase_add_test (tc, test_get_keys);
    suite_add_tcase (s, tc);

    sr = srunner_create (s);
    srunner_run_all (sr, CK_NORMAL);
    num_failed = srunner_ntests_failed (sr);
    srunner_free (sr);

    return num_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
