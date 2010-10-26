#
# Makefile for src/sbin (sources for programs installed in /sbin)
#

include ../defs.mk

all depend tags clean install:
	(cd reboot && $(MAKE) $@)
	(cd halt && $(MAKE) $@)
	(cd poweroff && $(MAKE) $@)
	(cd mksfs && $(MAKE) $@)
	(cd dumpsfs && $(MAKE) $@)

clean: cleanhere
cleanhere:
	rm -f *~

.PHONY: all depend tags clean cleanhere install
