#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "vector.h"

typedef struct {
    int value;
    void* data;
} mheap_element;
typedef vector mheap;

void mheap_push(mheap *h, void *data, int priority);
void mheap_pop(mheap *h, void** data, int* priority);
void mheap_peek(mheap *h, void** data, int* priority);
mheap* make_mheap();
void print_heap(mheap *h);
void destroy_mheap(mheap *h);

#endif
