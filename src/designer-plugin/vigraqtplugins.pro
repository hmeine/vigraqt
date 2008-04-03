TEMPLATE     = lib
TARGET = vigraqtplugins
CONFIG      += designer plugin debug_and_release
QT	+= qt3support


target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target 


unix::INCLUDEPATH += $$system( vigra-config --cppflags | sed "s,-I,,g" ) ../vigraqt
unix::LIBS        += -lvigraqt -L../vigraqt

win32:TEMPLATE    = vclib
win32::INCLUDEPATH += 	"C:/Programme/Vigra 1.5/include" "C:/Dokumente und Einstellungen/Bug/Eigene Dateien/vigraqt"
win32:LIBS +=	"C:/Programme/Vigra 1.5/src/impex/vigraimpex_debug.dll.lib" \
				"C:/Dokumente und Einstellungen/Bug/Eigene Dateien/vigraqt/lib/vigraqt.lib" \
				
SOURCES     += vigraqtplugins.cxx

