# Toplevel makefile for OS/161 kernel source.
# Note: actual kernels are not compiled here; they are compiled in 
# compile/FOO where FOO is a kernel configuration name.
#

include ../defs.mk

#
# We don't actually do anything from here except install includes.
# Kernels get built in one or more subdirectories of compile/.
#
# (Actually, some stuff is done on clean below.)
#
all tags depend clean install: ;

#
# Directories in the system runtime tree that include files get
# installed into.
#
INCDIR=$(OSTREE)/include
KINCDIR=$(OSTREE)/include/kern
MDINCDIR=$(OSTREE)/include/$(PLATFORM)

#
# Machine-dependent headers that we export to userland. Other header
# files in arch/$(PLATFORM)/include are not exported.
#
MDHEADERS=asmdefs.h setjmp.h types.h

#
# Install kernel headers into system.
#
# We install everything in include/kern in $(OSTREE)/include/kern.
# We install only $(MDHEADERS) in $(OSTREE)/include/$(PLATFORM).
# We create $(OSTREE)/include/machine to point to 
# $(OSTREE)/include/$(PLATFORM).
#
includes:
	[ -d $(KINCDIR) ] || mkdir -p $(KINCDIR)
	[ -d $(MDINCDIR) ] || mkdir -p $(MDINCDIR)
	rm -f $(INCDIR)/machine
	ln -s $(PLATFORM) $(INCDIR)/machine

	@echo "Installing kernel includes..."
	@(cd include/kern && for h in *.h; do \
		if diff $$h $(KINCDIR)/$$h >/dev/null 2>&1; then \
			true; \
		else \
			echo "Installing $$h"; \
			cp $$h $(KINCDIR)/$$h; \
		fi; \
	 done)
	@echo "Installing machine-dependent includes for $(PLATFORM)..."
	@(cd arch/$(PLATFORM)/include && for h in $(MDHEADERS); do \
		if diff $$h $(MDINCDIR)/$$h >/dev/null 2>&1; then \
			true; \
		else \
			echo "Installing $$h"; \
			cp $$h $(MDINCDIR)/$$h; \
		fi; \
	 done)

#
# Delete all emacs backups in the kernel source when "clean" is invoked.
#
clean: cleanhere
cleanhere:
	find . -name '*~' -print | xargs rm -f

#
# This tells make that the listed rules are not files so it
# (hopefully) won't become confused if files by those names appear.
#
.PHONY: all tags depend clean cleanhere install includes
