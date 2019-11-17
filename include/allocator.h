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
void* allocate(t_allocator_ptr allocator, size_t size, const size_t alignment);
void free_memory(t_allocator_ptr allocator, void *ptr);


typedef struct Stack Stack;
struct Stack {
    unsigned char *buf;
    size_t buf_len;
    size_t offset;
};

typedef struct Stack_Allocation_Header Stack_Allocation_Header;
struct Stack_Allocation_Header {
    uint8_t padding;
};

Stack* stack_init(size_t backing_buffer_length);
void stack_free_all(Stack *s);
void *stack_alloc(Stack *s, size_t size);
void stack_free(Stack *s, void *ptr);

#endif /* allocator_h */
