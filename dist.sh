#!/bin/sh
VERSION=`grep ^VERSION vigraqt.qt4/VigraQt.pri | sed 's,.*= *,,'`
test -z "$VERSION" && {
	echo "ERROR: VERSION extraction failed."
    exit 1
}
echo "*** packaging vigraqt4-$VERSION ***"

hg clone vigraqt.qt4 vigraqt4.release || exit $?
cd $_

rm -rv .hg* dist.sh && \
(
  cd .. && cp -va vigraqt4.release vigraqt4.testing && (
    cd $_ && \
    qmake INSTALLBASE=/tmp/testing-vigraqt4 && make && make install && \
    echo 'SUCCESSFULLY FINISHED INSTALL!'
  ) && \
  rm -r vigraqt4.testing
)

cd .. && mv vigraqt4.release vigraqt4-$VERSION && \
tar cvzf vigraqt4-$VERSION.tar.gz vigraqt4-$VERSION && \
rm -r vigraqt4-$VERSION

echo "############ TODO: ############"
echo hg -R vigraqt.qt4 tag `echo release_$VERSION | tr . _`
echo mv vigraqt4-$VERSION.tar.gz public_html/software/vigraqt
