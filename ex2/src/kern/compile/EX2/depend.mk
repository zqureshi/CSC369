console.o: ../../dev/generic/console.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/errno.h ../../include/lib.h machine/setjmp.h \
  machine/spl.h ../../include/synch.h ../../dev/generic/console.h \
  ../../include/dev.h ../../include/vfs.h ../../include/uio.h autoconf.h
emu.o: ../../dev/lamebus/emu.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/kern/unistd.h ../../include/kern/stat.h \
  ../../include/lib.h machine/setjmp.h ../../include/synch.h \
  ../../include/array.h ../../include/uio.h ../../include/vfs.h \
  ../../include/emufs.h ../../include/vnode.h ../../include/fs.h \
  ../../dev/lamebus/emu.h machine/bus.h machine/vm.h \
  ../../dev/lamebus/lamebus.h autoconf.h
lser.o: ../../dev/lamebus/lser.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h machine/spl.h machine/bus.h machine/vm.h \
  ../../dev/lamebus/lamebus.h ../../dev/lamebus/lser.h autoconf.h
lhd.o: ../../dev/lamebus/lhd.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/synch.h ../../include/kern/errno.h \
  machine/bus.h machine/vm.h ../../dev/lamebus/lamebus.h \
  ../../include/uio.h ../../include/vfs.h ../../dev/lamebus/lhd.h \
  ../../include/dev.h autoconf.h
pseudorand.o: ../../dev/generic/pseudorand.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/uio.h \
  ../../dev/generic/pseudorand.h autoconf.h
random.o: ../../dev/generic/random.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/errno.h ../../include/kern/unistd.h \
  ../../include/lib.h machine/setjmp.h ../../include/uio.h \
  ../../include/vfs.h ../../dev/generic/random.h ../../include/dev.h \
  autoconf.h
ltimer.o: ../../dev/lamebus/ltimer.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/clock.h \
  machine/bus.h machine/vm.h ../../dev/lamebus/lamebus.h \
  ../../dev/lamebus/ltimer.h autoconf.h
lamebus.o: ../../dev/lamebus/lamebus.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h machine/spl.h \
  ../../dev/lamebus/lamebus.h
lrandom.o: ../../dev/lamebus/lrandom.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/uio.h machine/bus.h \
  machine/vm.h ../../dev/lamebus/lamebus.h ../../dev/lamebus/lrandom.h \
  autoconf.h
beep.o: ../../dev/generic/beep.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h ../../dev/generic/beep.h \
  autoconf.h
ltrace.o: ../../dev/lamebus/ltrace.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h machine/bus.h machine/vm.h \
  ../../dev/lamebus/lamebus.h ../../dev/lamebus/ltrace.h autoconf.h
rtclock.o: ../../dev/generic/rtclock.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/errno.h ../../include/lib.h machine/setjmp.h \
  ../../include/clock.h ../../dev/generic/rtclock.h autoconf.h
ltimer_att.o: ../../dev/lamebus/ltimer_att.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/lamebus/lamebus.h \
  ../../dev/lamebus/ltimer.h autoconf.h
con_lser.o: ../../dev/lamebus/con_lser.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/generic/console.h \
  ../../dev/lamebus/lser.h autoconf.h
lhd_att.o: ../../dev/lamebus/lhd_att.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/lamebus/lamebus.h \
  ../../dev/lamebus/lhd.h ../../include/dev.h autoconf.h
random_lrandom.o: ../../dev/lamebus/random_lrandom.c \
  ../../include/types.h machine/types.h ../../include/kern/types.h \
  machine/ktypes.h ../../include/lib.h machine/setjmp.h \
  ../../dev/generic/random.h ../../include/dev.h \
  ../../dev/lamebus/lrandom.h autoconf.h
rtclock_ltimer.o: ../../dev/lamebus/rtclock_ltimer.c \
  ../../include/types.h machine/types.h ../../include/kern/types.h \
  machine/ktypes.h ../../include/lib.h machine/setjmp.h \
  ../../dev/generic/rtclock.h ../../dev/lamebus/ltimer.h autoconf.h
ltrace_att.o: ../../dev/lamebus/ltrace_att.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/lamebus/lamebus.h \
  ../../dev/lamebus/ltrace.h autoconf.h
lser_att.o: ../../dev/lamebus/lser_att.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/lamebus/lamebus.h \
  ../../dev/lamebus/lser.h autoconf.h
lrandom_att.o: ../../dev/lamebus/lrandom_att.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/lamebus/lamebus.h \
  ../../dev/lamebus/lrandom.h autoconf.h
emu_att.o: ../../dev/lamebus/emu_att.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/lamebus/lamebus.h \
  ../../dev/lamebus/emu.h autoconf.h
beep_ltimer.o: ../../dev/lamebus/beep_ltimer.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../dev/generic/beep.h \
  ../../dev/lamebus/ltimer.h autoconf.h
pseudorand_att.o: ../../dev/generic/pseudorand_att.c \
  ../../include/types.h machine/types.h ../../include/kern/types.h \
  machine/ktypes.h ../../include/lib.h machine/setjmp.h \
  ../../dev/generic/random.h ../../include/dev.h \
  ../../dev/generic/pseudorand.h autoconf.h
