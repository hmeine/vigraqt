VERSION=`grep AM_INIT_AUTOMAKE configure.in | sed 's,[^0-9]*\([0-9.]\+\).*,\1,'`
echo "Preparing VigraQt v$VERSION..."

touch src/sip/vigraqt_buildinfo
cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4
aclocal
autoheader
touch config.h.in
automake --foreign --include-deps
autoconf
