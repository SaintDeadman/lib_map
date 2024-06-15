#ifndef _HASHES_H_
#define _HASHES_H_
#include <stdint.h>
#include <stdlib.h>

uint64_t mod_hash(const uint8_t* key, size_t key_size);
uint64_t fnv1a_hash(const void *data, size_t len);
uint64_t rot13_hash(const uint8_t *data, size_t len);
uint64_t xxhash64(const void* input, size_t length);


#endif //_HASHES_H_