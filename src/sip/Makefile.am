AM_CXXFLAGS = -DSIP_MAKE_MODULE_DLL

INCLUDES = -I$(top_srcdir)/src/vigraqt $(all_includes) $(SIP_INCLUDES)

dynmod_LTLIBRARIES = vigraqt.la

vigraqt_la_LDFLAGS = -module -avoid-version $(all_libraries) -L/usr/lib/python2.3/site-packages
vigraqt_la_LIBADD = $(LIB_QT) ../vigraqt/libvigraqt.la
vigraqt_la_SOURCES = \
	sipvigraqtcmodule.cpp sipvigraqtQGLImageWidget.cpp sipvigraqtQGLImageViewer.cpp sipvigraqtQImageViewer.cpp sipvigraqtQImageViewerBase.cpp moc_sipvigraqtcmodule.cpp

SIP=sip
SIP_PYQT_PARAMS=`python -c 'import pyqtconfig; c = pyqtconfig.Configuration(); print c.pyqt_qt_sip_flags, "-I", c.pyqt_sip_dir'`
SIP_INCLUDES=`python -c 'import pyqtconfig; c = pyqtconfig.Configuration(); print "-I" + c.sip_inc_dir + " -I" + c.py_inc_dir'`

vigraqt_buildinfo: $(srcdir)/vigraqtmod.sip $(srcdir)/qimageviewer.sip $(srcdir)/qglimageviewer.sip $(srcdir)/wrapper.sip
	$(SIP) $(SIP_PYQT_PARAMS) -b $@ -c . $< && \
	echo "SIP_INCLUDES = $(SIP_INCLUDES)" >> $@

sipvigraqtcmodule.cpp: vigraqt_buildinfo
sipvigraqtcmodule.h: vigraqt_buildinfo
sipvigraqtQGLImageWidget.cpp: vigraqt_buildinfo
sipvigraqtQGLImageViewer.cpp: vigraqt_buildinfo
sipvigraqtQImageViewer.cpp: vigraqt_buildinfo
sipvigraqtQImageViewerBase.cpp: vigraqt_buildinfo

# "sip" target for your convenience
.PHONY: sip
sip: vigraqt_buildinfo

# $(patsubst %.h, moc_%.cpp, $(moc_headers))

moc_%.cpp: %.h
	$(MOC) -o $@ $<

clean-local:
	rm -f sipqtQ*.h sipqtglQ*.h sipvigraqt* sipAPIvigraqt.h moc_sip*.cpp vigraqt_buildinfo
