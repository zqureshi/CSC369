#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "bheap.h"


static unsigned bh_psize;
static unsigned bh_shift;
static unsigned bh_mask;
static unsigned bh_hshift;
static unsigned bh_hmask;
static unsigned bh_half;
static unsigned bh_len;

struct data **heap;
int verbose = 0;


int getval(int pageno, int index) {
    return (heap[pageno][index]).key;
}

void setval(int pageno, int index, int value) {
    heap[pageno][index].key = value;
}

void dump_bh() {
    int page = 0;
    int len = 0;
    int limit = PAGESIZE / sizeof(struct data);  // must divide evenly
    while(heap[page] != NULL) {
        int j = 0;
        while(j < limit && len <= bh_len) {
            if(j >= limit-10){
                printf("%d\n", j);
            };
            printf("%d ", heap[page][j].key);
            len++;
            j++;
        }
        printf("\n");
        page++;
    }
    printf("Num pages used = %d, Num elements = %d\n", page, bh_len);
}

static uintptr_t
bh_rd(unsigned idx) {

    assert(idx <= bh_len);
    return (getval(idx >> bh_shift, idx & bh_mask));
}

static void
bh_wr(unsigned idx, uintptr_t val) {

    assert(idx <= bh_len);
    setval(idx >> bh_shift, idx & bh_mask, val);
}

// returns the page offset
static unsigned
bh_po(unsigned idx) {

            return (idx & bh_mask);
}

static void
bh_bubble_up(unsigned idx, unsigned v) {

    unsigned ip, pv; //ip == index of parent, pv = value of parent
    unsigned po;     //po == page offset

    while (idx > 1) {
        po = bh_po(idx);
        if (idx < bh_psize || po > 3) {
        // we are the top level page or the parent is on
        // the current page
            ip = (idx & ~bh_mask) | (po >> 1);

        } else if (po < 2) {
        // if the index is 0 or 1 then we have to move up a page
            ip = (idx - bh_psize) >> bh_shift;
            ip += (ip & ~bh_hmask);
            ip |= bh_psize / 2;
        } else {
            ip = idx - 2;
        }

        pv = bh_rd(ip);
        if (pv < v)
            return;
        bh_wr(ip, v);
        bh_wr(idx, pv);
        idx = ip;
    }
}

static void 
bh_bubble_down(unsigned idx, unsigned v) {

    unsigned i1, i2, v1, v2;

    while (idx < bh_len) {
        if (idx > bh_mask && !(idx & (bh_mask - 1))) {
                /* first two elements in nonzero pages */
            i1 = i2 = idx + 2;
        } else if (idx & (bh_psize >> 1)) {
                /* Last row of page */
            i1 = (idx & ~bh_mask) >> 1;
            i1 |= idx & (bh_mask >> 1);
            i1 += 1;
            i1 <<= bh_shift;
            i2 = i1 + 1;
        } else {
            i1 = idx + (idx & bh_mask);
            i2 = i1 + 1;
        }
        if (i1 != i2 && i2 <= bh_len) {
            v1 = bh_rd(i1);
            v2 = bh_rd(i2);
            if (v1 < v && v1 <= v2) {
                bh_wr(i1, v);
                bh_wr(idx, v1);
                idx = i1;
            } else if (v2 < v) {
                bh_wr(i2, v);
                bh_wr(idx, v2);
                idx = i2;
            } else {
                break;
            }
        } else if (i1 <= bh_len) {
            v1 = bh_rd(i1);
            if (v1 < v) {
                bh_wr(i1, v);
                bh_wr(idx, v1);
                idx = i1;
            } else {
                break;
            }
        } else
            break;
    }
}

/* Allocate memory for the heap and set up global variables
 * The number of operations or numtests is effectively the size of the tree.
 * The heap is an array of pointers to pages.
 */
void
bh_init(unsigned psz, unsigned numtests) {
    unsigned u;
    unsigned numpages = (numtests * sizeof(struct data))/ psz + 1;
    if(verbose) {
        printf("numpages = %d\n", numpages);
        printf("allcoated %d\n", (numpages + 1) * sizeof(struct data *));
    }
    heap = malloc((numpages + 1) * sizeof(struct data *));
    unsigned i;
    for(i = 0; i < numpages; i++) {
        void *memptr;
		int r;
        // The -8 on the size should not be necessary, but it seemed that
        // memallign was allocating 2 pages if psz == _SC_PAGESIZE
        if((r= posix_memalign(&memptr, sysconf(_SC_PAGESIZE), psz-8)) != 0 ) {
            fprintf(stderr, "Error: memalign failed %s\n", strerror(r));
        }

        heap[i] = memptr;
        if(verbose) {
            printf("heap[%d] = %x\n", i, (unsigned int)heap[i]);
        }
    }
    heap[numpages] = NULL;

    unsigned numelem = psz / sizeof(struct data);
    /* Calculate the log2(numelem) */
    assert((numelem & (numelem - 1)) == 0);	/* Must be power of two */
    for (u = 1; (1U << u) != numelem; u++)
        ;
    bh_shift = u;
    bh_mask = numelem - 1;

    bh_half = numelem / 2;
    bh_hshift = bh_shift - 1;
    bh_hmask = bh_mask >> 1;

    bh_len = 0;
    bh_psize = numelem;
}

void
bh_insert(unsigned val) {

    bh_len++;
    bh_wr(bh_len, val);
    bh_bubble_up(bh_len, val);
}

unsigned
bh_remove(void) {

    unsigned val, retval;

    retval = bh_rd(1);
    val = bh_rd(bh_len);
    bh_len--;
    if (bh_len == 0)
        return (retval);
    bh_wr(1, val);
    bh_bubble_down(1, val);
    return (retval);
}
