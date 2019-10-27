#ifndef TESTS_COMPILE_H
#define TESTS_COMPILE_H

#include <brama.h>
#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"

MunitResult ast_compile_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "10 + 20");
    brama_run(context);
    size_t opcode_index = 0;
    munit_assert_int(context->compiler->op_codes->length, == , 2);
    munit_assert_int(context->compiler->op_codes->data[1], == , NULL);

    munit_assert_int(context->status, == , BRAMA_OK);

    t_brama_vmdata vmdata;
    vm_decode(context->compiler->op_codes->data[0], &vmdata);

    munit_assert_int(vmdata.op, ==, VM_OPT_ADDITION);
    munit_assert_int(context->compiler->global_storage->variables.length, ==, 1);
    munit_assert_int(context->compiler->global_storage->constants.length, ==, 2);

    munit_assert_double(valueToNumber(context->compiler->global_storage->constants.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->constants.data[1]), ==, 20.0);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 30.0);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var test1; var test2 = 1024; test1 = 2048;");
    brama_run(context);
    size_t opcode_index = 0;
    munit_assert_int(context->compiler->op_codes->length, == , 3);

    t_brama_vmdata vmdata;
    vm_decode(context->compiler->op_codes->data[0], &vmdata);

    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 2);
    munit_assert_int(vmdata.reg2, ==, -1);
    munit_assert_int(vmdata.reg3, ==, 0);

    vm_decode(context->compiler->op_codes->data[1], &vmdata);

    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 1);
    munit_assert_int(vmdata.reg2, ==, -2);
    munit_assert_int(vmdata.reg3, ==, 0);

    munit_assert_int(context->compiler->global_storage->variables.length, ==, 2);
    munit_assert_int(context->compiler->global_storage->constants.length, ==, 2);

    munit_assert_double(valueToNumber(context->compiler->global_storage->constants.data[0]), ==, 1024.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->constants.data[1]), ==, 2048.0);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 2048.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 1024.0);


    brama_vm_const_type type;
    void*               data;
    brama_status status = brama_get_var(context, "test1", &type, &data);

    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (type,             == , CONST_INTEGER);
    munit_assert_double(*((double*)data), == , 2048.0);


    status = brama_get_var(context, "test2", &type, &data);

    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (type,             == , CONST_INTEGER);
    munit_assert_double(*((double*)data), == , 1024.0);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var test_1 = 10; var test_2 = 20; var test_3 = test_1 < test_2;");
    brama_run(context);
    size_t opcode_index = 0;
    munit_assert_int(context->compiler->op_codes->length, == , 4);

    t_brama_vmdata vmdata;

    vm_decode(context->compiler->op_codes->data[0], &vmdata);
    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 1);
    munit_assert_int(vmdata.reg2, ==, -1);
    munit_assert_int(vmdata.reg3, ==, 0);

    vm_decode(context->compiler->op_codes->data[1], &vmdata);
    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 2);
    munit_assert_int(vmdata.reg2, ==, -2);
    munit_assert_int(vmdata.reg3, ==, 0);

    vm_decode(context->compiler->op_codes->data[2], &vmdata);
    munit_assert_int(vmdata.op,   ==, VM_OPT_LT);
    munit_assert_int(vmdata.reg1, ==, 3);
    munit_assert_int(vmdata.reg2, ==, 1);
    munit_assert_int(vmdata.reg3, ==, 2);

    munit_assert_int(context->compiler->global_storage->variables.length, ==, 3);
    munit_assert_int(context->compiler->global_storage->constants.length, ==, 2);

    munit_assert_double(valueToNumber(context->compiler->global_storage->constants.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->constants.data[1]), ==, 20.0);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 20.0);
    munit_assert_double(AS_BOOL(context->compiler->global_storage->variables.data[2]),       ==, true);


    brama_vm_const_type type;
    void*               data;
    brama_status status = brama_get_var(context, "test_1", &type, &data);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (type,             == , CONST_INTEGER);
    munit_assert_double(*((double*)data), == , 10.0);


    status = brama_get_var(context, "test_2", &type, &data);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (type,             == , CONST_INTEGER);
    munit_assert_double(*((double*)data), == , 20.0);


    status = brama_get_var(context, "test_3", &type, &data);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (type,             == , CONST_BOOL);
    munit_assert_int   (*((bool*)data),   == , true);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var test_1 = 10; "
                           "var test_2 = 20; "
                           "var test_3 = test_1 < test_2; "
                           "if (test_3) "
                           "    test_1 = 1; "
                           "else "
                           "    test_1 = 0"
                           "test_2 = 200000.111");
    brama_run(context);

    brama_vm_const_type type;
    void*               data;
    brama_status status = brama_get_var(context, "test_1", &type, &data);
    munit_assert_int   (status,            ==, BRAMA_OK);
    munit_assert_int   (type,              ==, CONST_INTEGER);
    munit_assert_double(*((double *)data), ==, 1);

    status = brama_get_var(context, "test_2", &type, &data);
    munit_assert_int   (status,            ==, BRAMA_OK);
    munit_assert_int   (type,              ==, CONST_INTEGER);
    munit_assert_double(*((double *)data), ==, 200000.111);

    status = brama_get_var(context, "test_3", &type, &data);
    munit_assert_int   (status,          ==, BRAMA_OK);
    munit_assert_int   (type,            ==, CONST_BOOL);
    munit_assert_double(*((bool *)data), ==, true);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var x = 0; var y = 1;\n"
                           "var count = 2 ;\n"
                           "var fib ;\n"
                           "while(count <=10){\n"
                           "    var fib = x+y ;\n"
                           "    x = y ;\n"
                           "    y = fib;\n"
                           "count++\n"
                           "}");
    brama_run(context);

    brama_vm_const_type type;
    void*               data;
    brama_status status = brama_get_var(context, "fib", &type, &data);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (type,             == , CONST_INTEGER);
    munit_assert_double(*((double *)data),   == , 55);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var test1 = 10; var test2 = -test1;");
    brama_run(context);

    brama_vm_const_type type;
    void*               data;
    brama_status status = brama_get_var(context, "test2", &type, &data);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (type,             == , CONST_INTEGER);
    munit_assert_double(*((double *)data),   == , -10.0);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_7(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var test = 'ERHAN BARIS' + ' AYSEL BARIS'");
    brama_run(context);

    brama_vm_const_type type;
    void*               data;
    brama_status status = brama_get_var(context, "test", &type, &data);
    munit_assert_int         (status,           == , BRAMA_OK);
    munit_assert_int         (type,             == , CONST_STRING);
    munit_assert_string_equal(((char_ptr)data),   "ERHAN BARIS AYSEL BARIS");
    BRAMA_FREE(((char_ptr)data));

    brama_destroy(context);
    return MUNIT_OK;
}

MunitTest COMPILE_TESTS[] = {

        ADD_TEST(ast_compile_1),
        ADD_TEST(ast_compile_2),
        ADD_TEST(ast_compile_3),
        ADD_TEST(ast_compile_4),
        ADD_TEST(ast_compile_5),
        ADD_TEST(ast_compile_6),
        ADD_TEST(ast_compile_7),

        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_COMPILE_H
