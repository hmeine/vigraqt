SUBDIRS = vigraqt

vigraqt/Makefile.am: vigraqtmod.sip
	sip -t WS_X11 -t Qt_3_1_0 -I ~/PyQt-x11-gpl-3.5/sip -m vigraqt/Makefile.am -c vigraqt vigraqtmod.sip
