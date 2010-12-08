#
# Makefile for crt0.o
#

include ../../defs.mk

all: crt0.o

# Assemble the crt0 for the current platform. Depend on defs.mk in case
# the platform gets changed.
crt0.o: $(PLATFORM)-crt0.S ../../defs.mk
	$(CC) $(TREE_CFLAGS) $(CFLAGS) -c $(PLATFORM)-crt0.S -o crt0.o

# Remove extraneous files.
clean:
	rm -f *.o *~

# tags/depend don't need to do anything.
tags depend:;

#
# [ -d $(OSTREE)/lib ] succeeds if $(OSTREE)/lib is a directory.
# (See test(1).) Thus, if $(OSTREE)/lib doesn't exist, it will be
# created.
#
install:
	[ -d $(OSTREE)/lib ] || mkdir $(OSTREE)/lib
	cp crt0.o $(OSTREE)/lib
