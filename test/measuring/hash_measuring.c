#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#include "misc/hashes.h"


static void generate_random_key(unsigned int seed, unsigned char *buffer, size_t length) {
    srand((unsigned int)clock()+seed);
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = (unsigned char)(rand() % 256); // Генерация случайного байта
    }
}
static void generate_random_keys_pack(unsigned int seed, unsigned char *key, size_t key_size, size_t pack) {
    for (size_t i = 0; i < pack; ++i)
        generate_random_key(seed, key+i*key_size, key_size);
}

#define PACK_COUNT ((uint64_t)1e3)
#define KEY_SIZE (64)
#define KEYS_PER_PACK (1024)


static void avr_measuring(char* name, uint64_t(*hash_fn)(uint8_t*,size_t)) {
    int64_t num_key, it = 0;
    uint8_t keys [KEYS_PER_PACK][KEY_SIZE];
    clock_t clock_start = 0;
    clock_t clock_end = 0;
    clock_t clock_sum = 0;
    for(it =0; it < PACK_COUNT; it++) {
        generate_random_keys_pack(0, keys, KEY_SIZE,KEYS_PER_PACK);
        
        clock_start = clock();
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            hash_fn(keys[num_key], KEY_SIZE);
        }
        clock_end = clock();
        clock_sum += (clock_end - clock_start);
    }
    printf("Hash funtion: %s\n", name);
    printf("avr pack time [pack/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT);
    printf("avr hash time [hash/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT/(double)KEYS_PER_PACK);
}


static void measure_distribution(uint64_t (*hash_fn)(uint8_t *, size_t)) {
    int64_t num_key, it = 0;
    uint8_t keys[KEYS_PER_PACK][KEY_SIZE];

    uint64_t hash_count[256] = {0};
    uint64_t total_keys = PACK_COUNT * KEYS_PER_PACK;

    for (it = 0; it < PACK_COUNT; it++) {
        generate_random_keys_pack(0, keys, KEY_SIZE, KEYS_PER_PACK);

        for (num_key = 0; num_key < KEYS_PER_PACK; num_key++) {
            uint64_t hash_value = hash_fn(keys[num_key], KEY_SIZE);
            hash_count[hash_value % 256]++;
        }
    }

    double expected_count = (double)total_keys / 256.0;
    double chi_square_sum = 0.0;

    for (int i = 0; i < 256; i++) {
        double deviation = (double)hash_count[i] - expected_count;
        chi_square_sum += (deviation * deviation) / expected_count;
    }

    // Нормированное квадратичное отклонение
    double normalized_std_dev = sqrt(chi_square_sum / 256.0) / expected_count;

    printf("Normalized standard deviation = %.8f\n", normalized_std_dev);
}

int main ()
{
    avr_measuring("mod", mod_hash);
    measure_distribution(mod_hash);
    avr_measuring("fnv1a", fnv1a_hash);
    measure_distribution(fnv1a_hash);
    avr_measuring("rot13", rot13_hash);
    measure_distribution(rot13_hash);
    avr_measuring("xxhash64", xxhash64);
    measure_distribution(xxhash64);
    return 0;    
}