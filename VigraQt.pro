# -*-Makefile-*-
TEMPLATE = subdirs
SUBDIRS  = src/VigraQt

include(VigraQt.pri)

# pass on variables (is there a better way?):
QMAKE_QMAKE += INSTALLBASE=$${INSTALLBASE} libdir_name=$${libdir_name}

pkgconfig.path  = $${INSTALLBASE}/$${libdir_name}/pkgconfig
pkgconfig.extra = sed \'s,@prefix@,$${INSTALLBASE},;s,@version@,$${VERSION},;s,@libdir_name@,$${libdir_name},\' VigraQt.pc.in > VigraQt.pc
pkgconfig.files = VigraQt.pc

INSTALLS = pkgconfig
