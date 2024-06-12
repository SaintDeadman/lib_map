#include "../map_lib/hash.h"

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "common.h"

extern const hash_type_e map_name =  hash_double;


int main() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_pSuite pSuite = CU_add_suite("SimpleTestSuite", 0, 0);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((NULL == CU_add_test(pSuite, "test_create_table", test_create_table)) ||
        (NULL == CU_add_test(pSuite, "test_insert_node", test_insert_node)) ||
        (NULL == CU_add_test(pSuite, "test_find_node", test_find_node)) ||
        (NULL == CU_add_test(pSuite, "test_delete_node", test_delete_node)) ||
        (NULL == CU_add_test(pSuite, "test_fill_table", test_fill_table)) ||
        (NULL == CU_add_test(pSuite, "test_multitreads", test_multitreads))
        ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
