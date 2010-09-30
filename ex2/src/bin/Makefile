#
# Makefile for src/bin (sources for programs installed in /bin)
#

include ../defs.mk

all depend tags clean install:
	(cd true && $(MAKE) $@)
	(cd false && $(MAKE) $@)
	(cd sync && $(MAKE) $@)
	(cd mkdir && $(MAKE) $@)
	(cd rmdir && $(MAKE) $@)
	(cd pwd && $(MAKE) $@)
	(cd cat && $(MAKE) $@)
	(cd cp && $(MAKE) $@)
	(cd ln && $(MAKE) $@)
	(cd mv && $(MAKE) $@)
	(cd rm && $(MAKE) $@)
	(cd ls && $(MAKE) $@)
	(cd sh && $(MAKE) $@)

clean: cleanhere
cleanhere:
	rm -f *~

.PHONY: all depend tags clean cleanhere install
