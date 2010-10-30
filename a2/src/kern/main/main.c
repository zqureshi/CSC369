/*
 * Main.
 */

#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <lib.h>
#include <machine/spl.h>
#include <test.h>
#include <synch.h>
#include <thread.h>
#include <scheduler.h>
#include <dev.h>
#include <vfs.h>
#include <vm.h>
#include <syscall.h>
#include <version.h>
#include <pid.h>   // A2: NEW

/*
 * These two pieces of data are maintained by the makefiles and build system.
 * buildconfig is the name of the config file the kernel was configured with.
 * buildversion starts at 1 and is incremented every time you link a kernel. 
 *
 * The purpose is not to show off how many kernels you've linked, but
 * to make it easy to make sure that the kernel you just booted is the
 * same one you just built.
 */
extern const int buildversion;
extern const char buildconfig[];

/*
 * Copyright message for the OS/161 base code.
 */
static const char harvard_copyright[] =
    "Copyright (c) 2000, 2001, 2002, 2003\n"
    "   President and Fellows of Harvard College.  All rights reserved.\n";


/*
 * Initial boot sequence.
 */
static
void
boot(void)
{
	/*
	 * The order of these is important!
	 * Don't go changing it without thinking about the consequences.
	 *
	 * Among other things, be aware that console output gets
	 * buffered up at first and does not actually appear until
	 * dev_bootstrap() attaches the console device. This can be
	 * remarkably confusing if a bug occurs at this point. So
	 * don't put new code before dev_bootstrap if you don't
	 * absolutely have to.
	 *
	 * Also note that the buffer for this is only 1k. If you
	 * overflow it, the system will crash without printing
	 * anything at all. You can make it larger though (it's in
	 * dev/generic/console.c).
	 */

	kprintf("\n");
	kprintf("OS/161 base system version %s\n", BASE_VERSION);
	kprintf("%s", harvard_copyright);
	kprintf("\n");

	kprintf("Grouup-36 system version %s (%s #%d)\n", 
		GROUP_VERSION, buildconfig, buildversion);
	kprintf("\n");

	ram_bootstrap();
	vm_bootstrap();
	scheduler_bootstrap();
    pid_bootstrap();  // A2: NEW
	thread_bootstrap();
	vfs_bootstrap();
	dev_bootstrap();
	kprintf_bootstrap();

	/* Default bootfs - but ignore failure, in case emu0 doesn't exist */
	vfs_setbootfs("emu0");


	/*
	 * Make sure various things aren't screwed up.
	 */
	assert(sizeof(userptr_t)==sizeof(char *));
	assert(sizeof(*(userptr_t)0)==sizeof(char));
}

/*
 * Shutdown sequence. Opposite to boot().
 */
static
void
shutdown(void)
{

	kprintf("Shutting down.\n");
	
	vfs_clearbootfs();
	vfs_clearcurdir();
	vfs_unmountall();

	splhigh();

	scheduler_shutdown();
	thread_shutdown();
}

/*****************************************/

/*
 * reboot() system call.
 *
 * Note: this is here because it's directly related to the code above,
 * not because this is where system call code should go. Other syscall
 * code should probably live in the "userprog" directory.
 */
int
sys_reboot(int code)
{
	switch (code) {
	    case RB_REBOOT:
	    case RB_HALT:
	    case RB_POWEROFF:
		break;
	    default:
		return EINVAL;
	}

	shutdown();

	switch (code) {
	    case RB_HALT:
		kprintf("The system is halted.\n");
		md_halt();
		break;
	    case RB_REBOOT:
		kprintf("Rebooting...\n");
		md_reboot();
		break;
	    case RB_POWEROFF:
		kprintf("The system is halted.\n");
		md_poweroff();
		break;
	}

	panic("reboot operation failed\n");
	return 0;
}

/*
 * Kernel main. Boot up, then fork the menu thread; wait for a reboot
 * request, and then shut down.
 */
int
kmain(char *arguments)
{
	boot();

	menu(arguments);

	/* Should not get here */
	return 0;
}
