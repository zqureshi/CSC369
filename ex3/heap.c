#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/queue.h>

#include "heap.h"

static unsigned h_len;

struct data *heap = NULL;

int getval(int index) {
    assert(index <= h_len);
    return (heap[index]).key;
}

void setval(int index, int value) {
    assert(index <= h_len);
    heap[index].key = value;
}

void dump_h() {
    unsigned int i;
    for(i = 0; i <= h_len; i++) {
        printf("%d ", heap[i].key);
    }
    printf("\n");
}


static void
h_bubble_up(unsigned idx, unsigned v) {
    unsigned ip, pv;

    while (idx > 1) {
        ip = idx / 2;

        pv = getval(ip);
        if (pv < v)
            return;
        setval(ip, v);
        setval(idx, pv);
        idx = ip;
    }
}

static void
h_bubble_down(unsigned idx, unsigned v) {
    unsigned i1, i2, v1, v2;

    while (idx < h_len) {
        i1 = idx * 2;
        i2 = i1 + 1;

        if (i1 != i2 && i2 <= h_len) {
            v1 = getval(i1);
            v2 = getval(i2);
            if (v1 < v && v1 <= v2) {
                setval(i1, v);
                setval(idx, v1);
                idx = i1;
            } else if (v2 < v) {
                setval(i2, v);
                setval(idx, v2);
                idx = i2;
            } else {
                break;
            }
        } else if (i1 <= h_len) {
            v1 = getval(i1);
            if (v1 < v) {
                setval(i1, v);
                setval(idx, v1);
                idx = i1;
            } else {
                break;
            }
        } else
            break;
    }
}

void
h_init(unsigned ntest) {
    void *memptr;
    int r;
    unsigned long size = (ntest + 1) * sizeof(struct data);
    if((r = posix_memalign(&memptr, sysconf(_SC_PAGESIZE), size)) != 0 ) {
        fprintf(stderr, "Error: memalign failed %s\n", strerror(r)); 
    }

    heap = memptr;
    h_len = 0;
}

void
h_insert(unsigned val) {

    h_len++;
    setval(h_len, val);
    h_bubble_up(h_len, val);
}

unsigned
h_remove(void) {
    unsigned val, retval;

    retval = getval(1);
    val = getval(h_len);
    h_len--;
    if (h_len == 0)
        return (retval);
    setval(1, val);
    h_bubble_down(1, val);
    return (retval);
}
