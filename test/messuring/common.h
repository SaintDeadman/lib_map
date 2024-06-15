#ifndef _COMMON_H_
#define _COMMON_H_

#include "map.h"

void avr_insert(char* name, hash_type_e map);
void avr_find(char* name, hash_type_e map);
void avr_erase(char* name, hash_type_e map);
void avr_ins_del_one_thread(char* name, hash_type_e map );
void test_multitreads(hash_type_e map);

#endif //_COMMON_H_