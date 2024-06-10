#ifndef _MAP_MOCK_H_
#define _MAP_MOCK_H_

#include <stdint.h>

void* new_mock_map(const size_t size, const size_t key_size, const size_t val_size);
void  free_mock_map(void* map);
uint8_t insert_mock(const void* map, const uint8_t* key, const uint8_t* value);
uint8_t* find_mock(const void* map, const uint8_t* key);
uint8_t erase_mock(const void* map, const uint8_t* key); 

#endif //_MAP_MOCK_H_