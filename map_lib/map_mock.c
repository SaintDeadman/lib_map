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

void find_mock(void* obj) 
{
    (void)(obj);
    printf("find function is undefined\n\r");
}

void erase_mock(void* obj) 
{
    (void)(obj);
    printf("erase function is undefined\n\r");
}