library_includedir=$(includedir)/$(NGF_LIBRARY_NAME)-$(NGF_API_VERSION)/$(NGF_LIBRARY_NAME)
library_include_HEADERS = ngf.h proplist.h client.h

INCLUDES		= -I$(top_srcdir)
lib_LTLIBRARIES		= libngf0.la

libngf0_la_SOURCES	= ngf.h \
			  client.h client.c \
			  proplist.h proplist.c
libngf0_la_CPPFLAGS	= $(BASE_CFLAGS)
libngf0_la_LIBADD	= $(BASE_LIBS)
libngf0_la_LDFLAGS	= -version-info $(NGF_LIBRARY_VERSION) -release $(NGF_RELEASE)
