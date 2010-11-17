#
# OS/161 makefile template for programs
#
# defs.mk should already have been included.
# That sets:
#    OSTREE	Directory with root of installed OS/161 system
#    CC		C compiler 
#    CFLAGS	C compiler flags
#    LDCC	C compiler for linking
#    LDFLAGS	C compiler flags when linking
#    LIBS	Libraries to link with
#    TREE_CFLAGS,
#    TREE_LDFLAGS,
#    TREE_LIBS	Additional flags/libs to point the toolchain at $(OSTREE)
#
# You should set:
#    PROG	Name of program to build
#    SRCS	.c and .S files that are part of the program
#    BINDIR	Directory to install program in, as if the system were
#       	installed in /, such as "/bin" or "/sbin".
#		(We prepend $(OSTREE) for you.)
#


#
# The list of .o files is the list of .c and .S files with those suffixes
# changed to .o.
#
OBJS1=$(SRCS:.c=.o)
OBJS=$(OBJS1:.S=.o)

#
# Default rule - create program.
#
all: $(PROG)

#
# Delete everything extraneous.
#
clean: progclean
progclean:
	rm -f *.o *.a *~ $(PROG)

#
# Use the -M argument to gcc to get it to output dependency information.
# Note that we use -M, which includes deps for #include <...> files,
# rather than -MM, which doesn't. This is because we are the operating
# system: the #include <...> files are part of our project and we may 
# well change them!
#
# The sed command replaces the value of $(OSTREE) - which is some pathname -
# with the string $(OSTREE). This makes the depend.mk file independent
# of what $(OSTREE) actually is.
#

depend.mk:
	echo "" > depend.mk

depend: progdepend
progdepend:
	$(CC) $(TREE_CFLAGS) $(CFLAGS) -M $(SRCS) |\
	  awk '{x=$$0~"^ ";for(i=1;i<=NF;i++){printf "%d %s\n",x,$$i;x=1; }}'|\
	  sed '/1 \\/d' | awk '{ printf "%s%s", $$1?" \\\n ":"\n", $$2 }' |\
	  sed 's|$(OSTREE)|$$(OSTREE)|;$$p;$$x' > .deptmp
	mv -f .deptmp depend.mk
include depend.mk

#
# [ -d $(OSTREE)/$(BINDIR) ] succeeds if $(OSTREE)/$(BINDIR) is a directory.
# (See test(1).) Thus, if $(OSTREE)/$(BINDIR) doesn't exist, it will be
# created.
#
install: proginstall
proginstall:
	[ -d $(OSTREE)$(BINDIR) ] || mkdir $(OSTREE)$(BINDIR)
	cp $(PROG) $(OSTREE)$(BINDIR)

#
# Run ctags to update the tags database.
#
tags: progtags
progtags:
	ctags -wtd $(SRCS) *.h

#
# Link the program.
# Explicitly depend on crt0.o (the startup code) and libc.a (the C library).
#

$(PROG): $(OBJS) $(OSTREE)/lib/crt0.o $(OSTREE)/lib/libc.a
	$(LDCC) $(TREE_LDFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) $(TREE_LIBS) \
	   -o $(PROG)

#
# Generic make rule for compiling .c files into .o files.
#
.c.o:
	$(CC) $(TREE_CFLAGS) $(CFLAGS) -c $<

#
# Generic make rule for compiling .S files (assembler to be fed through cpp)
# into .o files. gcc knows how to do this, so just use it.
#
.S.o:
	$(CC) $(TREE_CFLAGS) $(CFLAGS) -c $<

#
# This tells make that all, clean, depend, install, and tags are not files
# so it (hopefully) won't become confused if files by those names appear.
#
.PHONY: all clean depend install tags
.PHONY: progclean progdepend proginstall
