# -*-Makefile-*-

INCLUDEPATH += $$system("python python-config.py includepath")
LIBS        += $$system("python python-config.py libs")

system(python callsip.py)

SOURCES = $$system('grep ^sources VigraQt.sbf | sed s,^.*=,,')
HEADERS = $$system('grep ^headers VigraQt.sbf | sed s,^.*=,,')

TEMPLATE = lib

QT += opengl
