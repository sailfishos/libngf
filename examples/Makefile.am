INCLUDES		= -I$(top_srcdir)

bin_PROGRAMS		= ngf-client

ngf_client_SOURCES	= ngf-client.c
ngf_client_CPPFLAGS	= $(BASE_CFLAGS) $(GLIB_CFLAGS)
ngf_client_LDADD	= $(BASE_LIBS) $(GLIB_LIBS) -L../libngf/.libs/ -lngf0

