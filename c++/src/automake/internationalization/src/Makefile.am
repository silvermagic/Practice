NULL =

bin_PROGRAMS = app
app_LDADD =

app_CPPFLAGS =           \
	$(NULL)

app_CXXFLAGS = -ggdb3 -Wall -MMD  -fpermissive -g
MAINTAINERCLEANFILES =

COMMON_LIBS =             \
    $(GLIB2_LIBS)         \
	$(NULL)

if !OS_WIN32
COMMON_CPPFLAGS =                           \
    -DLOCALE_DIR=\""$(datadir)/locale"\"    \
	$(GLIB2_CFLAGS)                         \
	$(NULL)
else
COMMON_CPPFLAGS = \
    -DLOCALE_DIR=\""./../share/locale"\"    \
	$(GLIB2_CFLAGS)                         \
	$(NULL)
endif

app_SOURCES =             \
    main.cpp              \
    $(NULL)

app_LDFLAGS =             \
	$(COMMON_LIBS)        \
	$(NULL)

app_CPPFLAGS +=           \
	$(COMMON_CPPFLAGS)    \
	$(NULL)

