dynmod_LTLIBRARIES = vigraqt.la

AM_CXXFLAGS = -DSIP_MAKE_MODULE_DLL

include $(srcdir)/vigraqt_buildinfo

INCLUDES = -I$(top_srcdir)/src/vigraqt $(all_includes) $(SIP_INCLUDES)

vigraqt_la_LDFLAGS = -module -avoid-version $(all_libraries) -L/usr/lib/python2.3/site-packages
vigraqt_la_LIBADD = $(LIB_QT) ../vigraqt/libvigraqt.la
vigraqt_la_SOURCES = \
	$(sources) \
#	moc_sipvigraqtcmodule.cpp

# $(patsubst %.h, moc_%.cpp, $(moc_headers))

moc_%.cpp: $(srcdir)/%.h
	$(MOC) -o $@ $<