dumbvm.o: ../../arch/mips/mips/dumbvm.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/errno.h ../../include/lib.h machine/setjmp.h \
  ../../include/thread.h machine/pcb.h ../../include/curthread.h \
  ../../include/addrspace.h ../../include/vm.h machine/vm.h opt-dumbvm.h \
  machine/spl.h machine/tlb.h ../../include/bitmap.h
cache_mips1.o: ../../arch/mips/mips/cache_mips1.S machine/asmdefs.h
exception.o: ../../arch/mips/mips/exception.S machine/asmdefs.h \
  machine/specialreg.h
lamebus_mips.o: ../../arch/mips/mips/lamebus_mips.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/unistd.h ../../include/lib.h machine/setjmp.h \
  ../../include/synch.h machine/spl.h machine/pcb.h ../../include/dev.h \
  machine/bus.h machine/vm.h ../../dev/lamebus/lamebus.h autoconf.h
interrupt.o: ../../arch/mips/mips/interrupt.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h machine/bus.h machine/vm.h \
  ../../dev/lamebus/lamebus.h machine/spl.h machine/pcb.h
pcb.o: ../../arch/mips/mips/pcb.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h machine/pcb.h machine/spl.h machine/switchframe.h \
  ../../include/thread.h
ram.o: ../../arch/mips/mips/ram.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/vm.h machine/vm.h machine/pcb.h
spl.o: ../../arch/mips/mips/spl.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h machine/spl.h machine/specialreg.h
start.o: ../../arch/mips/mips/start.S machine/asmdefs.h \
  machine/specialreg.h
switch.o: ../../arch/mips/mips/switch.S machine/asmdefs.h
syscall.o: ../../arch/mips/mips/syscall.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/errno.h ../../include/lib.h machine/setjmp.h \
  machine/pcb.h machine/spl.h machine/trapframe.h \
  ../../include/kern/callno.h ../../include/syscall.h
threadstart.o: ../../arch/mips/mips/threadstart.S machine/asmdefs.h
trap.o: ../../arch/mips/mips/trap.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h machine/trapframe.h machine/specialreg.h machine/pcb.h \
  machine/spl.h ../../include/vm.h machine/vm.h ../../include/thread.h \
  ../../include/curthread.h
tlb_mips1.o: ../../arch/mips/mips/tlb_mips1.S machine/asmdefs.h \
  machine/specialreg.h
mips-setjmp.o: ../../../lib/libc/mips-setjmp.S machine/asmdefs.h
copyinout.o: ../../lib/copyinout.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h machine/pcb.h ../../include/vm.h \
  machine/vm.h ../../include/thread.h ../../include/curthread.h
array.o: ../../lib/array.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h ../../include/array.h
bitmap.o: ../../lib/bitmap.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/kern/errno.h ../../include/bitmap.h
queue.o: ../../lib/queue.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h ../../include/queue.h
kheap.o: ../../lib/kheap.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/vm.h machine/vm.h machine/spl.h \
  opt-dumbvm.h
kprintf.o: ../../lib/kprintf.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/stdarg.h \
  ../../include/lib.h machine/setjmp.h ../../include/kern/unistd.h \
  ../../include/synch.h ../../include/vfs.h ../../include/thread.h \
  machine/pcb.h machine/spl.h
kgets.o: ../../lib/kgets.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h
misc.o: ../../lib/misc.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errmsg.h \
  ../../include/lib.h machine/setjmp.h
ntoh.o: ../../lib/ntoh.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h
__printf.o: ../../../lib/libc/__printf.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/stdarg.h
snprintf.o: ../../../lib/libc/snprintf.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/stdarg.h
atoi.o: ../../../lib/libc/atoi.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h
bzero.o: ../../../lib/libc/bzero.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h
memcpy.o: ../../../lib/libc/memcpy.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
memmove.o: ../../../lib/libc/memmove.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
strcat.o: ../../../lib/libc/strcat.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
strchr.o: ../../../lib/libc/strchr.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
strcmp.o: ../../../lib/libc/strcmp.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
strcpy.o: ../../../lib/libc/strcpy.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
strlen.o: ../../../lib/libc/strlen.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
strrchr.o: ../../../lib/libc/strrchr.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
strtok_r.o: ../../../lib/libc/strtok_r.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h
init.o: ../../dev/init.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h machine/spl.h ../../include/dev.h autoconf.h
device.o: ../../fs/vfs/device.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/synch.h ../../include/kern/errno.h \
  ../../include/kern/unistd.h ../../include/kern/stat.h \
  ../../include/vnode.h ../../include/uio.h ../../include/dev.h
vfscwd.o: ../../fs/vfs/vfscwd.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/kern/stat.h ../../include/lib.h machine/setjmp.h \
  ../../include/vfs.h ../../include/fs.h ../../include/vnode.h \
  ../../include/uio.h ../../include/thread.h machine/pcb.h \
  ../../include/curthread.h
