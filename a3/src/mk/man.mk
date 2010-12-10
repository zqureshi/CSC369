#
# OS/161 makefile template for man pages.
#
# defs.mk should already have been included.
# That sets:
#    OSTREE	Directory with root of installed OS/161 system
#
# You should set:
#    MANDIR	Directory to install pages in, as if the system were
#       	installed in /, such as "/man/bin" or "/man/sbin".
#		(We prepend $(OSTREE) for you.)
#
# All .html files are installed.
#

#
# Default rule: do nothing.
# If we were generating some pages, we'd build them here.
#
all: ;

# Clean: all we delete are old editor backups
clean:
	rm -f *~

# Depend, tags: nothing to do
depend tags: ;

#
# Install. This does all the work.
#
install:
	[ -d $(OSTREE)$(MANDIR) ] || mkdir $(OSTREE)$(MANDIR)
	chmod a+rx $(OSTREE)$(MANDIR)
	cp *.html $(OSTREE)$(MANDIR)
	chmod a+r $(OSTREE)$(MANDIR)/*.html

#
# This tells make that all, clean, depend, install, and tags are not files
# so it (hopefully) won't become confused if files by those names appear.
#
.PHONY: all clean depend install tags
