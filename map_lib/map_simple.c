#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "map_simple.h"
#include "misc/allocator.h"
#include "misc/sync.h"


#define ENTRY_FREE      0
#define ENTRY_OCCUPIED  1
#define ENTRY_DELETED   2


typedef struct {
    uint32_t state;
    uint32_t used;
} entry_info_t;

typedef struct {
    entry_info_t* info;
    uint8_t* key;
    uint8_t* value;
} entry_t;

typedef struct map_simple {
    size_t mem_size;
    size_t key_size;
    size_t val_size;
    void* mem;
    entry_info_t* sync_data;
    _Sync size_t count;
    _Sync size_t count_lock_error;
} map_simple_t;


#define MAP_SUCCESS (0)
#define MAP_DUPLICATE (2)
#define MAP_ERROR (1)

/*===================================================
                NEW/FREE SALE
======================================================*/
map_simple_ptr new_simple_map(const size_t map_size, const size_t key_size, const size_t val_size) {
    /*create struct*/
    map_simple_ptr  map = (map_simple_ptr)malloc(sizeof(map_simple_t));
    if(!map) return NULL;
    memset(map, 0x0, sizeof(map_simple_t));

    /*fill struct*/
    map->mem_size = map_size;
    map->key_size = key_size;
    map->val_size = val_size;
    map->count = 0;

    /*allocate mem to nodes*/
    size_t entry_size = map->key_size + map->val_size;
    map->mem = allocate_memory(entry_size*map_size);
    if(map->mem == 0) {
        free(map);
        return NULL;
    }

    /*allocate mem to ctrl structs*/
    map->sync_data = allocate_memory(map_size*sizeof(entry_info_t));
    if(map->sync_data == 0) {
        free(map->mem);
        free(map);
        return NULL;
    }
    return map;
}

void free_simple_map(map_simple_ptr map) {
    deallocate_memory(map->sync_data, map->mem_size*sizeof(entry_info_t));
    deallocate_memory(map->mem, map->mem_size*(map->key_size + map->val_size));
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

static void save_entry(map_simple_ptr map, entry_t entry, const uint8_t* key, const uint8_t* value) {
    memcpy(entry.key, key, map->key_size);
    memcpy(entry.value, value, map->val_size);
}

static void print_entry(map_simple_ptr map, uint64_t index) {
    entry_t entry = get_entry(map, index);
    printf("[%zu] key %p value %p", index, entry.key, entry.value);
}

static void print_key(map_simple_ptr map, uint64_t index) {
    size_t i;
    entry_t entry = get_entry(map, index);
    printf("[%zu] key = ", index);
    for(i=0;i<map->key_size;i++)
        printf("%02X", entry.key[i]);
    printf("\n");
}

/*===================================================
                    HASH FUNCTION
===================================================*/
static uint64_t hash_func(const uint8_t* key, size_t key_size) {
    (void)(key_size);
    return (uint64_t)(*((uint64_t*)key));
}


/*===================================================
                    API
======================================================*/
uint8_t insert_simple(map_simple_ptr obj, const uint8_t* key, const uint8_t* value) {
    uint64_t i;
    /*here we can using map of hashes*/
    uint64_t hash = hash_func(key, obj->key_size);
    uint64_t cur_index =  hash % obj->mem_size;
    for(i = 0; i < obj->mem_size; i++, cur_index = (cur_index+1) % obj->mem_size) { 
        /*get entry*/
        entry_t entry = get_entry(obj, cur_index);
        
        /*lock entry*/
        if( !_Sync_check_and_set(&entry.info->used, 0, 1)) {
            _Sync_increment(&obj->count_lock_error);
            continue;
        }

        /*check duplicate (we can use here filter of Bloom)*/
        if(entry.info->state == ENTRY_OCCUPIED) {
            if(!memcmp(entry.key, key, obj->key_size)) {
                entry.info->used = 0;
                return MAP_DUPLICATE;
            }
            entry.info->used = 0;
            continue; 
        }

        /*try to write*/
        if( entry.info->state == ENTRY_FREE || entry.info->state == ENTRY_DELETED ) {
            save_entry(obj, entry, key, value);
            _Sync_increment(&obj->count);  
            entry.info->state = ENTRY_OCCUPIED;
            entry.info->used = 0;

            return MAP_SUCCESS;
        }

    }
    return MAP_ERROR;
}

uint8_t* find_simple(map_simple_ptr obj, const uint8_t* key) {
    uint64_t hash = hash_func(key, obj->key_size);
    uint64_t cur_index =  hash % obj->mem_size;
    uint64_t i;
    
    for(i = 0; i < obj->mem_size; i++, cur_index = (cur_index+1) % obj->mem_size) {
        /*get entry*/
        entry_t entry = get_entry(obj, cur_index);

        /*check duplicate*/
        if(entry.info->state == ENTRY_DELETED) continue;
        if(entry.info->state == ENTRY_OCCUPIED)
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
        if(entry.info->state == ENTRY_OCCUPIED) {
            if(!memcmp(entry.key, key, obj->key_size)) {
                entry.info->state = ENTRY_DELETED;
                _Sync_decrement(&obj->count);
                return MAP_SUCCESS;
            } 
        }
    }

    return MAP_ERROR;
}

size_t count_simple(map_simple_ptr obj) {
    return obj->count;
}