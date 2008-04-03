TEMPLATE     = lib
CONFIG      += qt warn_on release plugin
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" ) ../vigraqt
SOURCES     += widgetplugin.cxx
LIBS        += -lvigraqt -L../vigraqt
#The following line was inserted by qt3to4
QT +=  qt3support 
