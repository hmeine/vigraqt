TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
TARGET       = vigraqt4
VERSION      = 0.4

target.path   = $$INSTALLBASE/lib
headers.path  = $$INSTALLBASE/include/VigraQt4
INSTALLS      = target headers

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

QT += opengl qt3support
