SOURCES  += widgetplugin.cxx
#HEADERS  += 
#DESTDIR   = ../../../../../plugins/designer

#target.path=$$plugins.path

#INSTALLS    += target
TEMPLATE     = lib
CONFIG      += qt warn_on release plugin
INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" ) ../vigraqt
#INCLUDEPATH += $$QT_SOURCE_TREE/tools/designer/interfaces
#DBFILE       = plugin.db
#LANGUAGE     = C++
