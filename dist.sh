#!/bin/sh
VERSION=`grep ^VERSION VigraQt.pri | sed 's,.*= *,,'`
echo "*** packaging vigraqt4-$VERSION ***"

hg clone vigraqt.qt4 vigraqt4.release
cd $_

rm -rv .hg* && \
(
  cd .. && cp -va vigraqt4.release vigraqt4.testing && (
    cd $_ && \
    qmake INSTALLBASE=/tmp/testing-vigraqt4 && make && make install && \
    echo 'SUCCESSFULLY FINISHED INSTALL!'
  ) && \
  rm -r vigraqt4.testing
)

cd .. && mv vigraqt4.release vigraqt4-$VERSION && \
tar cvzf vigraqt4-$VERSION.tar.gz vigraqt4-$VERSION

echo "############ TODO: ############"
echo hg tag `echo release_$VERSION | tr . _`
echo mv vigraqt4-$VERSION.tar.gz public_html/software/vigraqt
echo rm -r vigraqt4-$VERSION
