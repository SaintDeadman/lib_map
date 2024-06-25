
#ifndef _MAP_STD_H_
#define _MAP_STD_H_

#include <stdint.h>
#include "map.h"


#ifdef __cplusplus
extern "C" {
#endif


void* new_std_map(const size_t size, const size_t key_size, const size_t val_size);
void free_std_map(void* map);
uint8_t insert_std(const void* map, const uint8_t* key, const uint8_t* value);
uint8_t* find_std(const void* map, const uint8_t* key);
uint8_t erase_std(const void* map, const uint8_t* key);
size_t count_std(const void* map);
void get_stat_std(const void* map, map_counters_t* statistic);

#ifdef __cplusplus
}
#endif

#endif //_MAP_STD_H_