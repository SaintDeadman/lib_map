#include <stdint.h>
#include <stdlib.h>
#include "hashes.h"


uint64_t hash_func(const uint8_t* key, size_t key_size) {
    (void)(key_size);
    return (uint64_t)(*((uint64_t*)key));
}

uint64_t fnv1a_64(const void *data, size_t len) {
    
    const uint8_t *bytes = (const uint8_t *)data;
    const uint64_t FNV_prime = 1099511628211ULL;
    uint64_t hash = 14695981039346656037ULL;

    for (size_t i = 0; i < len; ++i) {
        hash ^= bytes[i];
        hash *= FNV_prime;
    }

    return hash;
}