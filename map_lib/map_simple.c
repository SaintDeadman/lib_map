#include "map_simple.h"
#include "../misc/sync.h"
#include <stdlib.h>


#define ENTRY_FREE      0
#define ENTRY_OCCUPIED  1
#define ENTRY_DELETED   2
#define ENTRY_BUSY      3

typedef struct map_simple {
    int mem_size;
    int key_size;
    int val_size;
    void* mem;
    uint32_t* sync_data;
    _Sync int data;
} map_simple_t;

typedef struct {
    uint32_t* info;
    uint8_t* key;
    uint8_t* value;
} entry_t;


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
    map->mem = calloc(map_size, map->key_size + map->val_size);
    if(!map->mem) {
        free(map);
        return 0;
    }
    map->sync_data = calloc(map_size, sizeof(uint32_t));
    if(!map->sync_data) {
        free(map->mem);
        free(map);
        return 0;
    }

    return map;
}

void free_simple_map(map_simple_ptr map) {
    free(map->mem);
    free(map);
}


/*=============================================================*/
static entry_t get_entry(map_simple_ptr map, uint64_t index) {
    uint64_t ent_size = map->key_size+map->val_size;
    entry_t ent = {.info = &map->sync_data[index],
                 .key = (uint8_t*)(map->mem + index*ent_size),
                 .value = (uint8_t*)(map->mem + index*ent_size + map->key_size) };
    return ent;
}

static void save_entry(map_simple_ptr map, entry_t entry, uint8_t* key, uint8_t* value) {
    memcpy(entry.key, key, map->key_size);
    memcpy(entry.value, value, map->val_size);
}

static void print_entry(map_simple_ptr map, uint64_t index) {
    entry_t entry = get_entry(map, index);
    printf("[%d] key %#016x value %#016x\n", index, entry.key, entry.value);
}
static void print_key(map_simple_ptr map, uint64_t index) {
    int i;
    entry_t entry = get_entry(map, index);
    printf("[%d] key = ", index);
    for(i=0;i<map->key_size;i++)
        printf("%02X", entry.key[i]);
    printf("\n");
}

/*===================================================
                    HASH FUNCTION
===================================================*/
static uint64_t hash_func(const uint8_t* key, size_t key_size) {
    return (uint64_t)(*((uint64_t*)key));
}


/*===================================================
                    API
======================================================*/
uint8_t insert_simple(map_simple_ptr obj, const uint8_t* key, const uint8_t* value) {
    uint64_t hash = hash_func(key, obj->key_size);
    uint64_t cur_index =  hash % obj->mem_size;
    uint64_t i;
    
    for(i = 0; i < obj->mem_size; i++)
    { 
        /*get index*/
        cur_index = (cur_index+1) % obj->mem_size;

        /*get entry*/
        entry_t entry = get_entry(obj, cur_index);

        /*check duplicate*/
        /*need filter of Blum*/
        if(*entry.info == ENTRY_OCCUPIED)
            if(!memcmp(entry.key, key, obj->key_size)) return 1; 

        /*try to write*/
        if( *entry.info == ENTRY_FREE || *entry.info == ENTRY_DELETED ){
            save_entry(obj, entry, key, value);
            print_entry(obj, cur_index);
            print_key(obj, cur_index);      
            *entry.info = ENTRY_OCCUPIED;
            return 0;
        }
    }
    return 1;
}

uint8_t* find_simple(map_simple_ptr obj, const uint8_t* key) {
    uint64_t hash = hash_func(key, obj->key_size);
    uint64_t cur_index =  hash % obj->mem_size;
    uint64_t i;
    
    for(i = 0; i < obj->mem_size; i++, cur_index = (cur_index+1) % obj->mem_size)
    {
        /*get entry*/
        entry_t entry = get_entry(obj, cur_index);

        /*check duplicate*/
        if(*entry.info == ENTRY_DELETED) continue;
        if(*entry.info == ENTRY_OCCUPIED)
            if(!memcmp(entry.key, key, obj->key_size)) return entry.value; 
    }
    return NULL;
}

uint8_t erase_simple(map_simple_ptr obj, const uint8_t* key) {
    uint64_t hash = hash_func(key, obj->key_size);
    uint64_t cur_index =  hash % obj->mem_size;
    uint64_t i;
    
    for(i = 0; i < obj->mem_size; i++, cur_index = (cur_index+1) % obj->mem_size) {
        /*get entry*/
        entry_t entry = get_entry(obj, cur_index);
        /*check duplicate*/
        if(*entry.info == ENTRY_OCCUPIED) {
            if(!memcmp(entry.key, key, obj->key_size)) {
                *entry.info = ENTRY_DELETED;
                return 0;
            } 
        }
    }

    return 1;
}