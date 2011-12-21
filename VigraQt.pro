# -*-Makefile-*-
TEMPLATE = subdirs
SUBDIRS  = src/VigraQt

include(VigraQt.pri)

pkgconfig.path  = $${INSTALLBASE}/$${libdir_name}/pkgconfig
pkgconfig.extra = sed \'s,@prefix@,$${INSTALLBASE},;s,@version@,$${VERSION},;s,@libdir_name@,$${libdir_name},\' VigraQt.pc.in > VigraQt.pc
pkgconfig.files = VigraQt.pc

INSTALLS = pkgconfig
