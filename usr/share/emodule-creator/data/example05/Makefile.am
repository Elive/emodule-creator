ACLOCAL_AMFLAGS = -I m4
MAINTAINERCLEANFILES = Makefile.in aclocal.m4 config.guess config.h.in \
		       config.sub configure depcomp install-sh ltmain.sh \
		       missing module.desktop config.rpath mkinstalldirs

SUBDIRS = src

if HAVE_PO

SUBDIRS += po

endif

EDJE_FLAGS = -v \
	   -id $(top_srcdir)/images \
	   -fd $(top_srcdir)/fonts

# data files for the module
filesdir =  $(datadir)

files_DATA = module.desktop \
	     e-module-HOHO.edj

EXTRA_DIST = images/module_icon.png \
         module.desktop.in \
	     e_modules-skel.spec.in \
	     e-module-HOHO.edc \
	     fonts/VeraBd.ttf

e-module-HOHO.edj: Makefile e-module-HOHO.edc images/module_icon.png

%.edj:  %.edc
	$(EDJE_CC) $(EDJE_FLAGS) $< $@

clean-local:
	 rm -rf *.edj module.desktop e_modules-skel.spec *~

uninstall:
	 rm -rf $(DESTDIR)$(datadir)

