#include "hash.h"
#include "map_mock.h"
#include "map_simple.h"

void hash_create(hash_t* obj, hash_type_e type)
{

}





uint8_t (*insert_fn[])(void* obj, const uint8_t* key, const uint8_t* value) = {
    [hash_simple...hash_dummy] = insert_mock,
    [hash_simple] = (uint8_t (*)(void *, const uint8_t*, const uint8_t*))insert_simple
};

void (*find_fn[])(void* obj) = {
    [hash_simple...hash_dummy] = find_mock,
    [hash_simple] = (void (*)(void *))find_simple
};

void (*erase_fn[])(void* obj) = {
    [hash_simple...hash_dummy] = erase_mock,
    [hash_simple] = (void (*)(void *))erase_simple
};


uint8_t insertm(hash_t* map, const uint8_t* key, const uint8_t* value) {
    if(!map) return;
    if(!map->obj) return;

    return insert_fn[map->type](map->obj, key, value);
}
void findm(hash_t* map) {
    find_fn[map->type](map->obj);
}
void erasem(hash_t* map) {
    erase_fn[map->type](map->obj);
}
