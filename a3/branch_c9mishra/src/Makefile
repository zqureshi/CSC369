#
# Toplevel makefile for OS/161.
#
#
# Rules:
#    all (default):  depend, compile, and install system.
#    build:          depend, compile, and install system from scratch.
#    compile:        compile and install binaries.
#    includes:       install header files.
#    tags:           generate/regenerate "tags" files.
#    depend:         update make dependency information.
#    clean:          remove generated files.
#
# The "compile" rule subsumes both compilation and installation,
# because certain parts must be installed before subsequent parts
# can be built. Note that it does *not* install include files.
#
# The "includes" rule installs header files from the kernel and
# libraries into the include directory of the system tree.
#

all:
	$(MAKE) includes
	$(MAKE) depend
	$(MAKE) compile

build:
	$(MAKE) clean
	$(MAKE) all

compile:
	(cd lib && $(MAKE))
	(cd lib && $(MAKE) install)
	(cd bin && $(MAKE))
	(cd sbin && $(MAKE))
	(cd testbin && $(MAKE))
	(cd bin && $(MAKE) install)
	(cd sbin && $(MAKE) install)
	(cd testbin && $(MAKE) install)
	(cd man && $(MAKE) install)

includes:
	(cd kern && $(MAKE) includes)
	(cd include && $(MAKE) includes)
	(cd lib/hostcompat && $(MAKE) includes)

tags depend clean:
	(cd include && $(MAKE) $@)
	(cd lib && $(MAKE) $@)
	(cd bin && $(MAKE) $@)
	(cd sbin && $(MAKE) $@)
	(cd testbin && $(MAKE) $@)
	(cd man && $(MAKE) $@)
	(cd kern && $(MAKE) $@)

clean: cleanhere
cleanhere:
	rm -f *~ mk/*~


.PHONY: all build compile includes tags depend clean cleanhere
