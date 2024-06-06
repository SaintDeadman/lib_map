#ifndef _MAP_SIMPLE_H_
#define _MAP_SIMPLE_H_

#include <stdint.h>
#include <stddef.h>

typedef struct map_simple* map_simple_ptr;

map_simple_ptr new_simple_map(size_t map_size, size_t key_size, size_t val_size);
void free_simple_map(map_simple_ptr);

uint8_t insert_simple(map_simple_ptr obj, const uint8_t* key, const uint8_t* value);
uint8_t* find_simple(map_simple_ptr obj, const uint8_t* key);
uint8_t erase_simple(map_simple_ptr obj, const uint8_t* key);


#endif //_MAP_SIMPLE_H_