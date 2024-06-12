#ifndef _SYNC_H_
#define _SYNC_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #include <stdatomic.h>
    #define _Sync _Atomic volatile
#elif defined(__GNUC__) || defined(__clang__)
    #define _Sync volatile
#else
    #error "No suitable sync operations available"
#endif

int _Sync_check_and_set(void* ptr, int val_e, int val_s);
void _Sync_increment(_Sync size_t *value);
void _Sync_decrement(_Sync size_t *value);

#endif //_SYNC_H_