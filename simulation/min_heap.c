#include <stdlib.h>
#include <stdio.h>

#include "min_heap.h"

void swap(mheap *h, int i, int j) {
    mheap_element old = h->e[i];
    h->e[i] = h->e[j];
    h->e[j] = old;
}

void upheap(mheap *h, int i) {
    if (i == 0) return;

    int parent = (i-1)/2;

    if (h->e[parent].value > h->e[i].value) {
        swap(h, parent, i);
        upheap(h, parent);
    }
}

void mheap_push(mheap *h, void *data, int priority) {
    if (h->capacity == h->length) {
        if (h->e == NULL) {
            h->e = malloc(100 * sizeof(mheap_element));
            h->capacity = 100;
        } else {
            h->e = realloc(h->e, h->capacity*2 * sizeof(mheap_element));
            if (h->e == NULL) exit(1);
            h->capacity *= 2;
        }
    }

    h->e[h->length].value = priority;
    h->e[h->length].data = data;
    upheap(h, h->length);

    h->length++;
}

void print_heap(mheap *h) {
    for (int i = 0; i < h->length; i++) printf("%d ", h->e[i]);
    printf("\n");
}

void downheap(mheap *h, int i) {
    int child_left = i*2 + 1;
    int child_right = i*2 + 2;
    int child = child_left;
    if (child_left >= h->length) return;

    if (child_right >= h->length) {
        child = child_left;
    } else if (h->e[child_right].value < h->e[child_left].value) {
        child = child_right;
    }

    if (h->e[child].value < h->e[i].value) {
        swap(h, child, i);
        downheap(h, child);
    }
}

void mheap_pop(mheap *h, void** data, int* priority) {
    *data = h->e[0].data;
    *priority = h->e[0].value;

    h->length--;

    if (h->length > 0) {
        h->e[0] = h->e[h->length];
        downheap(h, 0);
    }
}

void mheap_peek(mheap *h, void** data, int* priority) {
    *data = h->e[0].data;
    *priority = h->e[0].value;
}

void destroy_mheap(mheap *h) {
    free((void*)h->e);
    free((void*)h);
}

mheap* make_mheap() {
    mheap *h = malloc(sizeof(mheap));
    h->e = NULL;
    h->length = 0;
    h->capacity = 0;

    return h;
}
