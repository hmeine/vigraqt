TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )
DEFINES     += NO_MOC_INCLUSION

HEADERS += createqimage.hxx \
           imagecaption.hxx \
           qimageviewer.hxx \
           fimageviewer.hxx \
           qrgbvalue.hxx \
           rgbavalue.hxx \
           vigraqimage.hxx
SOURCES += imagecaption.cxx qimageviewer.cxx fimageviewer.cxx
