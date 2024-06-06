#include "hash.h"
#include "map_mock.h"
#include "map_simple.h"

void hash_create(hash_t* obj, hash_type_e type)
{

}




/*
for adding a new implementation you need add new fn to this tables
*/
uint8_t (*insert_fn[])(void* obj, const uint8_t* key, const uint8_t* value) = {
    [hash_simple...hash_dummy] = insert_mock,
    [hash_simple] = (uint8_t (*)(void *, const uint8_t*, const uint8_t*))insert_simple
};

uint8_t* (*find_fn[])(void* obj, const uint8_t* key) = {
    [hash_simple...hash_dummy] = find_mock,
    [hash_simple] = (uint8_t* (*)(void *,const uint8_t*))find_simple
};

uint8_t (*erase_fn[])(void* obj, const uint8_t* key) = {
    [hash_simple...hash_dummy] = erase_mock,
    [hash_simple] = (uint8_t (*)(void *, const uint8_t* key))erase_simple
};



/*
    it just callers
*/
uint8_t insertm(hash_t* map, const uint8_t* key, const uint8_t* value) {
    if(!map) return;
    if(!map->obj) return;

    return insert_fn[map->type](map->obj, key, value);
}

uint8_t* findm(hash_t* map, const uint8_t* key) {
    if(!map) return;
    if(!map->obj) return;
    return find_fn[map->type](map->obj,key);
}

uint8_t erasem(hash_t* map, const uint8_t* key) {
    if(!map) return;
    if(!map->obj) return;
    return erase_fn[map->type](map->obj, key);
}
