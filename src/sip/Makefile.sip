# -*-Makefile-*-

.PHONY: sip

#SIPQT_PARAMS="-t Qt_3_2_0 -I /usr/share/sip/qt"
#SIPQT_PARAMS=-t Qt_3_3_0 -I /software/python-qt-3.13/PyQt-x11-gpl-3.13/sip
SIPQT_PARAMS=`python -c 'import pyqtconfig; c = pyqtconfig.Configuration(); print c.pyqt_qt_sip_flags, "-I", c.pyqt_sip_dir'`

sip: vigraqt_buildinfo

vigraqt_buildinfo: vigraqtmod.sip qimageviewer.sip qglimageviewer.sip wrapper.sip
	sip -t WS_X11 $(SIPQT_PARAMS) -b $@ -c . $< && \
	python -c 'import pyqtconfig; c = pyqtconfig.Configuration(); print "SIP_INCLUDES = -I" + c.sip_inc_dir' >> $@

clean:
	rm -f sipqtQ*.h sipqtglQ*.h sipvigraqt* sipAPIvigraqt.h moc_sip*.cpp vigraqt_buildinfo
