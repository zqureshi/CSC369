#
# OS/161 makefile template for libraries
#
# defs.mk should already have been included.
# That sets:
#    OSTREE	Directory with root of installed OS/161 system
#    CC		C compiler 
#    CFLAGS	C compiler flags
#    AR		archiver (librarian)
#    RANLIB	library indexer
#    TREE_CFLAGS Additional flags to point the toolchain at $(OSTREE)
#
# You should set:
#    LIB	Name of library to build (lib$(LIB).a is created)
#    SRCS	.c and .S files to build into library.
#
# Supports the following rules:
#
#    all	Compile and build library
#    clean	Erase generated files
#    depend	Update make dependency information
#    install	Install library in OS/161 system library directory
#    tags	Update "tags" database
#


#
# The list of .o files is the list of .c and .S files with those suffixes
# changed to .o.
#
OBJS1=$(SRCS:.c=.o)
OBJS=$(OBJS1:.S=.o)

#
# Default rule - create library.
#
all: lib$(LIB).a

#
# Delete everything extraneous.
#
clean:
	rm -f *.o *.a *~

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

depend:
	$(CC) $(TREE_CFLAGS) $(CFLAGS) -M $(SRCS) |\
	  awk '{x=$$0~"^ ";for(i=1;i<=NF;i++){printf "%d %s\n",x,$$i;x=1; }}'|\
	  sed '/1 \\/d' | awk '{ printf "%s%s", $$1?" \\\n ":"\n", $$2 }' |\
	  sed 's|$(OSTREE)|$$(OSTREE)|;$$p;$$x' > depend.mk
include depend.mk

#
# [ -d $(OSTREE)/lib ] succeeds if $(OSTREE)/lib is a directory.
# (See test(1).) Thus, if $(OSTREE)/lib doesn't exist, it will be
# created.
#
install:
	[ -d $(OSTREE)/lib ] || mkdir $(OSTREE)/lib
	cp lib$(LIB).a $(OSTREE)/lib

#
# Run ctags to update the tags database.
#
tags:
	ctags -wtd $(SRCS)

#
# Create the library.
#
lib$(LIB).a: $(OBJS)
	$(AR) -cruv lib$(LIB).a $(OBJS)
	$(RANLIB) lib$(LIB).a

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
