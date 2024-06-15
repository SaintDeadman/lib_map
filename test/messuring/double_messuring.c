#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "map.h"
#include "common.h"

int main()
{
    avr_insert("double_map", hash_double);
    avr_find("double_map",hash_double);
    avr_erase("double_map", hash_double);
    avr_ins_del_one_thread("double_map", hash_double);
    test_multitreads(hash_double);
    return 0;
}