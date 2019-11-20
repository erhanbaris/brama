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

void* allocate(t_allocator_ptr allocator, size_t size) {
    const size_t currentAddress = (size_t)allocator->memory + allocator->offset;

    const size_t multiplier = (currentAddress / ALLOCATOR_ALIGNMENT) + 1;
    const size_t alignedAddress = multiplier * ALLOCATOR_ALIGNMENT;
    size_t padding = alignedAddress - currentAddress;

    //size_t padding = Utils::CalculatePaddingWithHeader(currentAddress, alignment, sizeof (AllocationHeader));
    size_t headerSize = sizeof (struct AllocationHeader);
    size_t neededSpace = headerSize;

    if (padding < neededSpace){
        // Header does not fit - Calculate next aligned address that header fits
        neededSpace -= padding;

        // How many alignments I need to fit the header
        if(neededSpace % ALLOCATOR_ALIGNMENT > 0){
            padding += ALLOCATOR_ALIGNMENT * (1+(neededSpace / ALLOCATOR_ALIGNMENT));
        }else {
            padding += ALLOCATOR_ALIGNMENT * (neededSpace / ALLOCATOR_ALIGNMENT);
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

#ifdef _DEBUG___
    printf("A\t@C-%p\t@R-%p\tS-%d\tO-%d\tP-%d\r\n", (void*) currentAddress, (void*) nextAddress, size, allocator->offset, padding);
#endif
    allocator->used = allocator->offset;
    allocator->peak = FAST_MAX(allocator->peak, allocator->used);

    return (void*) nextAddress;
}

void free_memory(t_allocator_ptr allocator, void *ptr) {
    // Move offset back to clear address
    const size_t currentAddress = (size_t) ptr;
    const size_t headerAddress = (size_t)allocator->memory - sizeof (struct AllocationHeader);
    struct AllocationHeader * allocationHeader = (struct AllocationHeader *) headerAddress;

    allocator->offset = currentAddress - allocationHeader->padding - (size_t) allocator->memory;
    allocator->used = allocator->offset;

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
