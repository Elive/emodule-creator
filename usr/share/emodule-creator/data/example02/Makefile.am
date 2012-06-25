ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing config.rpath mkinstalldirs 

SUBDIRS = src

if HAVE_PO

SUBDIRS += po

endif

EDJE_FLAGS = -v \
-id $(top_srcdir)/images \
-fd $(top_srcdir)/fonts

filesdir = $(datadir)
files_DATA = module.desktop \
	     e-module-HOHO.edj \
	     HOHO.edj
	     
EXTRA_DIST = $(files_DATA) \
		 e_modules-HOHO.spec \
	     HOHO.edc \
	     e-module-HOHO.edc \
	     images/module_icon.png \
	     images/HOHO.png

%.edj: %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@
	
clean-local:
	rm -rf *.edj *~
	
uninstall-hook:
	rm -rf $(datadir);
	
