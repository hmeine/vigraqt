lib_LTLIBRARIES = libvigraqt.la libvigrakde.la
include_HEADERS = \
	qimageviewer.hxx \
	fimageviewer.hxx \
	vigraqimage.hxx \
	qrgbvalue.hxx \
	createqimage.hxx

INCLUDES = $(all_includes)

METASOURCES = qimageviewer.moc fimageviewer.moc

libvigraqt_la_LDFLAGS = -avoid-version -no-undefined $(all_libraries)
libvigraqt_la_LIBADD = $(LIB_QT)
libvigraqt_la_SOURCES = qimageviewer.cxx

libvigrakde_la_LDFLAGS = -avoid-version -no-undefined $(all_libraries)
libvigrakde_la_LIBADD = $(LIB_QT) libvigraqt.la \
	-lkdecore -lkdeui # for i18n() and KDoubleNumInput
libvigrakde_la_SOURCES = fimageviewer.cxx
