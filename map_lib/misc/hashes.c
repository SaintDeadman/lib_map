#include <stdint.h>
#include <stdlib.h>
#include "hashes.h"


uint64_t mod_hash(const uint8_t* key, size_t key_size) {
    (void)(key_size);
    return (uint64_t)(*((uint64_t*)key));
}

uint64_t fnv1a_hash(const void *data, size_t len) {
    
    const uint8_t *bytes = (const uint8_t *)data;
    const uint64_t FNV_prime = 1099511628211ULL;
    uint64_t hash = 14695981039346656037ULL;

    for (size_t i = 0; i < len; ++i) {
        hash ^= bytes[i];
        hash *= FNV_prime;
    }

    return hash;
}

uint64_t rot13_hash(const uint8_t *data, size_t len) {
    uint64_t hash = 0;
    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        // Apply ROT13 to each byte
        byte = (byte + 13) % 256;
        // Combine the byte into the hash value
        hash = (hash << 5) + byte;
    }
    return hash;
}


#define XXH_PRIME64_1  11400714785074694791ULL
#define XXH_PRIME64_2  14029467366897019727ULL
#define XXH_PRIME64_3  1609587929392839161ULL
#define XXH_PRIME64_4  9650029242287828579ULL
#define XXH_PRIME64_5  2870177450012600261ULL
static uint64_t xxh64_round(uint64_t acc, uint64_t input) {
    acc += input * XXH_PRIME64_2;
    acc = (acc << 31) | (acc >> 33);
    acc *= XXH_PRIME64_1;
    return acc;
}

static uint64_t xxh64_avalanche(uint64_t h64) {
    h64 ^= h64 >> 33;
    h64 *= XXH_PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= XXH_PRIME64_3;
    h64 ^= h64 >> 32;
    return h64;
}

uint64_t xxhash64(const void* input, size_t length) {
    const uint8_t* data = (const uint8_t*)input;
    const uint8_t* end = data + length;
    uint64_t h64;

    if (length >= 32) {
        const uint8_t* limit = end - 32;
        uint64_t v1 = 0 + XXH_PRIME64_1 + XXH_PRIME64_2;
        uint64_t v2 = 0 + XXH_PRIME64_2;
        uint64_t v3 = 0 + 0;
        uint64_t v4 = 0 - XXH_PRIME64_1;

        do {
            v1 = xxh64_round(v1, *(uint64_t*)data); data += 8;
            v2 = xxh64_round(v2, *(uint64_t*)data); data += 8;
            v3 = xxh64_round(v3, *(uint64_t*)data); data += 8;
            v4 = xxh64_round(v4, *(uint64_t*)data); data += 8;
        } while (data <= limit);

        h64 = (v1 << 1) | (v1 >> 63);
        h64 += (v2 << 7) | (v2 >> 57);
        h64 += (v3 << 12) | (v3 >> 52);
        h64 += (v4 << 18) | (v4 >> 46);

        v1 *= XXH_PRIME64_2; v1 = (v1 << 31) | (v1 >> 33); v1 *= XXH_PRIME64_1; h64 ^= v1; h64 = h64 * XXH_PRIME64_1 + XXH_PRIME64_4;
        v2 *= XXH_PRIME64_2; v2 = (v2 << 31) | (v2 >> 33); v2 *= XXH_PRIME64_1; h64 ^= v2; h64 = h64 * XXH_PRIME64_1 + XXH_PRIME64_4;
        v3 *= XXH_PRIME64_2; v3 = (v3 << 31) | (v3 >> 33); v3 *= XXH_PRIME64_1; h64 ^= v3; h64 = h64 * XXH_PRIME64_1 + XXH_PRIME64_4;
        v4 *= XXH_PRIME64_2; v4 = (v4 << 31) | (v4 >> 33); v4 *= XXH_PRIME64_1; h64 ^= v4; h64 = h64 * XXH_PRIME64_1 + XXH_PRIME64_4;
    } else {
        h64 = 0 + XXH_PRIME64_5;
    }

    h64 += (uint64_t)length;

    while (data + 8 <= end) {
        uint64_t k1 = *(uint64_t*)data;
        k1 *= XXH_PRIME64_2; k1 = (k1 << 31) | (k1 >> 33); k1 *= XXH_PRIME64_1; h64 ^= k1;
        h64 = (h64 << 27) | (h64 >> 37);
        h64 = h64 * XXH_PRIME64_1 + XXH_PRIME64_4;
        data += 8;
    }

    while (data + 4 <= end) {
        h64 ^= (*(uint32_t*)data) * XXH_PRIME64_1;
        h64 = (h64 << 23) | (h64 >> 41);
        h64 *= XXH_PRIME64_2;
        data += 4;
    }

    while (data < end) {
        h64 ^= (*data) * XXH_PRIME64_5;
        h64 = (h64 << 11) | (h64 >> 53);
        h64 *= XXH_PRIME64_1;
        data++;
    }

    return xxh64_avalanche(h64);
}

