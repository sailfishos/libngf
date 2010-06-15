#include <stdlib.h>
#include <check.h>
#include <glib.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#include <libngf/client.h>

START_TEST (test_create_client)
{
	NgfClient *client = NULL;
	DBusConnection *connection = NULL;
	connection = dbus_bus_get (DBUS_BUS_SYSTEM, NULL);
	dbus_connection_setup_with_g_main (connection, NULL);

	client = ngf_client_create (NGF_TRANSPORT_DBUS, connection);
	fail_unless (client != NULL);

}
END_TEST

START_TEST (test_failed_create_client)
{
	NgfClient *client = NULL;
	DBusConnection *connection = NULL;

	client = ngf_client_create (NGF_TRANSPORT_DBUS, connection);
	fail_unless (client == NULL);

}
END_TEST

START_TEST (test_play_event_NULL)
{
	NgfClient *client = NULL;
	DBusConnection *connection = NULL;
	connection = dbus_bus_get (DBUS_BUS_SYSTEM, NULL);
	dbus_connection_setup_with_g_main (connection, NULL);

	client = ngf_client_create (NGF_TRANSPORT_DBUS, connection);
	fail_unless (client != NULL);

	NgfProplist *p = NULL;
	char *event_id = NULL;
	p  = ngf_proplist_new ();
	uint32_t id = ngf_client_play_event (client, event_id, p);
	fail_unless (id == 0);

}
END_TEST

START_TEST (test_play_client_NULL)
{
	NgfClient *client = NULL;
	NgfProplist *p = NULL;
	char *event_id = "sms";
	p  = ngf_proplist_new ();
	uint32_t id = ngf_client_play_event (client, event_id, p);
	fail_unless (id == 0);

}
END_TEST

START_TEST (test_play)
{
	NgfClient *client = NULL;
	DBusConnection *connection = NULL;
	connection = dbus_bus_get (DBUS_BUS_SYSTEM, NULL);
	dbus_connection_setup_with_g_main (connection, NULL);

	client = ngf_client_create (NGF_TRANSPORT_DBUS, connection);
	fail_unless (client != NULL);

	NgfProplist *p = NULL;
	char *event_id = "sms";
	p  = ngf_proplist_new ();
	uint32_t id = ngf_client_play_event (client, event_id, p);
	fail_unless (id != 0);
}
END_TEST

START_TEST (test_callback)
{
	NgfClient *client = NULL;
	NgfProplist *p = NULL;
	char *event_id = "sms";
	p  = ngf_proplist_new ();
	uint32_t id = ngf_client_play_event (client, event_id, p);
	fail_unless (id == 0);

//	ngf_client_set_callback(NULL, NULL, NULL);
	ngf_client_destroy(NULL);
//	fail_unless (client->callback == NULL);
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

	g_type_init ();

	s = suite_create ("Client interface");

	tc = tcase_create ("Create client");
	tcase_add_test (tc, test_create_client);
	suite_add_tcase (s, tc);

	tc = tcase_create ("Failed creation of client");
	tcase_add_test (tc, test_failed_create_client);
	suite_add_tcase (s, tc);

	tc = tcase_create ("Play with NULL event");
	tcase_add_test (tc, test_play_event_NULL);
	suite_add_tcase (s, tc);

	tc = tcase_create ("Play with NULL client");
	tcase_add_test (tc, test_play_client_NULL);
	suite_add_tcase (s, tc);

	tc = tcase_create ("Play sms");
	tcase_add_test (tc, test_play);
	suite_add_tcase (s, tc);

	tc = tcase_create ("Callback");
	tcase_add_test (tc, test_callback);
	suite_add_tcase (s, tc);

	sr = srunner_create (s);
	srunner_run_all (sr, CK_NORMAL);
	num_failed = srunner_ntests_failed (sr);
	srunner_free (sr);

	return num_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
