SUBDIRS = src

$(top_srcdir)/configure.in: $(top_srcdir)/subdirs $(top_srcdir)/configure.files $(CONF_FILES)
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in

ACINCLUDE_PARTS = $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
$(top_srcdir)/acinclude.m4: $(ACINCLUDE_PARTS)
	@cd $(top_srcdir) && cat $(ACINCLUDE_PARTS) > acinclude.m4

AUTOMAKE_OPTIONS = foreign 1.5
