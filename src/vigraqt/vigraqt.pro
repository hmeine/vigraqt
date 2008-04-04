TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
TARGET       = VigraQt
VERSION      = 0.4

target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include/$${TARGET}
pkgconfig.path = $$target.path/pkgconfig
INSTALLS       = target headers pkgconfig

HEADERS += \
	qimageviewer.hxx \
	qglimageviewer.hxx \
	overlayviewer.hxx \
	fimageviewer.hxx \
	imagecaption.hxx \
	vigraqimage.hxx \
	qrgbvalue.hxx \
	createqimage.hxx \
	colormap.hxx \
	cmeditor.hxx

headers.files = $$HEADERS

SOURCES += \
	qimageviewer.cxx \
	qglimageviewer.cxx \
	overlayviewer.cxx \
	fimageviewer.cxx \
	imagecaption.cxx \
	colormap.cxx \
	cmeditor.cxx

pkgconfig.extra = sed \'s,@prefix@,$$INSTALLBASE,;s,@version@,$$VERSION,\' ../../VigraQt.pc.in > VigraQt.pc
pkgconfig.files = VigraQt.pc

QT += opengl
