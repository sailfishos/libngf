#include <values.h>
#include <stdio.h>
#include <stdlib.h>
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
    snprintf (max_int, 255, "%d", INT_MAX);

    char min_int[255];
    memset (&min_int, 0, sizeof (char) * 255);
    snprintf (min_int, 255, "%d", INT_MIN);

    /* integer */
    fail_unless (ngf_proplist_parse_integer (NULL) == 0);
    fail_unless (ngf_proplist_parse_integer ("") == 0);
    fail_unless (ngf_proplist_parse_integer ("1") == 1);
    fail_unless (ngf_proplist_parse_integer ("555") == 555);
    fail_unless (ngf_proplist_parse_integer (max_int) == INT_MAX);
    fail_unless (ngf_proplist_parse_integer (min_int) == INT_MIN);
    fail_unless (ngf_proplist_parse_integer ("hello") == 0);

    /* boolean */
    fail_unless (ngf_proplist_parse_boolean (NULL) == 0);
    fail_unless (ngf_proplist_parse_boolean ("") == 0);
    fail_unless (ngf_proplist_parse_boolean ("TRUE") == 1);
    fail_unless (ngf_proplist_parse_boolean ("true") == 1);
    fail_unless (ngf_proplist_parse_boolean ("True") == 1);
    fail_unless (ngf_proplist_parse_boolean ("FALSE") == 0);
    fail_unless (ngf_proplist_parse_boolean ("random") == 0);
    fail_unless (ngf_proplist_parse_boolean ("5") == 0);
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
_proplist_cb (const char *key, const char *value, void *userdata)
{
    int *num_recognized = (int*) userdata;

    if (strcmp (key, "first") == 0 && strcmp (value, "1") == 0) {
        (*num_recognized)++;
    }

    else if (strcmp (key, "second") == 0 && strcmp (value, "5") == 0) {
        (*num_recognized)++;
    }

    else if (strcmp (key, "third") == 0 && strcmp (value, "TRUE") == 0) {
        (*num_recognized)++;
    }
}

START_TEST (test_foreach)
{
    NgfProplist *proplist = NULL;
    int num_recognized = 0;

    proplist = ngf_proplist_new ();
    fail_unless (proplist != NULL);

    ngf_proplist_sets (proplist, "first", "1");
    ngf_proplist_set_as_integer (proplist, "second", 5);
    ngf_proplist_set_as_boolean (proplist, "third", 1);

    ngf_proplist_foreach (proplist, _proplist_cb, &num_recognized);
    fail_unless (num_recognized == 3);

    ngf_proplist_free (proplist);
}
END_TEST

static void
_extended_cb (const char *key, const char *value, const char *type, void *userdata)
{
    int *num_recognized = (int*) userdata;

    if (strncmp (key, "string.1", 8) == 0) {
        fail_unless (strncmp (type, "string", 6) == 0);
        fail_unless (strncmp (value, "string value", 12) == 0);

        (*num_recognized)++;
    }

    if (strncmp (key, "integer.1", 9) == 0) {
        fail_unless (strncmp (type, "integer", 7) == 0);
        fail_unless (ngf_proplist_parse_integer (value) == 555);

        (*num_recognized)++;
    }

    if (strncmp (key, "boolean.1", 9) == 0) {
        fail_unless (strncmp (type, "boolean", 7) == 0);
        fail_unless (ngf_proplist_parse_boolean (value) == 1);

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
    fail_unless (strncmp (ngf_proplist_get_value_type (proplist, "string.1"), "string", 6) == 0);

    ngf_proplist_set_as_integer (proplist, "integer.1", 555);
    fail_unless (strncmp (ngf_proplist_get_value_type (proplist, "integer.1"), "integer", 7) == 0);

    ngf_proplist_set_as_boolean (proplist, "boolean.1", 1);
    fail_unless (strncmp (ngf_proplist_get_value_type (proplist, "boolean.1"), "boolean", 7) == 0);

    ngf_proplist_foreach_extended (proplist, _extended_cb, &num_recognized);
    fail_unless (num_recognized == 3);

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

    tc = tcase_create ("Parse integer and boolean values");
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
