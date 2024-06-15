

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "map.h"

#define MAP_SIZE ((size_t)1e7)
#define KEY_SIZE (64LU)
#define VAL_SIZE (128LU)

#define PACK_COUNT ((uint64_t)1e2)
#define KEY_SIZE (64)
#define KEYS_PER_PACK (1024)

#define NUM_THREADS 32
#define NUM_INS_PER_THREAD (1e2)

typedef struct {
    int thread_id;
    void *table;
} thread_data_t;

typedef struct {
    int thread_id;
    clock_t avr_val;
} thread_ret_data_t;

typedef struct {
    uint8_t key[KEY_SIZE];
    uint8_t val[VAL_SIZE];
}test_entry_t;

static void generate_random_data(unsigned int seed, unsigned char *buffer, size_t length) {
    srand((unsigned int)clock()+seed);
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = (unsigned char)(rand() % 256); // Генерация случайного байта
    }
}
static void generate_random_keys_pack(unsigned int seed, unsigned char *key, size_t key_size, size_t pack) {
    for (size_t i = 0; i < pack; ++i)
        generate_random_data(seed, key+i*key_size, key_size);
}
static void generate_rand_entry(unsigned int seed, test_entry_t* entry) {
    generate_random_data(seed, entry->key, KEY_SIZE);
    generate_random_data(seed, entry->val, VAL_SIZE);
}


