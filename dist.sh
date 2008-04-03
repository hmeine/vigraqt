cvs -d ~/cvsroot co vigraqt
cd vigraqt

source autogen.sh

function distclean {
   find . -name CVS -o -name .cvsignore | xargs rm -rv README.CVS autom4te.cache
}

(
  cd .. && cp -va vigraqt vigraqt.testing && (
    cd $_ && distclean && mkdir build && cd $_ && \
    ../configure --prefix=/tmp/testing-vigraqt && make && make install && \
    echo 'SUCCESSFULLY FINISHED INSTALL!'
  ) && \
  rm -r vigraqt.testing
)

#cvs ci -m "checking in working set of auto-generated files for release $VERSION"
cvs tag -F `echo release_$VERSION | tr . _`

distclean && \
cd .. && mv vigraqt vigraqt-$VERSION && \
tar cvzf vigraqt-$VERSION.tar.gz vigraqt-$VERSION
