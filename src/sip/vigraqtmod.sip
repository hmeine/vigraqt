%Module vigraqt

%Import qtmod.sip

%Include qimageviewer.sip
%Include wrapper.sip

%Makefile Makefile.am
dynmod_LTLIBRARIES = libvigraqtc.la
pymod_DATA = vigraqt.py

METASOURCES = AUTO

AM_CXXFLAGS = -DSIP_MAKE_MODULE_DLL

INCLUDES = -I$$(top_srcdir)/src/vigraqt $$(all_includes)

libvigraqtc_la_LDFLAGS = -avoid-version $$(all_libraries) -L/usr/lib/python2.2/site-packages
libvigraqtc_la_LIBADD = $$(LIB_QT) ../../vigraqt/libvigraqt.la -lqtcmodule -lsip
libvigraqtc_la_SOURCES = \
	$B
%End
