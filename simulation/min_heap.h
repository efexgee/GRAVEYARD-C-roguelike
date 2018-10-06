typedef struct {
    int value;
    void *data;
} mheap_element;
typedef struct {
    mheap_element* e;
    int length;
    int capacity;
} mheap;

void mheap_push(mheap *h, void *data, int priority);
void mheap_pop(mheap *h, void** data, int* priority);
void mheap_peek(mheap *h, void** data, int* priority);
mheap* make_mheap();
void destroy_mheap(mheap *h);
