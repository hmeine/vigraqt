#!/bin/sh
hg clone vigraqt vigraqt.release
cd $_

# fetches $VERSION from AM_INIT_AUTOMAKE line in configure.in:
source autogen.sh

rm -rv README.CVS autom4te.cache .hg* && \
(
  cd .. && cp -va vigraqt.release vigraqt.testing && (
    cd $_ && mkdir build && cd $_ && \
    ../configure --prefix=/tmp/testing-vigraqt && make && make install && \
    echo 'SUCCESSFULLY FINISHED INSTALL!'
  ) && \
  rm -r vigraqt.testing
)

#cvs ci -m "checking in working set of auto-generated files for release $VERSION"

cd .. && mv vigraqt.release vigraqt-$VERSION && \
tar cvzf vigraqt-$VERSION.tar.gz vigraqt-$VERSION

echo "############ TODO: ############"
echo hg tag `echo release_$VERSION | tr . _`
echo mv vigraqt-$VERSION.tar.gz ~/public_html/software/vigraqt
echo rm -r vigraqt-$VERSION
