#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"
#include "map_double.h"
#include "misc/allocator.h"
#include "misc/sync.h"
#include "misc/hashes.h"
#include "misc/misc.h"

typedef struct {
    uint32_t state;
    uint32_t used;
} entry_info_t;

typedef struct {
    entry_info_t* info;
    uint8_t* hash;
    uint8_t* key;
    uint8_t* value;
} entry_t;

typedef struct map_double {
    size_t mem_size;
    size_t key_size;
    size_t val_size;
    size_t ent_size;
    void* mem;
    _Sync size_t count;
    _Sync size_t count_inserts;
    _Sync size_t count_lock_error;
    _Sync size_t count_collision;
} map_double_t;

#define ENTRY_FREE      0
#define ENTRY_OCCUPIED  1
#define ENTRY_DELETED   2

map_double_ptr new_double_map(const size_t map_size, const size_t key_size, const size_t val_size)  {
    /*create struct*/
    map_double_ptr  map = (map_double_ptr)malloc(sizeof(map_double_t));
    if(!map) return NULL;
    memset(map, 0x0, sizeof(map_double_t));

    /*fill struct*/
    map->mem_size = 1 << (bitScanReverse(map_size)+1);
    map->key_size = key_size;
    map->val_size = val_size;
    map->ent_size = sizeof(entry_info_t) + sizeof(uint64_t) + map->key_size + map->val_size;

    /*allocate mem to nodes*/
    map->mem = allocate_memory(map->ent_size*map->mem_size);
    if(map->mem == 0) {
        free(map);
        return NULL;
    }
    return map;
}

void free_double_map(map_double_ptr map) {
    deallocate_memory(map->mem, map->mem_size*map->ent_size);
    free(map);
}

/*=============================================================*/
static inline void get_entry(const map_double_ptr map, entry_t* entry, const const uint64_t index) {
    const size_t entry_offset = index * map->ent_size;
    const uint8_t *mem_ptr = (uint8_t *)(map->mem + entry_offset);
    __builtin_prefetch(mem_ptr);
    entry->info = mem_ptr;
    entry->hash = mem_ptr+sizeof(entry_info_t);
    entry->key = entry->hash + sizeof(uint64_t);
    entry->value = entry->key + map->key_size;
}

static inline void copy_entry(map_double_ptr map, entry_t* dest_entry, const entry_t* src_entry) {
    memcpy(dest_entry->hash, src_entry->hash,sizeof(uint64_t));
    memcpy(dest_entry->key, src_entry->key, map->key_size);
    memcpy(dest_entry->value, src_entry->value, map->val_size);
}

static inline bool is_duplicate(map_double_ptr map, entry_t* first, const entry_t* second) {
    /*check hashes*/
    if(*((uint64_t*)first->hash) != *((uint64_t*)second->hash)) return false;
    /*check keys*/
    if(memcmp(first->key,second->key, map->key_size))return false;
    return true;
}

static inline uint64_t hash_to_index(map_double_ptr map, uint64_t hash){
    return hash & (map->mem_size-1);
}

static inline uint8_t insert_entry(map_double_ptr obj, uint64_t index, entry_t* ins_entry) {
    
    /*get entry*/
    entry_t entry; 
    get_entry(obj, &entry, index);

    /*check entry*/
    if(entry.info->state == ENTRY_OCCUPIED) {
        if(is_duplicate(obj,&entry, ins_entry)) return MAP_DUPLICATE;
        return MAP_ERROR; 
    }

    /*lock entry*/
    if( !_Sync_check_and_set(&entry.info->used, 0, 1)) {
        _Sync_increment(&obj->count_lock_error);
        return MAP_ERROR;
    }
    
    /*update entry*/
    copy_entry(obj, &entry, ins_entry);
    
    /*unlock*/
    _Sync_increment(&obj->count);  
    entry.info->state = ENTRY_OCCUPIED;
    entry.info->used = 0;
    return MAP_SUCCESS;
}

static void print_entry(map_double_ptr map, uint64_t index) {
    entry_t entry; 
    get_entry(map,&entry, index);
    printf("[%zu] key %p value %p\n", index, entry.key, entry.value);
}

static void print_key(map_double_ptr map, uint64_t index) {
    size_t i;
    entry_t entry; 
    get_entry(map,&entry, index);
    printf("[%zu] key = ", index);
    for(i=0;i<map->key_size;i++)
        printf("%02X", entry.key[i]);
    printf("\n");
}

//=================================================================================
//                              API
//=================================================================================

