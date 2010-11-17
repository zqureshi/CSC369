
dumpsfs.ho: \
 dumpsfs.c \
 support.h \
 $(OSTREE)/hostinclude/kern/sfs.h \
 $(OSTREE)/hostinclude/hostcompat.h \
 disk.h
disk.ho: \
 disk.c \
 support.h \
 disk.h
support.ho: \
 support.c

