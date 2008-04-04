# -*-Makefile-*-
TEMPLATE = subdirs
SUBDIRS  = src/vigraqt

# pass on this variable's value (is there a better way?):
QMAKE_QMAKE += INSTALLBASE=$$INSTALLBASE

pkgconfig.path  = $$INSTALLBASE/lib/pkgconfig
pkgconfig.extra = sed \'s,@prefix@,$$INSTALLBASE,;s,@version@,$$VERSION,\' VigraQt.pc.in > VigraQt.pc
pkgconfig.files = VigraQt.pc

INSTALLS = pkgconfig
