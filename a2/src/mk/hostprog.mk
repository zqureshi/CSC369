#
# OS/161 makefile template for programs to be built on the host OS
#
# defs.mk should already have been included.
# That sets:
#    OSTREE	  Directory with root of installed OS/161 system
#    HOST_CC	  C compiler 
#    HOST_CFLAGS  C compiler flags
#    HOST_LDCC	  C compiler for linking
#    HOST_LDFLAGS C compiler flags when linking
#    HOST_LIBS	  Libraries to link with
#
# You should set:
#    PROG	Name of program to build (host-$(PROG) will be built)
#    SRCS	.c and .S files that are part of the program
#

# .ho is a host object
.SUFFIXES: .ho

#
# The list of .ho files is the list of .c and .S files with those suffixes
# changed to .ho.
#
HOST_OBJS1=$(SRCS:.c=.ho)
HOST_OBJS=$(HOST_OBJS1:.S=.ho)

#
# Default rule - create program.
#
all: host-$(PROG)

#
# Delete everything extraneous.
#
clean: hostclean
hostclean:
	rm -f *.ho *.ha *~ host-$(PROG)

#
# Use the -MM argument to gcc to get it to output dependency information.
# For host versions we use -MM rather than -M so we don't include deps for
# include files we don't own.
#
# The sed command replaces the value of $(OSTREE) - which is some pathname -
# with the string $(OSTREE). This makes the depend.mk file independent
# of what $(OSTREE) actually is.
#
depend: hostdepend
hostdepend:
	$(HOST_CC) $(HOST_CFLAGS) -DHOST -MM $(SRCS) |\
	  awk '{x=$$0~"^ ";for(i=1;i<=NF;i++){printf "%d %s\n",x,$$i;x=1; }}'|\
	  sed '/1 \\/d' | awk '{ printf "%s%s", $$1?" \\\n ":"\n", $$2 }' |\
	  sed 's/\.o/\.ho/' |\
	  sed 's|$(OSTREE)|$$(OSTREE)|;$$p;$$x' > .deptmp
	mv -f .deptmp dependh.mk
include dependh.mk

#
# [ -d $(OSTREE)/hostbin ] succeeds if $(OSTREE)/hostbin is a directory.
# (See test(1).) Thus, if $(OSTREE)/hostbin doesn't exist, it will be
# created.
#
install: hostinstall
hostinstall:
	[ -d $(OSTREE)/hostbin ] || mkdir $(OSTREE)/hostbin
	cp host-$(PROG) $(OSTREE)/hostbin

#
# No tags for host programs.
#
tags: hosttags
hosttags:;

#
# Link the program.
# Depend on libhostcompat.
#

host-$(PROG): $(HOST_OBJS) $(OSTREE)/hostlib/libhostcompat.a
	$(HOST_LDCC) $(HOST_LDFLAGS) $(HOST_OBJS) $(HOST_LIBS) \
	  $(OSTREE)/hostlib/libhostcompat.a -o host-$(PROG)

#
# Generic make rule for compiling .c files into .ho files.
#
.c.ho:
	$(HOST_CC) $(HOST_CFLAGS) -DHOST -c $< -o $@

#
# Generic make rule for compiling .S files (assembler to be fed through cpp)
# into .ho files. gcc knows how to do this, so just use it.
#
.S.ho:
	$(HOST_CC) $(HOST_CFLAGS) -DHOST -c $< -o $@

#
# This tells make that all, clean, depend, install, and tags are not files
# so it (hopefully) won't become confused if files by those names appear.
#
.PHONY: all clean depend install tags
.PHONY: hostclean hostdepend hostinstall
