# -*-Makefile-*-

# FIXME: make target library name compatible for python
# (right now, using configure.py instead of qmake is preferred)

INCLUDEPATH += $$system("python python-config.py includepath") ../vigraqt
LIBS        += $$system("python python-config.py libs")

system(python callsip.py)

SOURCES = $$system('grep ^sources VigraQt.sbf | sed s,^.*=,,')
HEADERS = $$system('grep ^headers VigraQt.sbf | sed s,^.*=,,')

TEMPLATE = lib

QT += opengl
