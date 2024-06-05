#ifndef _MAP_MOCK_H_
#define _MAP_MOCK_H_

#include <stdint.h>

uint8_t insert_mock(void* obj, const uint8_t* key, const uint8_t* value);
void find_mock(void* obj);
void erase_mock(void* obj); 

#endif //_MAP_MOCK_H_