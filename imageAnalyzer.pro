TEMPLATE      = app
CONFIG       += qt warn_on release
FORMS         = imageAnalyzerBase.ui
HEADERS       = imageAnalyzer.hxx colormap.hxx cmEditor.h
SOURCES       = imageAnalyzer.cxx colormap.cxx cmEditor.cpp main.cxx
TARGET        = imageAnalyzer
INCLUDEPATH  += $$system( vigra-config --cppflags | sed "s,-I,,g" )
LIBS         += $$system( vigra-config --impex-lib ) -L/home/hans/uni/KOGS/local-gentoo/lib -lvigraqt
