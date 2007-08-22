# -*-Makefile-*-

TEMPLATE   = app
CONFIG    += qt warn_on release
FORMS      = imageAnalyzerBase.ui
HEADERS    = imageAnalyzer.hxx
SOURCES    = imageAnalyzer.cxx main.cxx
TARGET     = imageAnalyzer

INCLUDEPATH += \
	$$system( vigra-config --cppflags | sed "s,-I,,g" ) \
	$$system( pkg-config --cflags-only-I vigraqt | sed "s,-I,,g" )

LIBS += \
	$$system( vigra-config --impex-lib ) \
	$$system( pkg-config --libs vigraqt )
