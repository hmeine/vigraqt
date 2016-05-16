TEMPLATE     = lib
CONFIG      += qt warn_on release plugin

greaterThan(QT_MAJOR_VERSION, 4) {
 	QT += designer
} else {
 	CONFIG += designer
}

TARGET       = vigraqtplugins

target.path  = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

INCLUDEPATH += ../VigraQt $$system( vigra-config --cppflags | sed "s,-I,,g" )
LIBS        += -lVigraQt -L../VigraQt

SOURCES     += vigraqtplugins.cxx
