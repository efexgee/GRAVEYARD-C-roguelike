#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *e;
    int length;
    int capacity;
    size_t element_size;
} vector;

vector* make_vector(size_t element_size);
void destroy_vector(vector *v);
void vector_push(vector *v, void* element);
void* vector_peek(vector *v);
void vector_pop(vector *v, void** data);
void* vector_get(vector *v, int i);
void vector_set(vector *v, int i, void* data);
void vector_swap(vector *v, int i, int j);

#endif
