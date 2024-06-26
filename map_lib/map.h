#ifndef _HASH_H_
#define _HASH_H_

#include <stdint.h>
#include <stddef.h>

typedef enum{
    hash_simple = 0,
    hash_double,
    hash_std,
    hash_dummy
} hash_type_e;

typedef struct {
    hash_type_e type;
    void* obj;
} map_t;

typedef struct {
    size_t insert;
    size_t lock_errors;
    size_t collisions;
} map_counters_t;

#define MAP_SUCCESS (0)
#define MAP_DUPLICATE (2)
#define MAP_ERROR (1)

map_t* new_map(const hash_type_e type, const size_t size, const size_t key_size, const size_t val_size);
void free_map(map_t* map);
uint8_t insertm(const map_t* map, const uint8_t* key, const uint8_t* value);
uint8_t* findm(const map_t* map, const uint8_t* key);
uint8_t erasem(const map_t* map, const uint8_t* key);
size_t countm(const map_t* map);
void getstatm(const map_t* map, map_counters_t* statistic);

/* TODO */
uint8_t* beginm(map_t* map);
uint8_t* endm(map_t* map);
uint8_t* nextm(map_t* map);


#endif //_HASH_H_