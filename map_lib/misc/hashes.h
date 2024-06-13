#ifndef _HASHES_H_
#define _HASHES_H_
#include <stdint.h>
#include <stdlib.h>

uint64_t hash_func(const uint8_t* key, size_t key_size);
uint64_t fnv1a_64(const void *data, size_t len);


#endif //_HASHES_H_