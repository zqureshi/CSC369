#
# Makefile for src/man (man page tree)
#

include ../defs.mk

all depend tags clean install:
	(cd toplevel && $(MAKE) $@)
	(cd bin && $(MAKE) $@)
	(cd sbin && $(MAKE) $@)
	(cd testbin && $(MAKE) $@)
	(cd libc && $(MAKE) $@)
	(cd syscall && $(MAKE) $@)
	(cd dev && $(MAKE) $@)
	(cd misc && $(MAKE) $@)

clean: cleanhere
cleanhere:
	rm -f *~

.PHONY: all depend tags clean cleanhere install
