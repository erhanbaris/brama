#ifndef allocator_h
#define allocator_h

#include <stdint.h>

struct AllocationHeader {
    char padding;
};

typedef struct _t_allocator {
    void*  memory;
    size_t offset;
    size_t totalSize;
    size_t used;
    size_t peak;
} t_allocator;
typedef t_allocator* t_allocator_ptr;

t_allocator_ptr init_allocator(size_t totalSize);
void destroy_allocator(t_allocator_ptr allocator);
void* allocate(t_allocator_ptr allocator, size_t size);
void free_memory(t_allocator_ptr allocator, void *ptr);

#endif /* allocator_h */
