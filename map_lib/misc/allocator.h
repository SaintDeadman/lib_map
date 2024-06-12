#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <stdlib.h>

size_t get_page_size();
void* allocate_memory(size_t size);
void deallocate_memory(void* ptr, size_t size);

#endif //_ALLOCATOR_H_