include(../../VigraQt.pri)

TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
TARGET       = VigraQt
QT          += opengl

target.path    = $$INSTALLBASE/lib
headers.path   = $$INSTALLBASE/include/$${TARGET}
INSTALLS       = target headers

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
