designer_plugin_LTLIBRARIES = imageviewers.la

INCLUDES = $(all_includes) -I$(top_srcdir)/src/vigraqt

imageviewers_la_LDFLAGS = -module -avoid-version -no-undefined $(all_libraries)
imageviewers_la_LIBADD = $(LIB_QT) ../vigraqt/libvigraqt.la ../vigraqt/libvigrakde.la
imageviewers_la_SOURCES = widgetplugin.cxx
