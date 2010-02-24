testsdir   = $(datadir)/libngf-tests
tests_DATA = tests.xml

INCLUDES = -I$(top_srcdir)
bin_PROGRAMS = test-proplist
test_proplist_SOURCES = test-proplist.c
test_proplist_CFLAGS = @CHECK_CFLAGS@ @BASE_CFLAGS@ @GLIB_CFLAGS@
test_proplist_LDADD = @CHECK_LIBS@ @BASE_LIBS@ @GLIB_LIBS@ -L../libngf/.libs/ -lngf0
