TEMPLATE     = lib
CONFIG      += qt warn_on release plugin designer
TARGET       = vigraqtplugins
QT          += qt3support

target.path  = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

INCLUDEPATH += ../vigraqt $$system( vigra-config --cppflags | sed "s,-I,,g" )
LIBS        += -lvigraqt -L../vigraqt

SOURCES     += vigraqtplugins.cxx
