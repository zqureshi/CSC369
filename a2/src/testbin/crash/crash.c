/*
 * crash.c
 *
 * 	Commit a variety of exceptions, primarily address faults.
 *
 * Once the basic system calls assignment is complete, none of these
 * should crash the kernel.
 *
 * They should all, however, terminate this program, except for the
 * one that writes to the code segment. (That one won't cause program
 * termination until/unless you implement read-only segments in your
 * VM system.)
 */

#include <stdio.h>
#include <unistd.h>
#include <err.h>

#if defined(__mips__)
#define KERNEL_ADDR	0x80000000
#define INVAL_ADDR	0x40000000
#define INSN_TYPE	u_int32_t
#define INVAL_INSN	0x0000003f
#else
#error "Please fix this"
#endif

typedef void (*func)(void);

static
void
read_from_null(void)
{
	int *null = NULL;
	volatile int x;
	x = *null;
}

static
void
read_from_inval(void)
{
	int *ptr = (int *) INVAL_ADDR;
	volatile int x;
	x = *ptr;
}

static
void
read_from_kernel(void)
{
	int *ptr = (int *) KERNEL_ADDR;
	volatile int x;
	x = *ptr;
}

static
void
write_to_null(void)
{
	int *null = NULL;
	*null = 6;
}

static
void
write_to_inval(void)
{
	int *ptr = (int *) INVAL_ADDR;
	*ptr = 8;
}

static
void
write_to_code(void)
{
	INSN_TYPE *x = (INSN_TYPE *)write_to_code;
	*x = INVAL_INSN;
}

static
void
write_to_kernel(void)
{
	int *ptr = (int *) KERNEL_ADDR;
	*ptr = 8;
}

static
void
jump_to_null(void)
{
	func f = NULL;
	f();
}

static
void
jump_to_inval(void)
{
	func f = (func) INVAL_ADDR;
	f();
}

static
void
jump_to_kernel(void)
{
	func f = (func) KERNEL_ADDR;
	f();
}


static
void
illegal_instruction(void)
{
#if defined(__mips__)
	asm(".long 0x0000003f");
#else
#error "Please fix this"
#endif
}

static
void
alignment_error(void)
{
	int x;
	int *ptr = &x;
	int *badptr = (int *)(((char *)ptr)+1);

	volatile int j;
	j = *badptr;
}

static
void
divide_by_zero(void)
{
	volatile int x = 6;
	volatile int z = 0;
	volatile int a;

	a = x/z;
}

static
void
mod_by_zero(void)
{
	volatile int x = 6;
	volatile int z = 0;
	volatile int a;

	a = x%z;
}

static
void
recurse_inf(void)
{
	volatile char buf[16];
	buf[0] = 0;
	recurse_inf();
}


static
struct {
	int ch;
	const char *name;
	func f;
} ops[] = {
	{ 'a', "read from NULL",		read_from_null },
	{ 'b', "read from invalid address",	read_from_inval },
	{ 'c', "read from kernel address",	read_from_kernel },
	{ 'd', "write to NULL",			write_to_null },
	{ 'e', "write to invalid address",	write_to_inval },
	{ 'f', "write to code segment",		write_to_code },
	{ 'g', "write to kernel address",	write_to_kernel },
	{ 'h', "jump to NULL",			jump_to_null },
	{ 'i', "jump to invalid address",	jump_to_inval },
	{ 'j', "jump to kernel address",	jump_to_kernel },
	{ 'k', "alignment error",		alignment_error },
	{ 'l', "illegal instruction",		illegal_instruction },
	{ 'm', "divide by zero",		divide_by_zero },
	{ 'n', "mod by zero",			mod_by_zero },
	{ 'o', "Recurse infinitely",		recurse_inf },
	{ 0, NULL, NULL }
};

int
main(int argc, char **argv)
{
	int op, i, status;
	pid_t pid;

	if (argc == 2) {
		op = argv[1][0];
	}
	else {
		for (i=0; ops[i].name; i++) {
			printf("[%c] %s\n", ops[i].ch, ops[i].name);
		}
		printf("[*] Run everything (in subprocesses)\n");
		printf("Note: [f] may not cause an exception on some "
		       "platforms, in which\ncase it'll appear to fail.\n");

		printf("Choose: ");
		op = getchar();
	}

	if (op=='*') {
		for (i=0; ops[i].name; i++) {
			printf("Running: [%c] %s\n", ops[i].ch, ops[i].name);
			pid = fork();
			if (pid<0) {
				/* error */
				warn("fork");
			}
			else if (pid==0) {
				/* child */
				ops[i].f();
				printf("I wasn't killed - test fails!\n");
				_exit(1);
			}
			waitpid(pid, &status, 0);
			printf("Exit %d\n", status);
		}
	}
	else {
		/* intentionally don't check if op is in bounds :) */
		ops[op-'a'].f();

		printf("I wasn't killed - test fails!\n");
	}
  
	return 0;
}
