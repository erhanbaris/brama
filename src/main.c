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
    
    void* ptr = realloc(NULL, 10241);
    t_context* context = brama_init(0);
    brama_compile(context, "var a1=5;\n"
                           "var b1=6;\n"
                           "var r1=0;\n"
                           "\n"
                           "switch(4){\n"
                           "  case 6:\n"
                           "    break;\n"
                           "  case 4:\n"
                           "    break;\n"
                           "  case 7:\n"
                           "    break;\n"
                           "  default:\n"
                           "    break;\n"
                           "}");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "r1", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 0);
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
