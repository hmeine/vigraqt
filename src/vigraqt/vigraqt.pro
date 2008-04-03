TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
DEFINES     += NO_MOC_INCLUSION

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

SOURCES += \
	qimageviewer.cxx \
	qglimageviewer.cxx \
	overlayviewer.cxx \
	fimageviewer.cxx \
	imagecaption.cxx \
	colormap.cxx \
	cmeditor.cxx

QT += opengl qt3support
