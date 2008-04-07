# -*-Makefile-*-

TEMPLATE   = app
CONFIG    += qt warn_on release link_pkgconfig
FORMS      = colorizeBase.ui
HEADERS    = colorize.hxx
SOURCES    = colorize.cxx main.cxx

PKGCONFIG += VigraQt

INCLUDEPATH += \
	$$system( vigra-config --cppflags | sed "s,-I,,g" ) \

LIBS += \
	$$system( vigra-config --impex-lib ) \
