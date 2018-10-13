#include <stdlib.h>
#include <stdio.h>

#include "min_heap.h"

void upheap(mheap *h, int i) {
    if (i == 0) return;

    int parent = (i-1)/2;
    mheap_element *p, *c;
    p = (mheap_element*)vector_get(h, parent);
    c = (mheap_element*)vector_get(h, i);

    if (p->value > c->value) {
        vector_swap(h, parent, i);
        upheap(h, parent);
    }
}

void mheap_push(mheap *h, void *data, int priority) {
    mheap_element e;
    e.value = priority;
    e.data = data;
    vector_push(h, &e);
    upheap(h, h->length - 1);
}

void print_heap(mheap *h) {
    for (int i = 0; i < h->length; i++) {
        int pri = ((mheap_element*)h->e + i)->value;
        printf("%d ", pri);
    }
    printf("\n");
}

void downheap(mheap *h, int i) {
    int child_left = i*2 + 1;
    int child_right = i*2 + 2;
    int child;
    if (child_left >= h->length) return;

    mheap_element *left, *right, *c, *parent;
    left = (mheap_element*)vector_get(h, child_left);
    parent = (mheap_element*)vector_get(h, i);

    if (child_right >= h->length) {
        child = child_left;
        c = left;
    } else {
        right = (mheap_element*)vector_get(h, child_right);
        if (right->value <= left->value) {
            child = child_right;
            c = right;
        } else {
            child = child_left;
            c = left;
        }
    }

    if (c->value < parent->value) {
        vector_swap(h, child, i);
        downheap(h, child);
    }
}

void mheap_peek(mheap *h, void** data, int* priority) {
    mheap_element *e = vector_get(h, 0);
    *data = e->data;
    *priority = e->value;
}

void mheap_pop(mheap *h, void** data, int* priority) {
    mheap_element* e = (mheap_element*)vector_get(h, 0);
    *data = e->data;
    *priority = e->value;

    h->length--;

    if (h->length > 0) {
        vector_swap(h, 0, h->length);
        downheap(h, 0);
    }
}

void destroy_mheap(mheap *h) {
    destroy_vector(h);
}

mheap* make_mheap() {
    return make_vector(sizeof(mheap_element));
}
