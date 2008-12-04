include(../../VigraQt.pri)

!win32 { # hack: disable OpenGL on Windows
	CONFIG += opengl
	QT     += opengl
}

TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
TARGET       = VigraQt
DEFINES     += MAKE_VIGRAQT_LIB

target.path    = $$INSTALLBASE/$${libdir_name}
headers.path   = $$INSTALLBASE/include/$${TARGET}
INSTALLS       = target headers

HEADERS += \
	qimageviewer.hxx \
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
	overlayviewer.cxx \
	fimageviewer.cxx \
	imagecaption.cxx \
	colormap.cxx \
	cmeditor.cxx

CONFIG(opengl) {
	HEADERS += qglimageviewer.hxx
	SOURCES += qglimageviewer.cxx
} else {
	message(OpenGL support disabled; QGLImageViewer will not be included.)
}