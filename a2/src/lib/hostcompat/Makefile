#
# Makefile for hostcompat library
#
# defs.mk contains two special settings for us.
#
#   COMPAT_CFLAGS  contains our configuration cflags.
#   COMPAT_TARGETS are additional targets to run at install time.
#
# COMPAT_CFLAGS may include any of the following:
#
#   -DNEED_ERR     Compile err, errx, etc.
#
# COMPAT_TARGETS may include any of the following:
#
#   install-errh   Install an <err.h>
#

LIB=hostcompat
SRCS=err.c time.c hostcompat.c

include ../../defs.mk

########################################

# .ho is a host object; .ha is a host archive (library)
.SUFFIXES: .ho .ha

#
# The list of .ho files is the list of .c and .S files with those suffixes
# changed to .ho.
#
OBJS1=$(SRCS:.c=.ho)
OBJS=$(OBJS1:.S=.ho)

#
# Default rule - create library.
#
all: lib$(LIB).ha

#
# Delete everything extraneous.
#
clean:
	rm -f *.ho *.ha *~

#
# Use the -MM argument to gcc to get it to output dependency information.
# Note that in this case we use -MM and not -M, because the #include <...>
# files we're using aren't our own but belong to the host OS.
#
# The sed command replaces the value of $(OSTREE) - which is some pathname -
# with the string $(OSTREE). This makes the dependh.mk file independent
# of what $(OSTREE) actually is.
#
depend:
	$(HOST_CC) $(HOST_CFLAGS) $(COMPAT_CFLAGS) -MM $(SRCS) |\
	  awk '{x=$$0~"^ ";for(i=1;i<=NF;i++){printf "%d %s\n",x,$$i;x=1; }}'|\
	  sed '/1 \\/d' | awk '{ printf "%s%s", $$1?" \\\n ":"\n", $$2 }' |\
	  sed 's/\.o/\.ho/' |\
	  sed 's|$(OSTREE)|$$(OSTREE)|;$$p;$$x' > dependh.mk
include dependh.mk

#
# [ -d $(OSTREE)/hostlib ] succeeds if $(OSTREE)/hostlib is a directory.
# (See test(1).) Thus, if $(OSTREE)/hostlib doesn't exist, it will be
# created.
#
# On certain systems with broken native linkers (most notably OS X) you 
# have to rerun ranlib again after installing a library, or you can't
# link with it.
#
install:
	[ -d $(OSTREE)/hostlib ] || mkdir $(OSTREE)/hostlib
	cp lib$(LIB).ha $(OSTREE)/hostlib/lib$(LIB).a
	$(HOST_RANLIB) $(OSTREE)/hostlib/lib$(LIB).a

includes:
	[ -d $(OSTREE)/hostinclude ] || mkdir $(OSTREE)/hostinclude
	rm -f $(OSTREE)/hostinclude/kern
	ln -s ../include/kern $(OSTREE)/hostinclude/kern
	cp hostcompat.h $(OSTREE)/hostinclude

#
# No tags here.
#
tags:;

#
# Create the library.
#
lib$(LIB).ha: $(OBJS)
	$(HOST_AR) -cruv lib$(LIB).ha $(OBJS)
	$(HOST_RANLIB) lib$(LIB).ha

#
# Generic make rule for compiling .c files into .ho files.
#
.c.ho:
	$(HOST_CC) $(HOST_CFLAGS) $(COMPAT_CFLAGS) -c $< -o $@

#
# Generic make rule for compiling .S files (assembler to be fed through cpp)
# into .o files. gcc knows how to do this, so just use it.
#
.S.o:
	$(HOST_CC) $(HOST_CFLAGS) $(COMPAT_CFLAGS) -c $< -o $@

#
# This tells make that all, clean, depend, install, and tags are not files
# so it (hopefully) won't become confused if files by those names appear.
#
.PHONY: all clean depend install tags

########################################

includes: $(COMPAT_TARGETS)

install-errh:
	[ -d $(OSTREE)/hostinclude ] || mkdir $(OSTREE)/hostinclude
	cp host-err.h $(OSTREE)/hostinclude/err.h

.PHONY: install-errh
