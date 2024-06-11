#include "../map_lib/hash.h"
// #include "../map_lib/map_simple.h"

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "common.h"


#define MAP_SIZE ((size_t)1e6)
#define KEY_SIZE (64)
#define VAL_SIZE (128)
#define NUM_THREADS 32

typedef struct {
    uint8_t key[KEY_SIZE];
    uint8_t val[VAL_SIZE];
}test_entry_t;

typedef struct {
    int thread_id;
    void *table;
} thread_data_t;

static void generate_rand_entry(test_entry_t* entry) {
    generate_random_bytes(entry->key, KEY_SIZE);
    generate_random_bytes(entry->val, VAL_SIZE);
}

void test_create_table(void) {
    map_t* test_map = new_map(hash_simple, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) {
        CU_FAIL("allocation error")
        return;
    }
    free_map(test_map);
}

void test_insert_node(void) {
    /*create map*/
    map_t* test_map = new_map(hash_simple, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) {
        CU_FAIL("allocation error")
        return;
    }
    
    /*insert */
    test_entry_t entry;
    generate_rand_entry(&entry);
    CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), 0);
    
    /*check count */
    CU_ASSERT_EQUAL(countm(test_map), 1);
    
    /*free*/
    free_map(test_map);
}

void test_find_node(void) {
    /*create map*/
    map_t* test_map = new_map(hash_simple, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    CU_ASSERT_PTR_NOT_NULL(test_map);
    /*insert */
    test_entry_t entry;
    generate_rand_entry(&entry);
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
    map_t* test_map = new_map(hash_simple, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    CU_ASSERT_PTR_NOT_NULL(test_map);
    /*insert */
    test_entry_t entry;
    generate_rand_entry(&entry);
    CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), 0);
    /*delete*/
    CU_ASSERT_EQUAL(erasem(test_map, entry.key), 0);
    CU_ASSERT_EQUAL(countm(test_map), 0);
    /*free*/
    free_map(test_map);
}

void test_fill_table(void) {
    uint32_t map_size = (uint32_t)1e6;
    /*create map*/
    map_t* test_map = new_map(hash_simple, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) {
        CU_FAIL("allocation error")
        return;
    }
    /*fill*/
    uint32_t i = 0;
    test_entry_t entry;
    for(i =0; i < map_size; i++){
        generate_rand_entry(&entry);
        CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), 0);
    }
    /*check count*/
    CU_ASSERT_EQUAL(countm(test_map), map_size);
    /*free*/
    free_map(test_map);
}

void *thread_func(void *arg) {
    thread_data_t* thread_data = (thread_data_t*)arg;
    test_entry_t entry;
    for (size_t i = 0; i < (size_t)1e6; ++i) {
        generate_rand_entry(&entry);
        uint8_t status = insertm(thread_data->table, entry.key, entry.val);
        if(status == 0) {
            status = erasem(thread_data->table, entry.key);
            CU_ASSERT_EQUAL(status, 0);
            if(status){
                CU_FAIL("can't erase entry");
            }
        }
        else if(status == 2) {
            printf("duplicate entry\n");
        }
        else {
            CU_FAIL("space error\n");
        }
    }

    pthread_exit(NULL);
}
void test_multitreads(void) {
    /*create map*/
    map_t* test_map = new_map(hash_simple, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) {
        CU_FAIL("allocation error")
        return;
    }

    /*pre-fill*/
    uint32_t i = 0;
    test_entry_t entry;
    for(i =0; i < MAP_SIZE*0.75; i++) {
        generate_rand_entry(&entry);
        CU_ASSERT_EQUAL(insertm(test_map, entry.key, entry.val), 0);
    }
    
    /*create threads*/
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].thread_id = i;
        thread_data[i].table = test_map;
        pthread_create(&threads[i], NULL, thread_func, (void *)&thread_data[i]);
    }
    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_join(threads[i], NULL);

    /*free*/
    free_map(test_map);
}


int main() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_pSuite pSuite = CU_add_suite("SimpleTestSuite", 0, 0);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "test_create_table", test_create_table)) ||
        (NULL == CU_add_test(pSuite, "test_insert_node", test_insert_node)) ||
        (NULL == CU_add_test(pSuite, "test_find_node", test_find_node)) ||
        (NULL == CU_add_test(pSuite, "test_delete_node", test_delete_node)) ||
        (NULL == CU_add_test(pSuite, "test_fill_table", test_fill_table)) ||
        (NULL == CU_add_test(pSuite, "test_multitreads", test_multitreads))
        ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
