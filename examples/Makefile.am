INCLUDES		= -I$(top_srcdir)

bin_PROGRAMS		= test-client

test_client_SOURCES	= test-client.c
test_client_CPPFLAGS	= $(BASE_CFLAGS)
test_client_LDADD	= $(BASE_LIBS) -L../libngf/.libs/ -lngf-1.0

