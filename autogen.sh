VERSION=`grep AM_INIT_AUTOMAKE configure.in | sed 's,[^0-9]*\([0-9.a-z]\+\).*,\1,'`
echo "Preparing VigraQt v$VERSION..."

test -f src/sip/vigraqt_buildinfo && rm $_ # remove leftover from old versions
cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4
aclocal
autoheader
touch config.h.in
automake --foreign --include-deps
autoconf
