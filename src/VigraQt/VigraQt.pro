include(../../VigraQt.pri)

!win* { # hack: disable OpenGL on Windows
	CONFIG += opengl
	QT     += opengl
}

TEMPLATE           = lib
CONFIG            += warn_on release qtgui
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
	vigraqgraphicsimageitem.hxx \
	vigraqgraphicsscene.hxx \
	vigraqgraphicsview.hxx \

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
	vigraqgraphicsimageitem.cxx \
	vigraqgraphicsscene.cxx \
	vigraqgraphicsview.cxx \

CONFIG(opengl) {
	HEADERS += qglimageviewer.hxx 
	SOURCES += qglimageviewer.cxx
} else {
	message(OpenGL support disabled; QGLImageViewer will not be included.)
}

CONFIG += create_pc create_prl no_install_prl

QMAKE_PKGCONFIG_NAME = VigraQt
QMAKE_PKGCONFIG_DESCRIPTION = Qt4 bindings for the VIGRA library
QMAKE_PKGCONFIG_PREFIX = $$INSTALLBASE
QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_VERSION = $$VERSION
