%Module vigraqt

%Import qtmod.sip

%Include qimageviewer.sip

%Makefile Makefile.am
dynmod_LTLIBRARIES = libvigraqtc.la
pymod_DATA = vigraqt.py

AM_CXXFLAGS = -DSIP_MAKE_MODULE_DLL

INCLUDES = $$(all_includes) -I$$(top_srcdir)/src/vigraqt

libvigraqtc_la_LDFLAGS = -avoid-version $$(all_libraries)
libvigraqtc_la_LIBADD = $$(LIB_QT) ../../vigraqt/libvigraqt.la
libvigraqtc_la_SOURCES = \
	$B
%End
