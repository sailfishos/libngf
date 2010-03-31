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

    else if (strcmp (key, "second") == 0 && strcmp (value, "2") == 0) {
        (*num_recognized)++;
    }

    else if (strcmp (key, "third") == 0 && strcmp (value, "3") == 0) {
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
    ngf_proplist_sets (proplist, "second", "2");
    ngf_proplist_sets (proplist, "third", "3");

    ngf_proplist_foreach (proplist, _proplist_cb, &num_recognized);
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

    tc = tcase_create ("Minimum and maximum limits");
    tcase_add_test (tc, test_limits);
    suite_add_tcase (s, tc);

    tc = tcase_create ("Foreach iteration of property list");
    tcase_add_test (tc, test_foreach);
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
