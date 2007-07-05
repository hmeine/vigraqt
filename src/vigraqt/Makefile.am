lib_LTLIBRARIES = libvigraqt.la

include_HEADERS = \
	qimageviewer.hxx \
	qglimageviewer.hxx \
	overlayviewer.hxx \
	fimageviewer.hxx \
	imagecaption.hxx \
	vigraqimage.hxx \
	qrgbvalue.hxx \
	createqimage.hxx \
	colormap.hxx \
	cmeditor.hxx

INCLUDES = $(all_includes)

libvigraqt_la_LDFLAGS = -avoid-version -no-undefined $(all_libraries)
libvigraqt_la_LIBADD = $(LIB_QT) -lGL -lGLU
libvigraqt_la_SOURCES = \
	qimageviewer.cxx \
	qglimageviewer.cxx \
	overlayviewer.cxx \
	fimageviewer.cxx \
	imagecaption.cxx \
	colormap.cxx \
	cmeditor.cxx

check_PROGRAMS = test

test_SOURCES = test.cxx
test_LDADD = libvigraqt.la

%.moc: $(srcdir)/%.hxx
	$(MOC) $< -o $@

overlayviewer.lo: overlayviewer.moc
qimageviewer.lo: qimageviewer.moc
fimageviewer.lo: fimageviewer.moc
imagecaption.lo: imagecaption.moc
cmeditor.lo: cmeditor.moc
