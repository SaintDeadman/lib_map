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

typedef struct {
    uint8_t* ptr;
    uint32_t size;
} buffer_t;


void hash_create(hash_t* obj, hash_type_e type);

uint8_t insertm(hash_t* map, const uint8_t* key, const uint8_t* value);
void findm(hash_t* obj);
void erasem(hash_t* obj);

//void insertm(hash_t* obj, const uint8_t* key, const uint8_t* value);
//void findm(hash_t* obj, const uint8_t* key);
//void erasem(hash_t* obj, const uint8_t* key);

#endif //_HASH_H_