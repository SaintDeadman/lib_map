#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map_double.h"
#include "misc/allocator.h"
#include "misc/sync.h"

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
    void* mem;
    entry_info_t* sync_data;
    _Sync size_t count;
    _Sync size_t count_lock_error;
} map_double_t;

#define MAP_SUCCESS (0)
#define MAP_DUPLICATE (2)
#define MAP_ERROR (1)

#define ENTRY_FREE      0
#define ENTRY_OCCUPIED  1
#define ENTRY_DELETED   2

map_double_ptr new_double_map(const size_t map_size, const size_t key_size, const size_t val_size)  {
    /*create struct*/
    map_double_ptr  map = (map_double_ptr)malloc(sizeof(map_double_t));
    if(!map) return NULL;
    memset(map, 0x0, sizeof(map_double_t));

    /*fill struct*/
    map->mem_size = map_size;
    map->key_size = key_size;
    map->val_size = val_size;
    map->count = 0;

    /*allocate mem to nodes*/
    size_t entry_size = sizeof(uint64_t) + map->key_size + map->val_size;
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

void free_double_map(map_double_ptr map) {
    deallocate_memory(map->sync_data, map->mem_size*sizeof(entry_info_t));
    deallocate_memory(map->mem, map->mem_size*(sizeof(uint64_t) + map->key_size + map->val_size));
    free(map);
}

/*=============================================================*/
static entry_t get_entry(map_double_ptr map, uint64_t index) {
    uint64_t ent_size = sizeof(uint64_t) + map->key_size+map->val_size;
    entry_t ent = {
                .info = &map->sync_data[index],
                .hash = (uint8_t*)(map->mem + index*ent_size), 
                .key = (uint8_t*)(map->mem + index*ent_size + sizeof(uint64_t)),
                .value = (uint8_t*)(map->mem + index*ent_size + map->key_size + sizeof(uint64_t) ) };
    return ent;
}

static void update_entry(map_double_ptr map, entry_t entry, const entry_t* new_entry) {
    memcpy(entry.hash, new_entry->hash,sizeof(uint64_t));
    memcpy(entry.key, new_entry->key, map->key_size);
    memcpy(entry.value, new_entry->value, map->val_size);
}

static bool is_duplicate(map_double_ptr map, entry_t* first, const entry_t* second) {
    /*check hashes*/
    if(*((uint64_t*)first->hash) != *((uint64_t*)second->hash)) return false;
    /*check keys*/
    if(memcmp(first->key,second->key, map->key_size))return false;
    return true;
}

static void print_entry(map_double_ptr map, uint64_t index) {
    entry_t entry = get_entry(map, index);
    printf("[%zu] key %p value %p\n", index, entry.key, entry.value);
}

static void print_key(map_double_ptr map, uint64_t index) {
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

static uint64_t fnv1a_64(const void *data, size_t len) {
    
    const uint8_t *bytes = (const uint8_t *)data;
    const uint64_t FNV_prime = 1099511628211ULL;
    uint64_t hash = 14695981039346656037ULL;

    for (size_t i = 0; i < len; ++i) {
        hash ^= bytes[i];
        hash *= FNV_prime;
    }

    return hash;
}

//=================================================================================
//                              API
//=================================================================================

uint8_t insert_entry(map_double_ptr obj, uint64_t index, entry_t* ins_entry) {
    
    /*get entry*/
    entry_t entry = get_entry(obj, index);

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
    update_entry(obj, entry, ins_entry);
    
    /*unlock*/
    _Sync_increment(&obj->count);  
    entry.info->state = ENTRY_OCCUPIED;
    entry.info->used = 0;
    return MAP_SUCCESS;
}


uint8_t insert_double(map_double_ptr obj, const uint8_t* key, const uint8_t* value) {
    uint64_t hash;
    uint64_t cur_index;
    uint8_t status;
    entry_t ins_entry = {.hash = &hash, .key = key, .value = value};

    /* 1st probe */
    hash = fnv1a_64(key, obj->key_size);
    cur_index = hash % obj->mem_size;
    status = insert_entry(obj, cur_index, &ins_entry);
    if(status == MAP_SUCCESS) return MAP_SUCCESS;
    if(status == MAP_DUPLICATE) return MAP_DUPLICATE;

    /* 2nd probe */
    hash = hash_func(key, obj->key_size);
    cur_index = hash % obj->mem_size;
    status = insert_entry(obj, cur_index, &ins_entry);
    if(status == MAP_SUCCESS) return MAP_SUCCESS;
    if(status == MAP_DUPLICATE) return MAP_DUPLICATE;
    
    /*others*/
    uint64_t i = 0;
    for(i = 0; i < obj->mem_size; i++) {
        cur_index = (cur_index+1) % obj->mem_size;
        status = insert_entry(obj, cur_index, &ins_entry);
        if(status == MAP_SUCCESS) return MAP_SUCCESS;
        if(status == MAP_DUPLICATE) return MAP_DUPLICATE;
    }

    return MAP_ERROR;
}

uint8_t* find_double(map_double_ptr obj, const uint8_t* key) {
    uint64_t hash;
    uint64_t cur_index;
    entry_t entry;
    entry_t sel_entry = {.hash = &hash, .key = key};

    /* 1st probe */
    hash = fnv1a_64(key, obj->key_size);
    cur_index = hash % obj->mem_size;
    entry = get_entry(obj, cur_index);

    /*check entry*/
    if(entry.info->state == ENTRY_OCCUPIED) {
        if(is_duplicate(obj,&entry, &sel_entry)) return entry.value; 
    }

    /* 2nd probe */
    hash = hash_func(key, obj->key_size);
    cur_index = hash % obj->mem_size;
    entry = get_entry(obj, cur_index);
    /*check entry*/
    if(entry.info->state == ENTRY_OCCUPIED) {
        if(is_duplicate(obj,&entry, &sel_entry)) return entry.value;
    }

    /*others*/
    uint64_t i = 0;
    for(i = 0; i < obj->mem_size; i++) {
        cur_index = (cur_index+1) % obj->mem_size;
        entry = get_entry(obj, cur_index);
        /*check entry*/
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

    /* 1st probe */
    hash = fnv1a_64(key, obj->key_size);
    cur_index = hash % obj->mem_size;
    entry = get_entry(obj, cur_index);
    /*check entry*/
    if(entry.info->state == ENTRY_OCCUPIED) {
         if(is_duplicate(obj,&entry, &sel_entry)){
            entry.info->state = ENTRY_DELETED;
            _Sync_decrement(&obj->count);
            return MAP_SUCCESS;
         }
    }

    /* 2nd probe */
    hash = hash_func(key, obj->key_size);
    cur_index = hash % obj->mem_size;
    entry = get_entry(obj, cur_index);
    /*check entry*/
    if(entry.info->state == ENTRY_OCCUPIED) {
        if(is_duplicate(obj,&entry, &sel_entry)){
            entry.info->state = ENTRY_DELETED;
            _Sync_decrement(&obj->count);
            return MAP_SUCCESS;
         }
    }

    /*others*/
    uint64_t i = 0;
    for(i = 0; i < obj->mem_size; i++) {
        cur_index = (cur_index+1) % obj->mem_size;
        entry = get_entry(obj, cur_index);
        /*check entry*/
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