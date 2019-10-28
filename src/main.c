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
    t_context* context = brama_init();
    brama_compile(context, "var x = 9007199254740992;"
"var y = -x;"
"var a = x == x + 1;"
"var b = y == y - 1; "
"var c = x / 2;"
"var d = x / 2;"
"var e = x >> 1;     // 0"
"var f = x | 1;      // 1");
    brama_run(context);
    brama_compile_dump(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "fib", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_INTEGER);
    munit_assert_int   (var_info->double_,== , 55);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);

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
