#include <stddef.h>
#include <stdlib.h>
#include "map.h"
#include "map_mock.h"
#include "map_simple.h"
#include "map_double.h"
#include "map_std.h"

/*
for adding a new implementation you need add new fn to this tables
*/

void* (*new_map_fn[])(const size_t size, const size_t key_size, const size_t val_size) = {
    [hash_simple...hash_dummy] = new_mock_map,
    [hash_simple] = (void* (*)(const size_t, const size_t,const size_t))new_simple_map,
    [hash_double] = (void* (*)(const size_t, const size_t,const size_t))new_double_map,
    [hash_std] = (void* (*)(const size_t, const size_t,const size_t))new_std_map,
};

void (*free_map_fn[])(void* obj) = {
    [hash_simple...hash_dummy] = free_mock_map,
    [hash_simple] = (void (*)(void *))free_simple_map,
    [hash_double] = (void (*)(void *))free_double_map,
    [hash_std] = (void (*)(void *))free_std_map
};

uint8_t (*insert_fn[])(const void* obj, const uint8_t* key, const uint8_t* value) = {
    [hash_simple...hash_dummy] = insert_mock,
    [hash_simple] = (uint8_t (*)(const void *, const uint8_t*, const uint8_t*))insert_simple,
    [hash_double] = (uint8_t (*)(const void *, const uint8_t*, const uint8_t*))insert_double,
    [hash_std] = (uint8_t (*)(const void *, const uint8_t*, const uint8_t*))insert_std
};

uint8_t* (*find_fn[])(const void* obj, const uint8_t* key) = {
    [hash_simple...hash_dummy] = find_mock,
    [hash_simple] = (uint8_t* (*)(void *,const uint8_t*))find_simple,
    [hash_double] = (uint8_t* (*)(void *,const uint8_t*))find_double,
    [hash_std] = (uint8_t* (*)(void *,const uint8_t*))find_std
};

uint8_t (*erase_fn[])(const void* obj, const uint8_t* key) = {
    [hash_simple...hash_dummy] = erase_mock,
    [hash_simple] = (uint8_t (*)(void *, const uint8_t* key))erase_simple,
    [hash_double] = (uint8_t (*)(void *, const uint8_t* key))erase_double,
    [hash_std] = (uint8_t (*)(void *, const uint8_t* key))erase_std
};

size_t (*count_fn[])(const void* obj) = {
    [hash_simple...hash_dummy] = count_mock,
    [hash_simple] = (size_t (*)(void *))count_simple,
    [hash_double] = (size_t (*)(void *))count_double,
    [hash_std] = (size_t (*)(void *))count_std
};

void (*stat_fn[])(const void* obj, map_counters_t* ) = {
    [hash_simple...hash_dummy] = get_stat_mock,
    [hash_simple] = (void (*)(void *,map_counters_t*))get_stat_simple,
    [hash_double] = (void (*)(void *,map_counters_t*))get_stat_double,
    [hash_std] = (void (*)(void *,map_counters_t*))get_stat_std
};

/*
    it just callers
*/
map_t* new_map(const hash_type_e type, const size_t size, const size_t key_size, const size_t val_size) {
    /* check params */
    if(type > hash_dummy) return NULL;
    if(!size || !key_size || !val_size) return NULL;

    /*create common type*/
    map_t* map = malloc(sizeof(map));
    map->type = type;
    map->obj = new_map_fn[type](size, key_size, val_size);
    if(!map->obj) {
        free(map);
        return NULL;
    }

    return map;
}

void free_map(map_t* map) {
    if(!map) return;
    free_map_fn[map->type](map->obj);
    free(map);
}

uint8_t insertm(const map_t* map, const uint8_t* key, const uint8_t* value) {
    if(!map) return MAP_ERROR;
    if(!map->obj) return MAP_ERROR;
    return insert_fn[map->type](map->obj, key, value);
}

uint8_t* findm(const map_t* map, const uint8_t* key) {
    if(!map) return NULL;
    if(!map->obj) return NULL;
    return find_fn[map->type](map->obj,key);
}

uint8_t erasem(const map_t* map, const uint8_t* key) {
    if(!map) return MAP_ERROR;
    if(!map->obj) return MAP_ERROR;
    return erase_fn[map->type](map->obj, key);
}

size_t countm(const map_t* map) {
    if(!map) return 0;
    return count_fn[map->type](map->obj);
}

void getstatm(const map_t* map, map_counters_t* statistic) {
    if(!map) return 0;
    stat_fn[map->type](map->obj, statistic);
}
