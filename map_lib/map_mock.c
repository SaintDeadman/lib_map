#include <stdio.h>
#include "map_mock.h"
#define UNUSED(x) (void)(x)

void* new_mock_map(const size_t size, const size_t key_size, const size_t val_size){
    UNUSED(size);
    UNUSED(key_size);
    UNUSED(val_size);
    printf("new function is undefined\n");
    return NULL;
}

void free_mock_map(void* map) {
    UNUSED(map);
    printf("free function is undefined\n");
}

uint8_t insert_mock(const void* map, const uint8_t* key, const uint8_t* value) {
    UNUSED(map);
    UNUSED(key);
    UNUSED(value);
    printf("insert function is undefined\n");
    return 1;
}

uint8_t* find_mock(const void* map, const uint8_t* key) {
    UNUSED(map);
    UNUSED(key);
    printf("find function is undefined\n");
    return NULL;
}

uint8_t erase_mock(const void* map, const uint8_t* key) {
    UNUSED(map);
    UNUSED(key);
    printf("erase function is undefined\n\r");
    return 1;
}

size_t count_mock(const void* map) {
    UNUSED(map);
    return 0;
}