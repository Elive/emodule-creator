ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I. \
	 -I$(top_srcdir) \
	 -I$(includedir) \
	 @e_cflags@

pkgdir = $(datadir)/$(MODULE_ARCH)
pkg_LTLIBRARIES = module.la
module_la_SOURCES = \
            e_mod_main.h \
		    e_mod_main.c
#            e_mod_HOHO.c \
#            e_mod_HOHO.h
# HOHO: Add here the list of files to include in the compilation		    

module_la_LIBADD = @e_libs@
module_la_LDFLAGS = -module -avoid-version
module_la_DEPENDENCIES = $(top_builddir)/config.h

clean-local:
	rm -rf *~
