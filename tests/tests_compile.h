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
    munit_assert_int(context->compiler->global_storage->variables.length, ==, 3);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 20.0);

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

    munit_assert_int(context->compiler->global_storage->variables.length, ==, 4);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 1024.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 2048.0);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 2048.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 1024.0);


    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "test1", &var_info);

    munit_assert_int(status,         == , BRAMA_OK);
    munit_assert_int(var_info->type, == , CONST_INTEGER);
    munit_assert_int(var_info->double_, == , 2048);
    brama_destroy_get_var(context, &var_info);

    munit_assert_ptr_null(var_info);
    status = brama_get_var(context, "test2", &var_info);

    munit_assert_int(status,         == , BRAMA_OK);
    munit_assert_int(var_info->type, == , CONST_INTEGER);
    munit_assert_int(var_info->double_, == , 1024);
    brama_destroy_get_var(context, &var_info);

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

    munit_assert_int(context->compiler->global_storage->variables.length, ==, 5);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 20.0);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 20.0);
    munit_assert_double(AS_BOOL(context->compiler->global_storage->variables.data[2]),       ==, true);


    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "test_1", &var_info);
    munit_assert_int   (status,         == , BRAMA_OK);
    munit_assert_int   (var_info->type, == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 10);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "test_2", &var_info);
    munit_assert_int   (status,         == , BRAMA_OK);
    munit_assert_int   (var_info->type, == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 20);
    brama_destroy_get_var(context, &var_info);


    status = brama_get_var(context, "test_3", &var_info);
    munit_assert_int   (status,          == , BRAMA_OK);
    munit_assert_int   (var_info->type,  == , CONST_BOOL);
    munit_assert_int   (var_info->bool_, == , true);
    brama_destroy_get_var(context, &var_info);

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

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "test_1", &var_info);
    munit_assert_int   (status,         ==, BRAMA_OK);
    munit_assert_int   (var_info->type, ==, CONST_INTEGER);
    munit_assert_int   (var_info->double_, ==, 1);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "test_2", &var_info);
    munit_assert_int   (status,            ==, BRAMA_OK);
    munit_assert_int   (var_info->type,    ==, CONST_DOUBLE);
    munit_assert_double(var_info->double_, ==, 200000.111);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "test_3", &var_info);
    munit_assert_int   (status,          ==, BRAMA_OK);
    munit_assert_int   (var_info->type,  ==, CONST_BOOL);
    munit_assert_double(var_info->bool_, ==, true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var x = 0; var y = 1;\n"
                           "var count = 2 ;\n"
                           "var fib ;\n"
                           "while(count <=10){\n"
                           "    fib = x+y ;\n"
                           "    x = y ;\n"
                           "    y = fib;\n"
                           "count++\n"
                           "}");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "fib", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_INTEGER);
    munit_assert_int   (var_info->double_,== , 55);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var test1 = 10; var test2 = -test1;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "test2", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_INTEGER);
    munit_assert_int   (var_info->double_,== , -10.0);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_7(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var test = 'ERHAN BARIS' + ' AYSEL BARIS'");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "test", &var_info);
    munit_assert_int         (status,             == , BRAMA_OK);
    munit_assert_int         (var_info->type,     == , CONST_STRING);
    munit_assert_string_equal(var_info->char_ptr, "ERHAN BARIS AYSEL BARIS");
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var x = 9007199254740992;\r\n"
                           "var y = -x;\r\n"
                           "var a = x == x + 1;\r\n"
                           "var b = y == y - 1;\r\n"
                           "var c = x / 2;\r\n"
                           "var d = x >> 1;\r\n"
                           "var e = x | 1;\r\n");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "x", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, 9007199254740992.0);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "y", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, -9007199254740992.0);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "a", &var_info);
    munit_assert_int   (status,          == , BRAMA_OK);
    munit_assert_int   (var_info->type,  == , CONST_BOOL);
    munit_assert_double(var_info->bool_, ==, true);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "b", &var_info);
    munit_assert_int   (status,          == , BRAMA_OK);
    munit_assert_int   (var_info->type,  == , CONST_BOOL);
    munit_assert_double(var_info->bool_, ==, true);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "c", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, 4503599627370496.0);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "d", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, 0.0);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "e", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, 1.0);
    brama_destroy_get_var(context, &var_info);

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
        ADD_TEST(ast_compile_8),

        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_COMPILE_H
