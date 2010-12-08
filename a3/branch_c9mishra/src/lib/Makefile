#
# Makefile for src/lib (sources for libraries installed in /lib)
#
# Note that all includes are found in src/include - the "includes"
# make rule for installing .h files does not come here.
#

include ../defs.mk

all tags depend clean install:
	(cd crt0 && $(MAKE) $@)
	(cd libc && $(MAKE) $@)
	(cd hostcompat && $(MAKE) $@)

clean: cleanhere
cleanhere:
	rm -f *~

.PHONY: all depend tags clean cleanhere install
