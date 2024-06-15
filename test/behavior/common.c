#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "common.h"
#include "../map_lib/map.h"

#define MAP_SIZE ((size_t)1e6)
#define KEY_SIZE (64)
#define VAL_SIZE (128)

extern hash_type_e map_name;

typedef struct {
    uint8_t key[KEY_SIZE];
    uint8_t val[VAL_SIZE];
}test_entry_t;

typedef struct {
    int thread_id;
    void *table;
} thread_data_t;

static void generate_random_bytes(unsigned int seed, unsigned char *buffer, size_t length) {
    srand((unsigned int)clock()+seed);
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = (unsigned char)(rand() % 256); // Генерация случайного байта
    }
}

static void generate_rand_entry(unsigned int seed, test_entry_t* entry) {
    generate_random_bytes(seed, entry->key, KEY_SIZE);
    generate_random_bytes(seed, entry->val, VAL_SIZE);
}

void test_create_table(void) {
    map_t* test_map = new_map(map_name, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) {
        CU_FAIL("allocation error")
        return;
    }
    free_map(test_map);
}

void test_insert_node(void) {
    /*create map*/
    map_t* test_map = new_map(map_name, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) {
        CU_FAIL("allocation error")
        return;
    }
    
    /*insert */
    test_entry_t entry;
    generate_rand_entry(0, &entry);
    CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), MAP_SUCCESS);
    /*check unique*/
    CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), MAP_DUPLICATE);
    /*check count */
    CU_ASSERT_EQUAL(countm(test_map), 1);
    
    /*free*/
    free_map(test_map);
}

void test_find_node(void) {
    /*create map*/
    map_t* test_map = new_map(map_name, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    CU_ASSERT_PTR_NOT_NULL(test_map);
    /*insert */
    test_entry_t entry;
    generate_rand_entry(0, &entry);
    CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), 0);
    /* find */
    uint8_t* value = findm(test_map,entry.key);
    CU_ASSERT_PTR_NOT_NULL(value);
    CU_ASSERT_EQUAL(memcmp(value, entry.val, VAL_SIZE), 0);
    /*free*/
    free_map(test_map);
}

void test_delete_node(void) {
    /*create map*/
    map_t* test_map = new_map(map_name, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    CU_ASSERT_PTR_NOT_NULL(test_map);
    /*insert */
    test_entry_t entry;
    generate_rand_entry(0, &entry);
    CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), 0);
    /*delete*/
    CU_ASSERT_EQUAL(erasem(test_map, entry.key), 0);
    CU_ASSERT_EQUAL(countm(test_map), 0);
    /*free*/
    free_map(test_map);
}

void test_fill_table(void) {
    /*create map*/
    map_t* test_map = new_map(map_name, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) {
        CU_FAIL("allocation error")
        return;
    }
    /*fill*/
    uint32_t i = 0;
    test_entry_t entry;
    for(i =0; i < MAP_SIZE; i++){
        generate_rand_entry(0, &entry);
        CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), 0);
    }
    /*check count*/
    CU_ASSERT_EQUAL(countm(test_map), MAP_SIZE);
    /*free*/
    free_map(test_map);
}
