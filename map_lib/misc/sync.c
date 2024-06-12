#include <stdlib.h>
#include "sync.h"

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

    int _Sync_check_and_set(void* ptr, int val_e, int val_s) {
        return atomic_compare_exchange_strong((_Atomic int *)ptr, &val_e, val_s);
    }

    void _Sync_increment(_Sync size_t *value) {
        atomic_fetch_add(value, 1);
    }

    void _Sync_decrement(_Sync size_t *value) {
        atomic_fetch_sub(value, 1);
    }

#elif defined(__GNUC__) || defined(__clang__)
    #define _Sync volatile

    int _Sync_check_and_set(void* ptr, int val_e, int val_s) {
        return __sync_bool_compare_and_swap((int *)ptr, val_e, val_s);
    }

    void _Sync_increment(_Sync size_t *value) {
        __sync_fetch_and_add(value, 1);
    }

    void _Sync_decrement(_Sync size_t *value) {
        __sync_fetch_and_sub(value, 1);
    }

#else
    #error "No suitable sync operations available"

#endif