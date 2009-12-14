TEMPLATE   = app
CONFIG    += qt warn_on release

SOURCES    = minimal.cxx

!win32 {
	CONFIG    += link_pkgconfig
	PKGCONFIG += VigraQt
} else {
	INCLUDEPATH += ../../src
	LIBS        += -L../../src/VigraQt/release -lVigraQt0
}
