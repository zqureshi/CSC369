#include <types.h>
#include <lib.h>
#include <bitmap.h>
#include <test.h>

#define TESTSIZE 533

int
bitmaptest(int nargs, char **args)
{
	struct bitmap *b;
	char data[TESTSIZE];
	u_int32_t x;
	int i;

	(void)nargs;
	(void)args;

	kprintf("Starting bitmap test...\n");

	for (i=0; i<TESTSIZE; i++) {
		data[i] = random()%2;
	}

	b = bitmap_create(TESTSIZE);
	assert(b != NULL);

	for (i=0; i<TESTSIZE; i++) {
		assert(bitmap_isset(b, i)==0);
	}

	for (i=0; i<TESTSIZE; i++) {
		if (data[i]) {
			bitmap_mark(b, i);
		}
	}
	for (i=0; i<TESTSIZE; i++) {
		if (data[i]) {
			assert(bitmap_isset(b, i));
		}
		else {
			assert(bitmap_isset(b, i)==0);
		}
	}

	for (i=0; i<TESTSIZE; i++) {
		if (data[i]) {
			bitmap_unmark(b, i);
		}
		else {
			bitmap_mark(b, i);
		}
	}
	for (i=0; i<TESTSIZE; i++) {
		if (data[i]) {
			assert(bitmap_isset(b, i)==0);
		}
		else {
			assert(bitmap_isset(b, i));
		}
	}

	while (bitmap_alloc(b, &x)==0) {
		assert(x < TESTSIZE);
		assert(bitmap_isset(b, x));
		assert(data[x]==1);
		data[x] = 0;
	}

	for (i=0; i<TESTSIZE; i++) {
		assert(bitmap_isset(b, i));
		assert(data[i]==0);
	}

	kprintf("Bitmap test complete\n");
	return 0;
}
