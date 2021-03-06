#include <stdlib.h> 
#include <stdio.h>
#include "allocator.h"
#include "macros.h"

t_allocator_ptr init_allocator(size_t totalSize) {
    t_allocator_ptr allocator = (t_allocator_ptr)malloc(sizeof(t_allocator));
    allocator->memory         = malloc(totalSize);
    allocator->used           = 0;
    allocator->offset         = 0;
    allocator->totalSize      = totalSize;
    allocator->peak           = 0;
    
    return allocator;
}

void* stack_malloc(void* allocator, size_t size, char* file_name, int line_number) {
    const size_t currentAddress = (size_t)((t_allocator_ptr)allocator)->memory + ((t_allocator_ptr)allocator)->offset;

    const size_t multiplier = (currentAddress >> 3) + 1; // For performance improvement: (currentAddress / 8) + 1
    const size_t alignedAddress = multiplier * 8;
    size_t padding = alignedAddress - currentAddress;

    //size_t padding = Utils::CalculatePaddingWithHeader(currentAddress, alignment, sizeof (AllocationHeader));
    size_t headerSize = sizeof (struct AllocationHeader);
    size_t neededSpace = headerSize;

    if (padding < neededSpace){
        // Header does not fit - Calculate next aligned address that header fits
        neededSpace -= padding;

        // How many alignments I need to fit the header
        if((neededSpace & 7) > 0){ // For performance improvement: neededSpace % 8
            padding += 8 * (1+(neededSpace >> 3)); // For performance improvement: (1+(neededSpace / 8))
        }else {
            padding += 8 * (neededSpace >> 3); // For performance improvement: (currentAddress / 8)
        }
    }

    if (((t_allocator_ptr)allocator)->offset + padding + size > ((t_allocator_ptr)allocator)->totalSize) {
        return NULL;
    }
    ((t_allocator_ptr)allocator)->offset += padding;

    const size_t nextAddress  = currentAddress + padding;
    const size_t headerAddress = nextAddress - sizeof (struct AllocationHeader);
    struct AllocationHeader allocationHeader;
    allocationHeader.padding = padding;
    
    struct AllocationHeader * headerPtr = (struct AllocationHeader*) headerAddress;
    headerPtr = &allocationHeader;
    
    ((t_allocator_ptr)allocator)->offset += size;

#ifdef _DEBUG___
    printf("A\t@C-%p\t@R-%p\tS-%d\tO-%d\tP-%d\r\n", (void*) currentAddress, (void*) nextAddress, size, allocator->offset, padding);
#endif
    ((t_allocator_ptr)allocator)->used = ((t_allocator_ptr)allocator)->offset;
    ((t_allocator_ptr)allocator)->peak = FAST_MAX(((t_allocator_ptr)allocator)->peak, ((t_allocator_ptr)allocator)->used);

    return (void*) nextAddress;
}

void* stack_calloc(void* user_data, size_t count, size_t size, char* file_name, int line_number) {
    return stack_malloc(user_data, count * size, file_name, line_number);
}

void stack_free(void* allocator, void *ptr, char* file_name, int line_number) {
    // Move offset back to clear address
    const size_t currentAddress = (size_t) ptr;
    const size_t headerAddress  = (size_t)((t_allocator_ptr)allocator)->memory - sizeof (struct AllocationHeader);
    struct AllocationHeader * allocationHeader = (struct AllocationHeader *) headerAddress;

    ((t_allocator_ptr)allocator)->offset = currentAddress - allocationHeader->padding - (size_t) ((t_allocator_ptr)allocator)->memory;
    ((t_allocator_ptr)allocator)->used   = ((t_allocator_ptr)allocator)->offset;

    #ifdef _DEBUG___
        printf("F\t@C-%p\t@F-%p\t\tO-%d\r\n", (void*) currentAddress, (void*) ((char*) allocator->memory + allocator->offset), allocator->offset); 
    #endif
}

void destroy_allocator(t_allocator_ptr allocator) {
    if (NULL == allocator)
        return;
    
    free(allocator->memory);
    free(allocator);
}
