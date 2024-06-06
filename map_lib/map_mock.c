#include <stdio.h>
#include "map_mock.h"


uint8_t insert_mock(void* obj, const uint8_t* key, const uint8_t* value) 
{
    (void)(obj);
    (void)(key);
    (void)(value);
    printf("insert function is undefined\n\r");
    return 1;
}

uint8_t* find_mock(void* obj, const uint8_t* key) 
{
    (void)(obj);
    (void)(key);
    printf("find function is undefined\n\r");
    return NULL;
}

uint8_t erase_mock(void* obj, const uint8_t* key) 
{
    (void)(obj);
    (void)(key);
    printf("erase function is undefined\n\r");
    return 1;
}