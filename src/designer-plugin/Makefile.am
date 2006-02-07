designer_plugin_LTLIBRARIES = vigraqtwidgets.la

INCLUDES = $(all_includes) -I$(top_srcdir)/src/vigraqt

vigraqtwidgets_la_LDFLAGS = -module -avoid-version -no-undefined $(all_libraries)
vigraqtwidgets_la_LIBADD = $(LIB_QT) ../vigraqt/libvigraqt.la
vigraqtwidgets_la_SOURCES = widgetplugin.cxx
