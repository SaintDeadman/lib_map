#include "map_simple.h"
#include <stdlib.h>


typedef struct map_simple {
    int mem_size;
    int key_size;
    int val_size;
    void* mem;
} map_simple_t;


/*===================================================
            create
======================================================*/
map_simple_ptr new_simple_map(size_t map_size, size_t key_size, size_t val_size){
    /*create struct*/
    map_simple_ptr  map = (map_simple_ptr)malloc(sizeof(map_simple_t));
    if(!map) return NULL;
    memset(map, 0x0, sizeof(map_simple_t));

    /*fill struct*/
    map->mem_size = map_size;
    map->key_size = key_size;
    map->val_size = val_size;
    map->mem = calloc(map_size, sizeof(uint32_t) + map->key_size + map->val_size);
    if(!map->mem) {
        free(map);
        return 0;
    }

    return map;
}

void free_simple_map(map_simple_ptr map) {
    free(map->mem);
    free(map);
}

/*===================================================
                    HASH FUNCTION
===================================================*/
static uint64_t hash_func(const uint8_t* key, size_t key_size, size_t map_size) {
    return (uint64_t)(*((uint64_t*)key) % map_size);
}


/*===================================================
                    API
======================================================*/
uint8_t insert_simple(map_simple_ptr obj, const uint8_t* key, const uint8_t* value) {
    uint64_t cur_index = hash_func(key, obj->key_size, obj->mem_size);
    uint64_t i;
    for(i = 0; i < obj->mem_size; i++, cur_index = (cur_index+1) % obj->mem_size)
    {
        /*get state*/
        uint64_t ent_size = sizeof(uint32_t) + obj->key_size + obj->val_size;
        uint8_t* ent_ptr = (obj->mem + cur_index*ent_size);
        if( *((uint32_t*)ent_ptr) == 0 ){
            /*need check key /filter blum*/
            memcpy(ent_ptr+4, key, obj->key_size);
            memcpy(ent_ptr+4+obj->key_size, value, obj->val_size);
            *((uint32_t*)ent_ptr) = 1;

            printf("index    = %u\n\r", cur_index);
            printf("ent addr = %#0X\n\r", ent_ptr);
            return 0;
        }
    }
    return 1;
}

void find_simple(map_simple_ptr obj){

}
void erase_simple(map_simple_ptr obj){
    
}