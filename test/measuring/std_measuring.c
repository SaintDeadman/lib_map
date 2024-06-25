#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include "map.h"
#include "common.h"

int main()
{
    avr_insert("std_map", hash_std);
    avr_find("std_map",hash_std);
    avr_erase("std_map", hash_std);
    avr_ins_del_one_thread("std_map", hash_std);
    //test_multitreads(hash_std);
    return 0;
}