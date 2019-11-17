#include <stdio.h>
#include "allocator.h"
#include "macros.h"

t_allocator_ptr init_allocator(size_t totalSize) {
    t_allocator_ptr allocator = malloc(sizeof(t_allocator));
    allocator->memory         = malloc(totalSize);
    allocator->used           = 0;
    allocator->offset         = 0;
    allocator->totalSize      = totalSize;
    allocator->peak           = 0;
    
    return allocator;
}

void* allocate(t_allocator_ptr allocator, size_t size, const size_t alignment) {
    const size_t currentAddress = (size_t)allocator->memory + allocator->offset;

    const size_t multiplier = (currentAddress / alignment) + 1;
    const size_t alignedAddress = multiplier * alignment;
    size_t padding = alignedAddress - currentAddress;

    //size_t padding = Utils::CalculatePaddingWithHeader(currentAddress, alignment, sizeof (AllocationHeader));
    size_t headerSize = sizeof (struct AllocationHeader);
    size_t neededSpace = headerSize;

    if (padding < neededSpace){
        // Header does not fit - Calculate next aligned address that header fits
        neededSpace -= padding;

        // How many alignments I need to fit the header
        if(neededSpace % alignment > 0){
            padding += alignment * (1+(neededSpace / alignment));
        }else {
            padding += alignment * (neededSpace / alignment);
        }
    }

    if (allocator->offset + padding + size > allocator->totalSize) {
        return NULL;
    }
    allocator->offset += padding;

    const size_t nextAddress = currentAddress + padding;
    const size_t headerAddress = nextAddress - sizeof (struct AllocationHeader);
    struct AllocationHeader allocationHeader;
    allocationHeader.padding = padding;
    
    struct AllocationHeader * headerPtr = (struct AllocationHeader*) headerAddress;
    headerPtr = &allocationHeader;
    
    allocator->offset += size;

#ifdef _DEBUG
    std::cout << "A" << "\t@C " << (void*) currentAddress << "\t@R " << (void*) nextAddress << "\tO " << m_offset << "\tP " << padding << std::endl;
#endif
    allocator->used = allocator->offset;
    allocator->peak = FAST_MAX(allocator->peak, allocator->used);

    return (void*) nextAddress;
}

void free_memory(t_allocator_ptr allocator, void *ptr) {
    // Move offset back to clear address
    const size_t currentAddress = (size_t) ptr;
    const size_t headerAddress = allocator->memory - sizeof (struct AllocationHeader);
    struct AllocationHeader * allocationHeader = (struct AllocationHeader *) headerAddress;

    allocator->offset = currentAddress - allocationHeader->padding - (size_t) allocator->memory;
    allocator->used = allocator->offset;

    #ifdef _DEBUG
        std::cout << "F" << "\t@C " << (void*) currentAddress << "\t@F " << (void*) ((char*) m_start_ptr + m_offset) << "\tO " << m_offset << std::endl;
    #endif
}

void destroy_allocator(t_allocator_ptr allocator) {
    if (NULL == allocator)
        return;
    
    free(allocator->memory);
    free(allocator);
}



Stack* stack_init(size_t backing_buffer_length) {
    Stack* stack = malloc(sizeof(Stack));
    void* buffer = malloc(backing_buffer_length);
    stack->buf = (unsigned char *)buffer;
    stack->buf_len = backing_buffer_length;
    stack->offset = 0;
    return stack;
}

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size) {
    uintptr_t p, a, modulo, padding, needed_space;

    p = ptr;
    a = alignment;
    modulo = p & (a-1); // (p % a) as it assumes alignment is a power of two

    padding = 0;
    needed_space = 0;

    if (modulo != 0) { // Same logic as 'align_forward'
        padding = a - modulo;
    }

    needed_space = (uintptr_t)header_size;

    if (padding < needed_space) {
        needed_space -= padding;

        if ((needed_space & (a-1)) != 0) {
            padding += a * (1+(needed_space/a));
        } else {
            padding += a * (needed_space/a);
        }
    }

    return (size_t)padding;
}

void *stack_alloc_align(Stack *s, size_t size, size_t alignment) {
    uintptr_t curr_addr, next_addr;
    size_t padding;
    Stack_Allocation_Header *header;


    //assert(is_power_of_two(alignment));

    if (alignment > 128) {
        // As the padding is 8 bits (1 byte), the largest alignment that can
        // be used is 128 bytes
        alignment = 128;
    }

    curr_addr = (uintptr_t)s->buf + (uintptr_t)s->offset;
    padding = calc_padding_with_header(curr_addr, (uintptr_t)alignment, sizeof(Stack_Allocation_Header));
    if (s->offset + padding + size > s->buf_len) {
        // Stack allocator is out of memory
        return NULL;
    }
    s->offset += padding;

    next_addr = curr_addr + (uintptr_t)padding;
    header = (Stack_Allocation_Header *)(next_addr - sizeof(Stack_Allocation_Header));
    header->padding = (uint8_t)padding;

    s->offset += size;

    return memset((void *)next_addr, 0, size);
}

#define DEFAULT_ALIGNMENT 8
// Because C does not have default parameters
void *stack_alloc(Stack *s, size_t size) {
    return stack_alloc_align(s, size, DEFAULT_ALIGNMENT);
}

void stack_free(Stack *s, void *ptr) {
    if (ptr != NULL) {
        uintptr_t start, end, curr_addr;
        Stack_Allocation_Header *header;
        size_t prev_offset;

        start = (uintptr_t)s->buf;
        end = start + (uintptr_t)s->buf_len;
        curr_addr = (uintptr_t)ptr;

        if (!(start <= curr_addr && curr_addr < end)) {
            return;
        }

        if (curr_addr >= start+(uintptr_t)s->offset) {
            // Allow double frees
            return;
        }

        header = (Stack_Allocation_Header *)(curr_addr - sizeof(Stack_Allocation_Header));
        prev_offset = (size_t)(curr_addr - (uintptr_t)header->padding - start);

        s->offset = prev_offset;
    }
}

void *stack_resize_align(Stack *s, void *ptr, size_t old_size, size_t new_size, size_t alignment) {
    if (ptr == NULL) {
        return stack_alloc_align(s, new_size, alignment);
    } else if (new_size == 0) {
        stack_free(s, ptr);
        return NULL;
    } else {
        uintptr_t start, end, curr_addr;
        size_t min_size = old_size < new_size ? old_size : new_size;
        void *new_ptr;

        start = (uintptr_t)s->buf;
        end = start + (uintptr_t)s->buf_len;
        curr_addr = (uintptr_t)ptr;
        if (!(start <= curr_addr && curr_addr < end)) {
            return NULL;
        }

        if (curr_addr >= start + (uintptr_t)s->offset) {
            // Treat as a double free
            return NULL;
        }

        if (old_size == new_size) {
            return ptr;
        }

        new_ptr = stack_alloc_align(s, new_size, alignment);
        memmove(new_ptr, ptr, min_size);
        return new_ptr;
    }
}

void *stack_resize(Stack *s, void *ptr, size_t old_size, size_t new_size) {
    return stack_resize_align(s, ptr, old_size, new_size, DEFAULT_ALIGNMENT);
}

void stack_free_all(Stack *s) {
    s->offset = 0;
}
