lib_LTLIBRARIES = libvigraqt.la

include_HEADERS = \
	qimageviewer.hxx \
	fimageviewer.hxx \
	vigraqimage.hxx \
	qrgbvalue.hxx \
	createqimage.hxx \
	imagecaption.hxx

INCLUDES = $(all_includes)

libvigraqt_la_LDFLAGS = -avoid-version -no-undefined $(all_libraries)
libvigraqt_la_LIBADD = $(LIB_QT)
libvigraqt_la_SOURCES = qimageviewer.cxx imagecaption.cxx fimageviewer.cxx

%.moc: $(srcdir)/%.hxx
	$(MOC) $< -o $@
