TEMPLATE     = lib
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" )

HEADERS += createqimage.hxx \
           imagecaption.hxx \
           qimageviewer.hxx \
           fimageviewer.hxx \
           qrgbvalue.hxx \
           rgbavalue.hxx \
           vigraqimage.hxx
SOURCES += imagecaption.cxx qimageviewer.cxx fimageviewer.cxx
