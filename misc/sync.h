#ifndef _SYNC_H_
#define _SYNC_H_

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #include <stdatomic.h>
    #define _Sync _Atomic volatile

    inline int _Sync_check_and_set(void* ptr, int val_e, int val_s) {
        return atomic_compare_exchange_strong((_Atomic int *)ptr, &val_e, val_s);
    }
#elif defined(__GNUC__) || defined(__clang__)
    #define _Sync volatile

    inline int _Sync_check_and_set(void* ptr, int val_e, int val_s) {
        return __sync_bool_compare_and_swap((int *)ptr, val_e, val_s);
    }
#else
    #error "No suitable sync operations available"
#endif

#endif //_SYNC_H_