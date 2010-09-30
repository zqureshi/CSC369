#
# Makefile for OS/161 user-level include files.
#

include ../defs.mk

# The place include files go.
INCDIR=$(OSTREE)/include

# 
# [ -d $(INCDIR) ] succeeds if $(INCDIR) is a directory.
# (See test(1).) Thus, if $(INCDIR) doesn't exist, it will be
# created.
#
# The -p option to mkdir causes it to create any missing levels
# of intermediate directories.
#
# All *.h files are installed, as are those in the subdirectory "sys".
#
# (Traditionally in Unix, the "sys" include files are the ones that
# come from the kernel, like kern/*.h in OS/161. Unfortunately, some
# poor design decisions put a bunch of user-level declarations in
# various sys/*.h headers, and now many those header names are fixed
# by standards committees and defined to have various user-level
# things in them. So nowadays, if you are being properly careful to
# separate userland from the kernel, you end up with sys/*.h belonging
# to userland and kernel headers going elsewhere.)
#
# To avoid causing huge numbers of unnecessary recompiles, we don't
# install header files unless they're actually different.
#
includes:
	[ -d $(INCDIR) ] || mkdir -p $(INCDIR)
	[ -d $(INCDIR)/sys ] || mkdir -p $(INCDIR)/sys
	@echo "Installing library includes..."
	@for h in *.h sys/*.h; do \
		if diff $$h $(OSTREE)/include/$$h >/dev/null 2>&1; then \
			true; \
		else \
			echo "Installing $$h"; \
			cp $$h $(OSTREE)/include/$$h; \
	        fi; \
	done

install tags depend:;

clean:
	rm -f *~ */*~

#
# This tells make that includes, install, depend, and clean are not files
# so it (hopefully) won't become confused if files by those names appear.
#
.PHONY: includes install depend clean
