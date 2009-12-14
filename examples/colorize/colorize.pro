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
	VIGRA_ROOT = c:\vigra
	INCLUDEPATH += $${VIGRA_ROOT}\include
#	LIBS        += -L$${VIGRA_ROOT}\lib -lvigraimpex
	LIBS        += $${VIGRA_ROOT}\lib\libvigraimpex.dll.a # CMake
#	LIBS        += $${VIGRA_ROOT}\lib\vigraimpex.dll.lib  # MSVC

	INCLUDEPATH += ../../src
	LIBS        += -L../../src/VigraQt/release -lVigraQt0
}
