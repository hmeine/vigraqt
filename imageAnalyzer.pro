TEMPLATE      = app
CONFIG       += qt warn_on debug
FORMS         = imageAnalyzerBase.ui
HEADERS       = imageAnalyzer.hxx
SOURCES       = imageAnalyzer.cxx main.cxx
TARGET        = imageAnalyzer
INCLUDEPATH  += $$system( vigra-config --cppflags | sed "s,-I,,g" )
LIBS         += $$system( vigra-config --impex-lib ) -L/home/hans/uni/KOGS/local-gentoo/lib -lvigraqt
