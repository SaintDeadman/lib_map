#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "map.h"
#include "common.h"

int main()
{
    avr_insert("simple_map", hash_simple);
    avr_find("simple_map",hash_simple);
    avr_erase("simple_map", hash_simple);
    avr_ins_del_one_thread("simple_map", hash_simple);
    test_multitreads(hash_simple);
    return 0;
}