include(../../VigraQt.pri)

!win* { # hack: disable OpenGL on Windows
	CONFIG += opengl
	QT     += opengl
}

TEMPLATE           = lib
CONFIG            += warn_on release
TARGET             = VigraQt
!win*:INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
DEFINES           += MAKE_VIGRAQT_LIB

target.path    = $$INSTALLBASE/$${libdir_name}
headers.path   = $$INSTALLBASE/include/$${TARGET}
INSTALLS       = target headers

HEADERS += \
	vigraqt_export.hxx \
	qimageviewer.hxx \
	overlayviewer.hxx \
	fimageviewer.hxx \
	imagecaption.hxx \
	vigraqimage.hxx \
	qrgbvalue.hxx \
	createqimage.hxx \
	colormap.hxx \
	linear_colormap.hxx \
	cmgradient.hxx \
	cmeditor.hxx \

headers.files = $$HEADERS

SOURCES += \
	qimageviewer.cxx \
	overlayviewer.cxx \
	fimageviewer.cxx \
	imagecaption.cxx \
	colormap.cxx \
	linear_colormap.cxx \
	cmgradient.cxx \
	cmeditor.cxx \

CONFIG(opengl) {
	HEADERS += qglimageviewer.hxx
	SOURCES += qglimageviewer.cxx
} else {
	message(OpenGL support disabled; QGLImageViewer will not be included.)
}
