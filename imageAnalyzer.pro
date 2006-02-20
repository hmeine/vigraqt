# -*-Makefile-*-

TEMPLATE   = app
CONFIG    += qt warn_on
FORMS      = imageAnalyzerBase.ui
HEADERS    = imageAnalyzer.hxx
SOURCES    = imageAnalyzer.cxx main.cxx
TARGET     = imageAnalyzer

INCLUDEPATH += . \
	$$system( vigra-config --cppflags | sed "s,-I,,g" ) \
	$$system( pkg-config --cflags vigraqt | sed "s,-I,,g" )

LIBS += \
	`vigra-config --impex-lib` \
	`pkg-config --libs vigraqt`
