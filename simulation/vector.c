#include "vector.h"
#include <stdio.h>

vector* make_vector(size_t element_size) {
    vector *v = malloc(sizeof(vector));
    v->e = NULL;
    v->length = 0;
    v->capacity = 0;
    v->element_size = element_size;
    return v;
}

void destroy_vector(vector *v) {
    free((void*)v->e);
    free((void*)v);
}

void vector_push(vector *v, void* element) {
    if (v->capacity == v->length) {
        if (v->e == NULL) {
            v->e = malloc(100*v->element_size);
            v->capacity = 100;
        } else {
            v->e = realloc(v->e, v->capacity*2 * v->element_size);
            if (v->e == NULL) exit(1);
            v->capacity *= 2;
        }
    }

    memcpy(v->e + (v->length) * (v->element_size), element, v->element_size);
    v->length++;
}

void* vector_peek(vector *v) {
    return (void*)(v->e + (v->length-1)*v->element_size);
}

void vector_pop(vector *v, void **data) {
    *data = v->e + (v->length-1)*v->element_size;
    v->length--;

    if (v->length+1 < v->capacity/2) {
        v->e = realloc(v->e, v->capacity/2);
        if (v->e == NULL) exit(1);
        v->capacity /= 2;
    }
}

void* vector_get(vector *v, int i) {
    return (void*)(v->e + i*v->element_size);
}

void vector_set(vector *v, int i, void* data) {
    memcpy((void*)(v->e + i*v->element_size), data, v->element_size);
}

void vector_swap(vector *v, int i, int j) {
    if (i == j) return;
    void *a, *b;
    a = vector_get(v, i);
    b = malloc(v->element_size);
    b = memcpy(b, vector_get(v, j), v->element_size);
    vector_set(v, j, a);
    vector_set(v, i, b);
    free(b);
}
