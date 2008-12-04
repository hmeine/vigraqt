# -*-Makefile-*-

TEMPLATE   = app
CONFIG    += qt warn_on release
FORMS      = colorizeBase.ui
HEADERS    = colorize.hxx
SOURCES    = colorize.cxx main.cxx

!win32 {
	INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
	LIBS        += $$system( vigra-config --impex-lib )

	CONFIG    += link_pkgconfig
	PKGCONFIG += VigraQt
} else {
# FIXME: on Windows, we cannot use scripts or pkg-config in order
# to find library paths - what would be a good solution?
	INCLUDEPATH += c:\vigra\include
	LIBS        += -Lc:\vigra\lib -lvigraimpex

	INCLUDEPATH += ../../src
	LIBS        += -L../../src/vigraqt/release -lVigraQt0
}
