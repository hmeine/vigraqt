# -*-Makefile-*-

# install prefix; you may overwrite this when invoking qmake, e.g.
# > qmake INSTALLBASE=$HOME/local && make && make install
count(INSTALLBASE, 0) {
	INSTALLBASE=/usr/local
}

count(libdir_name, 0) {
	libdir_name = lib # FIXME: libdir_name can be lib64, autodetect that!
}

# version (used for pkgconfig and library versioning)
VERSION = 0.5
