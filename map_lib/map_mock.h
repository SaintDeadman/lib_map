#ifndef _MAP_MOCK_H_
#define _MAP_MOCK_H_

#include <stdint.h>

uint8_t insert_mock(void* obj, const uint8_t* key, const uint8_t* value);
uint8_t* find_mock(void* map, const uint8_t* key);
uint8_t erase_mock(void* obj, const uint8_t* key); 

#endif //_MAP_MOCK_H_