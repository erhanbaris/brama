#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>

#include <assert.h>
#include <stdint.h>

#include "tests.h"
#include "munit.h"
#include <float.h>

#include "brama.h"

int main(int argc, const char* argv[]) {
    t_context* context = brama_init(0);
    brama_compile(context, "var bob = {};"
    "bob['add'] = function(x,y) { return x+y; }; var result = bob['add'](5+3)");
    brama_run(context);
    brama_compile_dump(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int     (status,            == , BRAMA_OK);
    munit_assert_int     (var_info->type,    == , CONST_INTEGER);
    munit_assert_int     (var_info->double_, == , 8);

    t_brama_value saved_value =  *map_get(var_info->dict_, "add");
    munit_assert_true(IS_FUNCTION(saved_value));

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return 0;

    
    
#ifdef _WIN32
    int test_status = munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
    getchar();
    return test_status;
#else
    int test_status = munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
    getchar();
    return test_status;
#endif
}
