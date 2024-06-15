#include <stdlib.h>
#include "allocator.h" 
#include <errno.h>
#include <string.h>

#ifdef _WIN32
    #include <Windows.h>
#elif __linux__
    #include <unistd.h>
    #include <sys/mman.h>
#endif

size_t get_page_size() {
#ifdef _WIN32
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    return system_info.dwPageSize;
#elif __linux__
    return sysconf(_SC_PAGE_SIZE);
#endif
}

void* allocate_memory(size_t size) {
    size_t page_size = get_page_size();
    size_t rounded_size = (size + page_size - 1) & ~(page_size - 1);

#ifdef _WIN32
    void* ptr = VirtualAlloc(NULL, rounded_size, MEM_COMMIT, PAGE_READWRITE);
    if (ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return NULL;
    }
#elif __linux__
    void* ptr = mmap(NULL, rounded_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }
#endif

    return ptr;
}

void deallocate_memory(void* ptr, size_t size) {
#ifdef _WIN32
    // Освобождаем память с помощью VirtualFree
    VirtualFree(ptr, 0, MEM_RELEASE);
#elif __linux__
    size_t page_size = get_page_size();
    size_t rounded_size = (size + page_size - 1) & ~(page_size - 1);
    if(munmap(ptr, rounded_size) == -1)
    {
        printf("free ptr[%p] size[%zu] %s\n", ptr, size, strerror(errno));
    }
#endif
}