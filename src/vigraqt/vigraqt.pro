TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
LIBS        += $$system( vigra-config --impex-lib )
DEFINES     += NO_MOC_INCLUSION

HEADERS += createqimage.hxx \
           imagecaption.hxx \
           qimageviewer.hxx \
           qrgbvalue.hxx \
           rgbavalue.hxx \
           vigraqimage.hxx
SOURCES += imagecaption.cxx qimageviewer.cxx
