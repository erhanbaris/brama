#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>

#include <assert.h>
#include <stdint.h>

#include "tests.h"
#include "munit.h"

#include "brama.h"

int main(int argc, const char* argv[]) {
    t_context* context = brama_init();
    brama_compile(context, "10 + 20");
    brama_run(context);
    size_t opcode_index = 0;
    munit_assert_int(context->compiler->op_codes->length, == , 2);
    munit_assert_int(context->status, == , BRAMA_OK);

    munit_assert_int(context->compiler->op_codes->data[opcode_index], ==, VM_OPT_CONST_INT);
    t_brama_int i;
    OPCODE_TO_INT(i);
    munit_assert_int(i.int_, ==, 10);

    munit_assert_int(context->compiler->op_codes->data[++opcode_index], ==, VM_OPT_CONST_INT);
    OPCODE_TO_INT(i);
    munit_assert_int(i.int_, ==, 20);

    munit_assert_int(context->compiler->op_codes->data[++opcode_index], ==, VM_OPT_ADDITION);
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
