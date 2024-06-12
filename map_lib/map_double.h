#ifndef _MAP_DOUBLE_H_
#define _MAP_DOUBLE_H_
#include <stdint.h>

typedef struct map_double* map_double_ptr;

map_double_ptr new_double_map(const size_t map_size, const size_t key_size, const size_t val_size);
void free_double_map(map_double_ptr);
uint8_t insert_double(map_double_ptr obj, const uint8_t* key, const uint8_t* value);
uint8_t* find_double(map_double_ptr obj, const uint8_t* key);
uint8_t erase_double(map_double_ptr obj, const uint8_t* key);
size_t count_double(map_double_ptr obj);


#endif //_MAP_DOUBLE_H_