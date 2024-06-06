#ifndef _HASH_H_
#define _HASH_H_

#include <stdint.h>

typedef enum{
    hash_simple = 0,
    hash_dummy
} hash_type_e;

typedef struct {
    hash_type_e type;
    void* obj;
} hash_t;


void hash_create(hash_t* obj, hash_type_e type);

uint8_t insertm(hash_t* map, const uint8_t* key, const uint8_t* value);
uint8_t* findm(hash_t* map, const uint8_t* key);
uint8_t erase_m(hash_t* obj, uint8_t* key);

/* TODO */
uint8_t* beginm(hash_t* map);
uint8_t* endm(hash_t* map);
uint8_t* nextm(hash_t* map);


#endif //_HASH_H_