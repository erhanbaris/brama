#ifndef TESTS_COMPILE_H
#define TESTS_COMPILE_H

#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"

#define OPCODE_TO_INT(I)\
    I .bytes[3] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[2] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[1] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[0] = context->compiler->op_codes->data[ ++opcode_index ];

#define OPCODE_TO_DOUBLE(I)\
    I .bytes[7] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[6] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[5] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[4] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[3] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[2] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[1] = context->compiler->op_codes->data[ ++opcode_index ];\
    I .bytes[0] = context->compiler->op_codes->data[ ++opcode_index ];

MunitResult ast_compile_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "10 + 20");
    brama_run(context);
    size_t opcode_index = 0;
    munit_assert_int(context->compiler->op_codes->length, == , 11);
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
    return MUNIT_OK;
}

MunitResult ast_compile_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "10.1 + 20.2");
    brama_run(context);
    size_t opcode_index = 0;
    munit_assert_int(context->compiler->op_codes->length, == , 19);
    munit_assert_int(context->status, == , BRAMA_OK);

    munit_assert_int(context->compiler->op_codes->data[opcode_index], ==, VM_OPT_CONST_DOUBLE);
    t_brama_double d;
    OPCODE_TO_DOUBLE(d);
    munit_assert_int(d.double_, ==, 10.1);

    munit_assert_int(context->compiler->op_codes->data[++opcode_index], ==, VM_OPT_CONST_DOUBLE);
    OPCODE_TO_DOUBLE(d);
    munit_assert_int(d.double_, ==, 20.2);

    munit_assert_int(context->compiler->op_codes->data[++opcode_index], ==, VM_OPT_ADDITION);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "1024 + (22.1 + 11)");
    brama_run(context);
    size_t opcode_index = 0;
    munit_assert_int(context->compiler->op_codes->length, == , 19);
    munit_assert_int(context->status, == , BRAMA_OK);

    munit_assert_int(context->compiler->op_codes->data[opcode_index], ==, VM_OPT_CONST_DOUBLE);
    t_brama_double d;
    OPCODE_TO_DOUBLE(d);
    munit_assert_double(d.double_, ==, 10.1);

    munit_assert_int(context->compiler->op_codes->data[++opcode_index], ==, VM_OPT_CONST_DOUBLE);
    OPCODE_TO_DOUBLE(d);
    munit_assert_double(d.double_, ==, 20.2);

    munit_assert_int(context->compiler->op_codes->data[++opcode_index], ==, VM_OPT_ADDITION);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitTest COMPILE_TESTS[] = {

        ADD_TEST(ast_compile_1),
        ADD_TEST(ast_compile_2),
        ADD_TEST(ast_compile_3),

        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_COMPILE_H
