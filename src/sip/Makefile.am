.PHONY: sip

sip: vigraqt_buildinfo

vigraqt_buildinfo: vigraqtmod.sip qimageviewer.sip wrapper.sip
	sip -t WS_X11 -t Qt_3_2_0 -I /usr/share/sip/qt -b vigraqt_buildinfo -c . vigraqtmod.sip
#	sip -t WS_X11 -t Qt_3_2_0 -I /usr/share/sip/qt -m vigraqt/Makefile.am -c vigraqt vigraqtmod.sip

dynmod_LTLIBRARIES = libvigraqtc.la
pymod_DATA = $(srcdir)/vigraqt.py

include $(srcdir)/vigraqt_buildinfo

AM_CXXFLAGS = -DSIP_MAKE_MODULE_DLL

INCLUDES = -I$(top_srcdir)/src/vigraqt $(all_includes)

libvigraqtc_la_LDFLAGS = -avoid-version $(all_libraries) -L/usr/lib/python2.3/site-packages
libvigraqtc_la_LIBADD = $(LIB_QT) ../vigraqt/libvigraqt.la
libvigraqtc_la_SOURCES = \
	$(sources) \
	$(patsubst %.h, %.moc, $(moc_headers))

%.moc: $(srcdir)/%.h
	$(MOC) -o $@ $<
