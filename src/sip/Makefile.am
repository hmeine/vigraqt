SUBDIRS = vigraqt

.PHONY: sip

sip: vigraqt/Makefile.am

vigraqt/Makefile.am: vigraqtmod.sip qimageviewer.sip wrapper.sip
	sip -t WS_X11 -t Qt_3_2_0 -I /usr/share/sip/qt -m vigraqt/Makefile.am -c vigraqt vigraqtmod.sip
