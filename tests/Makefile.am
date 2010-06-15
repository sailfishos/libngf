TESTS = \
	test-proplist \
	test-client

check_PROGRAMS = \
	test-proplist \
	test-client

INCLUDES = -I$(top_srcdir)

test_proplist_SOURCES = test-proplist.c ../libngf/proplist.c
test_proplist_CFLAGS = @CHECK_CFLAGS@ @BASE_CFLAGS@ @GLIB_CFLAGS@
test_proplist_LDADD = @CHECK_LIBS@ @BASE_LIBS@ @GLIB_LIBS@

test_client_SOURCES = test-client.c ../libngf/client.c ../libngf/proplist.c
test_client_CFLAGS = @CHECK_CFLAGS@ @BASE_CFLAGS@ @GLIB_CFLAGS@
test_client_LDADD = @CHECK_LIBS@ @BASE_LIBS@ @GLIB_LIBS@
