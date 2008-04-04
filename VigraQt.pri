# -*-Makefile-*-

# install prefix; you may overwrite this when invoking qmake, e.g.
# > qmake INSTALLBASE=$HOME/local && make && make install
count(INSTALLBASE, 0) {
	INSTALLBASE=/usr/local
}

# version (used for pkgconfig and library versioning)
VERSION = 0.4