void avr_insert(char* name, hash_type_e map) {
    map_t* test_map = new_map(map, MAP_SIZE,KEY_SIZE,VAL_SIZE);
    
    int64_t num_key, it = 0;
    uint8_t keys [KEYS_PER_PACK][KEY_SIZE];
    uint8_t vals [KEYS_PER_PACK][VAL_SIZE];
    
    clock_t clock_start = 0;
    clock_t clock_end = 0;
    clock_t clock_sum = 0;
    for(it =0; it < PACK_COUNT; it++) {
        generate_random_keys_pack(0, keys, KEY_SIZE,KEYS_PER_PACK);
        generate_random_keys_pack(0, vals, VAL_SIZE,KEYS_PER_PACK);

        clock_start = clock();
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            insertm(test_map,keys[num_key],vals[num_key]);
        }
        clock_end = clock();
        clock_sum += (clock_end - clock_start);
    }
    map_counters_t counters;
    getstatm(test_map, &counters);
    printf("[INS] Table %s\n", name);
    printf("[INS] \tavr insert pack time [pack/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT);
    printf("[INS] \tavr insert node time [node/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT/(double)KEYS_PER_PACK);
    printf("[INS] \tcount = %zu, collisions = %zu, lock_errors = %zu\n", counters.insert, counters.collisions, counters.lock_errors);

    free_map(test_map);
}

void avr_find(char* name, hash_type_e map) {
    map_t* test_map = new_map(map, MAP_SIZE,KEY_SIZE,VAL_SIZE);
    
    int64_t num_key, it = 0;
    uint8_t keys [KEYS_PER_PACK][KEY_SIZE];
    uint8_t vals [KEYS_PER_PACK][VAL_SIZE];
    
    
    clock_t clock_start = 0;
    clock_t clock_end = 0;
    clock_t clock_sum = 0;
    for(it =0; it < PACK_COUNT; it++) {
        generate_random_keys_pack(0, keys, KEY_SIZE,KEYS_PER_PACK);
        generate_random_keys_pack(0, vals, VAL_SIZE,KEYS_PER_PACK);
        /*fill*/
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            insertm(test_map,keys[num_key],vals[num_key]);
        }

        /*find*/
        clock_start = clock();
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            if(!findm(test_map,keys[num_key])) return;
        }
        clock_end = clock();
        clock_sum += (clock_end - clock_start);
    }
    map_counters_t counters;
    getstatm(test_map, &counters);
    printf("[SEL] Table %s\n", name);
    printf("[SEL] \tavr select pack time [pack/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT);
    printf("[SEL] \tavr select node time [node/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT/(double)KEYS_PER_PACK);
    printf("[SEL] \tcount = %zu, collisions = %zu, lock_errors = %zu\n", counters.insert, counters.collisions, counters.lock_errors);
    free_map(test_map);
}

void avr_erase(char* name, hash_type_e map) {
    map_t* test_map = new_map(map, MAP_SIZE,KEY_SIZE,VAL_SIZE);
    
    int64_t num_key, it = 0;
    uint8_t keys [KEYS_PER_PACK][KEY_SIZE];
    uint8_t vals [KEYS_PER_PACK][VAL_SIZE];
    
    
    clock_t clock_start = 0;
    clock_t clock_end = 0;
    clock_t clock_sum = 0;
    for(it =0; it < PACK_COUNT; it++) {
        generate_random_keys_pack(0, keys, KEY_SIZE,KEYS_PER_PACK);
        generate_random_keys_pack(0, vals, VAL_SIZE,KEYS_PER_PACK);
        /*fill*/
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            if(insertm(test_map,keys[num_key],vals[num_key]) == MAP_ERROR) {
                printf("ins count[%zu]\n", countm(test_map));
                _exit(0);
            }
        }

        /*find*/
        clock_start = clock();
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            if(erasem(test_map, keys[num_key]) == MAP_ERROR) printf("error [%x]\n", keys[num_key][0]);
        }
        clock_end = clock();
        clock_sum += (clock_end - clock_start);
    }
    map_counters_t counters;
    getstatm(test_map, &counters);
    printf("[DEL] Table %s\n", name);
    printf("[DEL] \tavr delete pack time [pack/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT);
    printf("[DEL] \tavr delete node time [node/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT/(double)KEYS_PER_PACK);
    printf("[DEL] \tcount = %zu, collisions = %zu, lock_errors = %zu\n", counters.insert, counters.collisions, counters.lock_errors);
    free_map(test_map);

}


void avr_ins_del_one_thread(char* name, hash_type_e map ) {
    map_t* test_map = new_map(map, MAP_SIZE,KEY_SIZE,VAL_SIZE);
    
    int64_t num_key, it = 0;
    uint8_t keys [KEYS_PER_PACK][KEY_SIZE];
    uint8_t vals [KEYS_PER_PACK][VAL_SIZE];

    /*pre-fill*/
    uint32_t i = 0;
    test_entry_t entry;
    for(i =0; i < MAP_SIZE*0.75; i++) {
        generate_rand_entry(0, &entry);
        insertm(test_map, entry.key, entry.val);
    }

    clock_t clock_start = 0;
    clock_t clock_end = 0;
    clock_t clock_sum = 0;
    for(it =0; it < PACK_COUNT; it++) {
        generate_random_keys_pack(0, keys, KEY_SIZE,KEYS_PER_PACK);
        generate_random_keys_pack(0, vals, VAL_SIZE,KEYS_PER_PACK);
        /*fill*/
        clock_start = clock();
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            insertm(test_map,keys[num_key],vals[num_key]);
            erasem(test_map,keys[num_key]);
        }
        clock_end = clock();
        clock_sum += (clock_end - clock_start);
    }
    map_counters_t counters;
    getstatm(test_map, &counters);
    printf("[INS|DEL][S] Table %s\n", name);
    printf("[INS|DEL][S] \tavr handle pack time [pack/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT);
    printf("[INS|DEL][S] \tavr handle node time [node/clock] = %.8f\n", (double)clock_sum/(double)PACK_COUNT/(double)KEYS_PER_PACK);
    printf("[INS|DEL][S] \tcount = %zu, collisions = %zu, lock_errors = %zu\n", counters.insert, counters.collisions, counters.lock_errors);
    free_map(test_map);
}




void *thread_func(void *arg) {
    thread_data_t* thread_data = (thread_data_t*)arg;
    test_entry_t entry;
    int64_t num_key, it = 0;
    uint8_t keys [KEYS_PER_PACK][KEY_SIZE];
    uint8_t vals [KEYS_PER_PACK][VAL_SIZE];

    clock_t clock_start = 0;
    clock_t clock_end = 0;
    clock_t clock_sum = 0;
    for(it =0; it < PACK_COUNT; it++) {
        generate_random_keys_pack(thread_data->thread_id, keys, KEY_SIZE,KEYS_PER_PACK);
        generate_random_keys_pack(thread_data->thread_id, vals, VAL_SIZE,KEYS_PER_PACK);
        /*fill*/
        clock_start = clock();
        for(num_key =0; num_key < KEYS_PER_PACK; num_key++) {
            if(insertm((thread_data_t*)thread_data->table,keys[num_key],vals[num_key]) == 0);
                erasem((thread_data_t*)thread_data->table,keys[num_key]);
        }
        clock_end = clock();
        clock_sum += (clock_end - clock_start);
    }
    thread_ret_data_t* ret = malloc(sizeof(thread_ret_data_t));
    ret->thread_id = thread_data->thread_id;
    ret->avr_val = clock_sum; 
    pthread_exit(ret);
}

void test_multitreads(hash_type_e map) {
    /*create map*/
    map_t* test_map = new_map(map, MAP_SIZE, KEY_SIZE, VAL_SIZE);
    if(!test_map) return;

    /*pre-fill*/
    uint32_t i = 0;
    test_entry_t entry;
    for(i =0; i < MAP_SIZE*0.75; i++) {
        generate_rand_entry(0, &entry);
        insertm(test_map, entry.key, entry.val);
    }

    /*create threads*/
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_data[i].thread_id = i;
        thread_data[i].table = test_map;
        pthread_create(&threads[i], NULL, thread_func, (void *)&thread_data[i]);
    }
    thread_ret_data_t* ret;
    clock_t avr_threads = 0;
    for (int i = 0; i < NUM_THREADS; ++i){
        pthread_join(threads[i], &ret);
        if(ret) avr_threads += ret->avr_val;
        
    }
    map_counters_t counters;
    getstatm(test_map, &counters);
    printf("[INS|DEL][M] \tavr handle pack time [pack/clock] = %.8f\n", (double)ret->avr_val/(double)PACK_COUNT);
    printf("[INS|DEL][M] \tavr handle node time [node/clock] = %.8f\n", (double)ret->avr_val/(double)PACK_COUNT/(double)KEYS_PER_PACK);
    printf("[INS|DEL][M] \tcount = %zu, collisions = %zu, lock_errors = %zu\n", counters.insert, counters.collisions, counters.lock_errors);
    /*free*/
    free_map(test_map);
}