vfslist.o: ../../fs/vfs/vfslist.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/synch.h ../../include/array.h \
  ../../include/kern/errno.h ../../include/vfs.h ../../include/vnode.h \
  ../../include/fs.h ../../include/dev.h
vfslookup.o: ../../fs/vfs/vfslookup.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/errno.h ../../include/kern/limits.h \
  ../../include/lib.h machine/setjmp.h ../../include/synch.h \
  ../../include/vfs.h ../../include/vnode.h ../../include/fs.h
vfspath.o: ../../fs/vfs/vfspath.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/kern/limits.h ../../include/kern/unistd.h \
  ../../include/vfs.h ../../include/vnode.h ../../include/lib.h \
  machine/setjmp.h
vnode.o: ../../fs/vfs/vnode.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h ../../include/synch.h \
  ../../include/vnode.h
devnull.o: ../../fs/vfs/devnull.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h ../../include/vfs.h \
  ../../include/dev.h ../../include/uio.h
hardclock.o: ../../thread/hardclock.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h machine/spl.h \
  ../../include/thread.h machine/pcb.h ../../include/clock.h
synch.o: ../../thread/synch.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/synch.h ../../include/thread.h \
  machine/pcb.h ../../include/curthread.h machine/spl.h
scheduler.o: ../../thread/scheduler.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/scheduler.h \
  ../../include/thread.h machine/pcb.h machine/spl.h \
  ../../include/queue.h
thread.o: ../../thread/thread.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/kern/errno.h ../../include/array.h \
  machine/spl.h machine/pcb.h ../../include/thread.h \
  ../../include/curthread.h ../../include/scheduler.h \
  ../../include/addrspace.h ../../include/vm.h machine/vm.h opt-dumbvm.h \
  ../../include/vnode.h
main.o: ../../main/main.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/kern/unistd.h ../../include/lib.h machine/setjmp.h \
  machine/spl.h ../../include/test.h ../../include/synch.h \
  ../../include/thread.h machine/pcb.h ../../include/scheduler.h \
  ../../include/dev.h ../../include/vfs.h ../../include/vm.h machine/vm.h \
  ../../include/syscall.h ../../include/version.h
menu.o: ../../main/menu.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/kern/unistd.h ../../include/kern/limits.h \
  ../../include/lib.h machine/setjmp.h ../../include/clock.h \
  ../../include/thread.h machine/pcb.h ../../include/syscall.h \
  ../../include/uio.h ../../include/vfs.h ../../include/sfs.h \
  ../../include/vnode.h ../../include/fs.h ../../include/kern/sfs.h \
  ../../include/test.h opt-sfs.h opt-net.h
loadelf.o: ../../userprog/loadelf.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h ../../include/uio.h \
  ../../include/elf.h ../../include/addrspace.h ../../include/vm.h \
  machine/vm.h opt-dumbvm.h ../../include/thread.h machine/pcb.h \
  ../../include/curthread.h ../../include/vnode.h
runprogram.o: ../../userprog/runprogram.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/kern/unistd.h ../../include/kern/errno.h \
  ../../include/lib.h machine/setjmp.h ../../include/addrspace.h \
  ../../include/vm.h machine/vm.h opt-dumbvm.h ../../include/thread.h \
  machine/pcb.h ../../include/curthread.h ../../include/vfs.h \
  ../../include/test.h
uio.o: ../../userprog/uio.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/uio.h ../../include/thread.h \
  machine/pcb.h ../../include/curthread.h
simple_syscalls.o: ../../userprog/simple_syscalls.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/thread.h \
  machine/pcb.h
arraytest.o: ../../test/arraytest.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/array.h ../../include/test.h
bitmaptest.o: ../../test/bitmaptest.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/bitmap.h \
  ../../include/test.h
queuetest.o: ../../test/queuetest.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/queue.h ../../include/test.h
threadtest.o: ../../test/threadtest.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/synch.h \
  ../../include/thread.h machine/pcb.h ../../include/test.h
tt3.o: ../../test/tt3.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h machine/spl.h ../../include/synch.h \
  ../../include/thread.h machine/pcb.h ../../include/test.h
synchtest.o: ../../test/synchtest.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/lib.h \
  machine/setjmp.h ../../include/synch.h ../../include/thread.h \
  machine/pcb.h ../../include/test.h ../../include/clock.h
malloctest.o: ../../test/malloctest.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../include/synch.h \
  ../../include/thread.h machine/pcb.h ../../include/test.h
fstest.o: ../../test/fstest.c ../../include/types.h machine/types.h \
  ../../include/kern/types.h machine/ktypes.h ../../include/kern/errno.h \
  ../../include/kern/unistd.h ../../include/lib.h machine/setjmp.h \
  ../../include/synch.h ../../include/fs.h ../../include/vnode.h \
  ../../include/vfs.h ../../include/uio.h ../../include/test.h \
  ../../include/thread.h machine/pcb.h
autoconf.o: ../../compile/EX2/autoconf.c ../../include/types.h \
  machine/types.h ../../include/kern/types.h machine/ktypes.h \
  ../../include/lib.h machine/setjmp.h ../../compile/EX2/autoconf.h