uint8_t insert_double(map_double_ptr obj, const uint8_t* key, const uint8_t* value) {
    uint64_t hash;
    uint64_t cur_index;
    uint8_t status;
    entry_t ins_entry = {.hash = &hash, .key = key, .value = value};
    _Sync_increment(&obj->count_inserts);
    /*---------------1st probe-----------------*/
    hash = xxhash64(key, obj->key_size);
    cur_index = hash_to_index(obj, hash);
    status = insert_entry(obj, cur_index, &ins_entry);
    if(status != MAP_ERROR) return status;
    _Sync_increment(&obj->count_collision);
    /*---------------2nd probe-----------------*/
    hash = mod_hash(key, obj->key_size);
    cur_index = hash_to_index(obj, hash);
    status = insert_entry(obj, cur_index, &ins_entry);
    if(status != MAP_ERROR) return status;
    _Sync_increment(&obj->count_collision);
    /*---------------others-----------------*/
    uint64_t i = 0;
    for(i = 0; i < obj->mem_size; i++, cur_index++) {
        cur_index = hash_to_index(obj, cur_index+1);
        status = insert_entry(obj, cur_index, &ins_entry);
        if(status != MAP_ERROR) return status;
        _Sync_increment(&obj->count_collision);
    }

    return MAP_ERROR;
}

uint8_t* find_double(map_double_ptr obj, const uint8_t* key) {
    uint64_t hash;
    uint64_t cur_index;
    entry_t entry;
    entry_t sel_entry = {.hash = &hash, .key = key};

    /*---------------1st probe-----------------*/
    hash = xxhash64(key, obj->key_size);
    cur_index = hash_to_index(obj, hash);
    get_entry(obj,&entry, cur_index);
    if(entry.info->state == ENTRY_OCCUPIED) {
        if(is_duplicate(obj,&entry, &sel_entry)) return entry.value; 
    }

    /*---------------2nd probe-----------------*/
    hash = mod_hash(key, obj->key_size);
    cur_index = hash_to_index(obj, hash);
    get_entry(obj,&entry, cur_index);
    if(entry.info->state == ENTRY_OCCUPIED) {
        if(is_duplicate(obj,&entry, &sel_entry)) return entry.value;
    }

    /*---------------others-----------------*/
    uint64_t i = 0;
    for(i = 0; i < obj->mem_size; i++, cur_index++) {
        cur_index = hash_to_index(obj, cur_index+1);
        get_entry(obj,&entry, cur_index);
        if(entry.info->state == ENTRY_OCCUPIED) {
            if(is_duplicate(obj,&entry, &sel_entry)) return entry.value;
        }
    }
    
    return NULL;
}

uint8_t erase_double(map_double_ptr obj, const uint8_t* key) {
    uint64_t hash;
    uint64_t cur_index;
    entry_t entry;
    entry_t sel_entry = {.hash = &hash, .key = key};

    /*---------------1st probe-----------------*/
    hash = xxhash64(key, obj->key_size);
    cur_index = hash_to_index(obj, hash);
    get_entry(obj, &entry, cur_index);
    if(entry.info->state == ENTRY_OCCUPIED) {
         if(is_duplicate(obj,&entry, &sel_entry)){
            entry.info->state = ENTRY_DELETED;
            _Sync_decrement(&obj->count);
            return MAP_SUCCESS;
         }
    }

   /*---------------2nd probe-----------------*/
    hash = mod_hash(key, obj->key_size);
    cur_index = hash_to_index(obj, hash);
    get_entry(obj,&entry, cur_index);
    if(entry.info->state == ENTRY_OCCUPIED) {
        if(is_duplicate(obj,&entry, &sel_entry)){
            entry.info->state = ENTRY_DELETED;
            _Sync_decrement(&obj->count);
            return MAP_SUCCESS;
         }
    }

    /*---------------others-----------------*/
    uint64_t i = 0;
    for(i = 0; i < obj->mem_size; i++,cur_index++) {
        cur_index = hash_to_index(obj, cur_index+1);
        get_entry(obj, &entry, cur_index);
        if(entry.info->state == ENTRY_OCCUPIED) {
            if(is_duplicate(obj,&entry, &sel_entry)){
                entry.info->state = ENTRY_DELETED;
                _Sync_decrement(&obj->count);
                return MAP_SUCCESS;
            }
        }
    }

    return MAP_ERROR;
}

size_t count_double(map_double_ptr obj) {
    return obj->count;
}

void get_stat_double(const map_double_ptr map, map_counters_t* statistic) {
    statistic->insert = map->count_inserts;
    statistic->collisions = map->count_collision;
    statistic->lock_errors = map->count_lock_error;
}