#if 0
#include <immintrin.h>
static __m256i xxh64_round_avx2(__m256i acc, __m256i input) {
    __m256i prime2 = _mm256_set1_epi64x(XXH_PRIME64_2);
    __m256i prime1 = _mm256_set1_epi64x(XXH_PRIME64_1);

    acc = _mm256_add_epi64(acc, _mm256_mullo_epi64(input, prime2));
    acc = _mm256_or_si256(_mm256_slli_epi64(acc, 31), _mm256_srli_epi64(acc, 33));
    acc = _mm256_mullo_epi64(acc, prime1);
    return acc;
}

uint64_t xxhash64_avx2(const void* input, size_t length, uint64_t seed) {
    const uint8_t* data = (const uint8_t*)input;
    const uint8_t* end = data + length;
    uint64_t h64;

    if (length >= 32) {
        const uint8_t* limit = end - 32;
        __m256i v1 = _mm256_set1_epi64x(seed + XXH_PRIME64_1 + XXH_PRIME64_2);
        __m256i v2 = _mm256_set1_epi64x(seed + XXH_PRIME64_2);
        __m256i v3 = _mm256_set1_epi64x(seed);
        __m256i v4 = _mm256_set1_epi64x(seed - XXH_PRIME64_1);

        do {
            __m256i input1 = _mm256_loadu_si256((const __m256i*)data); data += 32;
            v1 = xxh64_round_avx2(v1, input1);
            v2 = xxh64_round_avx2(v2, input1);
            v3 = xxh64_round_avx2(v3, input1);
            v4 = xxh64_round_avx2(v4, input1);
        } while (data <= limit);

        h64 = _mm256_extract_epi64(v1, 0) + _mm256_extract_epi64(v2, 1) + _mm256_extract_epi64(v3, 2) + _mm256_extract_epi64(v4, 3);

        h64 = xxh64_avalanche(h64);
    } else {
        h64 = seed + XXH_PRIME64_5;
    }

    h64 += (uint64_t)length;

    while (data + 8 <= end) {
        uint64_t k1 = *(uint64_t*)data;
        k1 *= XXH_PRIME64_2; k1 = (k1 << 31) | (k1 >> 33); k1 *= XXH_PRIME64_1; h64 ^= k1;
        h64 = (h64 << 27) | (h64 >> 37);
        h64 = h64 * XXH_PRIME64_1 + XXH_PRIME64_4;
        data += 8;
    }

    while (data + 4 <= end) {
        h64 ^= (*(uint32_t*)data) * XXH_PRIME64_1;
        h64 = (h64 << 23) | (h64 >> 41);
        h64 *= XXH_PRIME64_2;
        data += 4;
    }

    while (data < end) {
        h64 ^= (*data) * XXH_PRIME64_5;
        h64 = (h64 << 11) | (h64 >> 53);
        h64 *= XXH_PRIME64_1;
        data++;
    }

    return xxh64_avalanche(h64);
}

#endif