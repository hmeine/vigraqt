lib_LTLIBRARIES = libvigraqt.la
include_HEADERS = qimageviewer.hxx fimageviewer.hxx vigraqimage.hxx

METASOURCES = AUTO

INCLUDES = $(all_includes)

libvigraqt_la_LDFLAGS = -avoid-version -no-undefined $(all_libraries)
libvigraqt_la_LIBADD = $(LIB_QT) -lkdecore -lkdeui # for i18n() and KDoubleNumInput
libvigraqt_la_SOURCES = qimageviewer.cxx fimageviewer.cxx
