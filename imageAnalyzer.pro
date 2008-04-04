# -*-Makefile-*-

TEMPLATE   = app
CONFIG    += qt warn_on release link_pkgconfig
FORMS      = imageAnalyzerBase.ui
HEADERS    = imageAnalyzer.hxx
SOURCES    = imageAnalyzer.cxx main.cxx
TARGET     = imageAnalyzer

PKGCONFIG += VigraQt

INCLUDEPATH += \
	$$system( vigra-config --cppflags | sed "s,-I,,g" ) \

LIBS += \
	$$system( vigra-config --impex-lib ) \

QT +=  qt3support
