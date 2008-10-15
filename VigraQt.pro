# -*-Makefile-*-
TEMPLATE = subdirs
SUBDIRS  = src/vigraqt

libdir = lib # FIXME: libdir can be lib64!

# pass on variables (is there a better way?):
QMAKE_QMAKE += INSTALLBASE=$${INSTALLBASE} libdir=$${libdir}

pkgconfig.path  = $${INSTALLBASE}/$${libdir}/pkgconfig
pkgconfig.extra = sed \'s,@prefix@,$${INSTALLBASE},;s,@version@,$${VERSION},;s,@libdir@,$${libdir},\' VigraQt.pc.in > VigraQt.pc
pkgconfig.files = VigraQt.pc

INSTALLS = pkgconfig
