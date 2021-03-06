#ifndef TESTS_COMPILE_H
#define TESTS_COMPILE_H

#include <brama.h>
#include <math.h>
#include <float.h>
#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"

MunitResult ast_compile_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "10 + 20");
    brama_run(context);
    munit_assert_int(context->compiler->op_codes->length, == , 2);
    munit_assert_int(context->compiler->op_codes->data[1], == , NULL);

    munit_assert_int(context->status, == , BRAMA_OK);

    t_brama_vmdata vmdata;
    vm_decode(context->compiler->op_codes->data[0], vmdata);

    munit_assert_int(vmdata.op, ==, VM_OPT_ADDITION);
    munit_assert_int(context->compiler->global_storage->variables.length, ==, 3);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 20.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[2]), ==, 30.0);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var test1; var test2 = 1024; test1 = 2048;");
    brama_run(context);

    munit_assert_int(context->compiler->op_codes->length, == , 3);

    t_brama_vmdata vmdata;
    vm_decode(context->compiler->op_codes->data[0], vmdata);

    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 3);
    munit_assert_int(vmdata.reg2, ==, 0);
    munit_assert_int(vmdata.reg3, ==, 0);

    vm_decode(context->compiler->op_codes->data[1], vmdata);

    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 4);
    munit_assert_int(vmdata.reg2, ==, 1);
    munit_assert_int(vmdata.reg3, ==, 0);

    munit_assert_int(context->compiler->global_storage->variables.length, ==, 5);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 1024);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 2048);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[4]), ==, 2048);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[3]), ==, 1024);


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
    t_context* context = brama_init(0);
    brama_compile(context, "var test_1 = 10; var test_2 = 20; var test_3 = test_1 < test_2;");
    brama_run(context);
    munit_assert_int(context->compiler->op_codes->length, == , 4);

    t_brama_vmdata vmdata;

    vm_decode(context->compiler->op_codes->data[0], vmdata);
    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 5);
    munit_assert_int(vmdata.reg2, ==, 0);
    munit_assert_int(vmdata.reg3, ==, 0);

    vm_decode(context->compiler->op_codes->data[1], vmdata);
    munit_assert_int(vmdata.op,   ==, VM_OPT_INIT_VAR);
    munit_assert_int(vmdata.reg1, ==, 3);
    munit_assert_int(vmdata.reg2, ==, 1);
    munit_assert_int(vmdata.reg3, ==, 0);

    vm_decode(context->compiler->op_codes->data[2], vmdata);
    munit_assert_int(vmdata.op,   ==, VM_OPT_LT);
    munit_assert_int(vmdata.reg1, ==, 4);
    munit_assert_int(vmdata.reg2, ==, 5);
    munit_assert_int(vmdata.reg3, ==, 3);

    munit_assert_int(context->compiler->global_storage->variables.length, ==, 7);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 20.0);

    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[0]), ==, 10.0);
    munit_assert_double(valueToNumber(context->compiler->global_storage->variables.data[1]), ==, 20.0);
    munit_assert_double(AS_BOOL(context->compiler->global_storage->variables.data[4]),       ==, true);


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
    t_context* context = brama_init(0);
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
    t_context* context = brama_init(0);
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
    t_context* context = brama_init(0);
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
    t_context* context = brama_init(0);
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
    t_context* context = brama_init(0);
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

MunitResult ast_compile_9(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var x = 10\r\n"
                           "var a = ++x;\r\n"
                           "var b = x++;\r\n");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "x", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, 12.0);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "a", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, 11.0);
    brama_destroy_get_var(context, &var_info);

    status = brama_get_var(context, "b", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_double(var_info->double_, ==, 11.0);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_10(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var a = (2<<2);\r\n"
                           "var b = (16>>3);\r\n"
                           "var c = (-1 >>> 16);\r\n"
                           "result = a==8 && b==2 && c == 0xFFFF;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_BOOL);
    munit_assert_int   (var_info->bool_,== , true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_11(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var a = 42;\n"
                           "if (a != 42)\n"
                           "  result = 0;\n"
                           "else\n"
                           "  result = 1;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 1.0);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_12(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "// Number definition from http://en.wikipedia.org/wiki/JavaScript_syntax\n"
                           "a = 345;    // an \"integer\", although there is only one numeric type in JavaScript\n"
                           "b = 34.5;   // a floating-point number\n"
                           "c = 3.45e2; // another floating-point, equivalent to 345\n"
                           "d = 0377;   // an octal integer equal to 255\n"
                           "e = 0xFF;   // a hexadecimal integer equal to 255, digits represented by the letters A-F may be upper or lowercase\n"
                           "\n"
                           "result = a==345 && b*10==345 && c==345 && d==255 && e==255;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_BOOL);
    munit_assert_int   (var_info->bool_, == , true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_13(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var test = 'erhan' + undefined; // Undefined/null from http://en.wikipedia.org/wiki/JavaScript_syntax\n"
                           "var testUndefined;        // variable declared but not defined, set to value of undefined\n"
                           "\n"
                           "result = 1;\n"
                           "if ((\"\"+testUndefined) != \"undefined\") result = 0; // test variable exists but value not defined, displays undefined\n"
                           "if (!(undefined == null)) result = 0;  // unenforced type during check, displays true\n"
                           "if (undefined === null) result = 0;// enforce type during check, displays false\n"
                           "\n"
                           "\n"
                           "if (null != undefined) result = 0;  // unenforced type during check, displays true\n"
                           "if (null === undefined) result = 0; // enforce type during check, displays false");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 1.0);
    brama_destroy_get_var(context, &var_info);
    return MUNIT_OK;
}


MunitResult ast_compile_14(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "function fib(n) { "
"  if (n < 2){ "
"    return n "
"  } var p1 =  fib(n - 1) ; var p2 =  fib(n - 2) ; "
"  return p1 + p2 "
"}; var index = 0; var total = 0; while (100 > index) { ++index; total += fib(10); }");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "total", &var_info);

    munit_assert_int(status,         == , BRAMA_OK);
    munit_assert_int(var_info->type, == , CONST_INTEGER);
    munit_assert_int(var_info->double_, == , 5500);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_15(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "function fib(n) { "
"  if (n < 2){ "
"    return n "
"  } "
"  return fib(n - 1) + fib(n - 2) "
"}; var index = 0; var total = 0; while (100 > index) { ++index; total += fib(10); }");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "total", &var_info);

    munit_assert_int(status,         == , BRAMA_OK);
    munit_assert_int(var_info->type, == , CONST_INTEGER);
    munit_assert_int(var_info->double_, == , 5500);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_16(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var a = 0;\r\n"
    "var i;\r\n"
    "for (i=1;i<10;i++) a = a + i;\r\n"
    "result = a==45;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_BOOL);
    munit_assert_int   (var_info->bool_,== , true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_17(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var a = 0;\r\n"
                           "for (var i=1;i<10;i++) a += i;\r\n"
                           "result = a==45;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_BOOL);
    munit_assert_int   (var_info->bool_,== , true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_18(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "function add(x,y) { return x+y; }\n"
                           "result = add(3,6)==9;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_BOOL);
    munit_assert_int   (var_info->bool_,== , true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_19(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var a = 7;\n"
                           "function add(x,y) { var a=x+y; return a; }\n"
                           "result = add(3,6)==9 && a==7;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int   (status,           == , BRAMA_OK);
    munit_assert_int   (var_info->type,   == , CONST_BOOL);
    munit_assert_int   (var_info->bool_,== , true);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_20(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var bob = {};");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "bob", &var_info);
    munit_assert_int     (status,           == , BRAMA_OK);
    munit_assert_int     (var_info->type,   == , CONST_DICT);
    munit_assert_not_null(var_info->dict_);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_21(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var bob = {}; bob['test'] = 1024;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "bob", &var_info);
    munit_assert_int     (status,           == , BRAMA_OK);
    munit_assert_int     (var_info->type,   == , CONST_DICT);
    munit_assert_not_null(var_info->dict_);

    t_brama_value saved_value =  *map_get(var_info->dict_, "test");
    munit_assert_true(IS_NUM(saved_value));
    munit_assert_int(valueToNumber(saved_value), ==, 1024);

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_22(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var bob = {};"
    "bob['add'] = function(x,y) { return x+y; }; var result_1 = bob['add'](5,3); var result_2 = bob['add']('erhan', ' baris')");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result_1", &var_info);
    munit_assert_int     (status,            == , BRAMA_OK);
    munit_assert_int     (var_info->type,    == , CONST_INTEGER);
    munit_assert_int     (var_info->double_, == , 8);

    brama_destroy_get_var(context, &var_info);

    status = brama_get_var   (context, "result_2", &var_info);
    munit_assert_int         (status,            == , BRAMA_OK);
    munit_assert_int         (var_info->type,    == , CONST_STRING);
    munit_assert_string_equal(var_info->char_ptr, "erhan baris");

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_23(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var bob = {};"
    "bob.add = function(x,y) { return x+y; }; var result_1 = bob.add(5,3); var result_2 = bob.add('erhan', ' baris')");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result_1", &var_info);
    munit_assert_int     (status,            == , BRAMA_OK);
    munit_assert_int     (var_info->type,    == , CONST_INTEGER);
    munit_assert_int     (var_info->double_, == , 8);

    brama_destroy_get_var(context, &var_info);

    status = brama_get_var   (context, "result_2", &var_info);
    munit_assert_int         (status,            == , BRAMA_OK);
    munit_assert_int         (var_info->type,    == , CONST_STRING);
    munit_assert_string_equal(var_info->char_ptr, "erhan baris");

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_24(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var bob = { add : function(x,y) { return x+y; } }; "
                           "result = bob.add(3,6)==9;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int     (status,          == , BRAMA_OK);
    munit_assert_int     (var_info->type,  == , CONST_BOOL);
    munit_assert_int     (var_info->bool_, == , true);

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_25(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "function a(x) { return x+2; }\n"
                           "function b(x) { return a(x)+1; }\n"
                           "result = a(3)==5 && b(3)==6;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int     (status,          == , BRAMA_OK);
    munit_assert_int     (var_info->type,  == , CONST_BOOL);
    munit_assert_int     (var_info->bool_, == , true);

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_26(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "function a(x) { \n"
                           "  if (x>1)\n"
                           "    return x*a(x-1);\n"
                           "  return 1;\n"
                           "}\n"
                           "result = a(5)==1*2*3*4*5;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int     (status,          == , BRAMA_OK);
    munit_assert_int     (var_info->type,  == , CONST_BOOL);
    munit_assert_int     (var_info->bool_, == , true);

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_27(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "// Variable creation and scope from http://en.wikipedia.org/wiki/JavaScript_syntax\n"
                           "x = 0; // A global variable\n"
                           "var y = 'Hello!'; // Another global variable\n"
                           "z = 0; // yet another global variable\n"
                           "\n"
                           "function f(){\n"
                           "  var z = 'foxes'; // A local variable\n"
                           "  twenty = 20; // Global because keyword var is not used\n"
                           "  return x; // We can use x here because it is global\n"
                           "}\n"
                           "// The value of z is no longer available\n"
                           "\n"
                           "\n"
                           "// testing\n"
                           "blah = f();\n"
                           "result = blah==0 && z!='foxes' && twenty==20;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int     (status,            == , BRAMA_OK);
    munit_assert_int     (var_info->type,    == , CONST_BOOL);
    munit_assert_int     (var_info->bool_, == , true);

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_28(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var a = []; \r\n"
                           "a[0] = 10; \r\n"
                           "a[1] = 22; \r\n"
                           "b = a; \r\n"
                           "b[0] = 5; \r\n"
                           "result = a[0]==5 && a[1]==22 && b[1]==22;");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "result", &var_info);
    munit_assert_int     (status,            == , BRAMA_OK);
    munit_assert_int     (var_info->type,    == , CONST_BOOL);
    munit_assert_int     (var_info->bool_,   == , true);

    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_29(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var a = 42;\n"
                           "var b = [];\n"
                           "b[0] = 43;\n"
                           "\n"
                           "function foo(myarray) {\n"
                           "  myarray[0]++;\n"
                           "}\n"
                           "\n"
                           "function bar(myvalue) {\n"
                           "  myvalue++;\n"
                           "}\n"
                           "\n"
                           "foo(b);\n"
                           "bar(a);\n"
                           "\n"
                           "result = a==42 && b[0]==44;");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,         == , BRAMA_OK);
    munit_assert_int(var_info->type, == , CONST_BOOL);
    munit_assert_int(var_info->bool_,== , true);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

BOOL_FALSE_TEST(ast_compile_30, "var result = Number.isFinite(1/0)")
BOOL_TRUE_TEST(ast_compile_31,  "var result = Number.isFinite(10/5)")
BOOL_FALSE_TEST(ast_compile_32, "var result = Number.isFinite(0/0)")
BOOL_TRUE_TEST(ast_compile_33, "var result = Number.isNaN(0 / 0)")
BOOL_FALSE_TEST(ast_compile_34, "var result = Number.isNaN('NaN')")
BOOL_FALSE_TEST(ast_compile_35, "var result = Number.isNaN(undefined)")
BOOL_FALSE_TEST(ast_compile_36, "var result = Number.isNaN({})")
BOOL_FALSE_TEST(ast_compile_37, "var result = Number.isNaN(true)")
BOOL_FALSE_TEST(ast_compile_38, "var result = Number.isNaN(37)")
BOOL_FALSE_TEST(ast_compile_39, "var result = Number.isInteger('10')")
BOOL_FALSE_TEST(ast_compile_40, "var result = Number.isInteger(true)")
BOOL_FALSE_TEST(ast_compile_41, "var result = Number.isInteger(false)")
BOOL_FALSE_TEST(ast_compile_42, "var result = Number.isInteger([1])")
BOOL_TRUE_TEST(ast_compile_43, "var result = Number.isInteger(5.0)")
BOOL_FALSE_TEST(ast_compile_44, "var result = Number.isInteger(5.000000000000001)")
BOOL_TRUE_TEST(ast_compile_45, "var result = Number.isInteger(5.0000000000000001)")
BOOL_FALSE_TEST(ast_compile_46, "var result = isFinite(1/0)")
BOOL_TRUE_TEST(ast_compile_47,  "var result = isFinite(10/5)")
BOOL_FALSE_TEST(ast_compile_48, "var result = isFinite(0/0)")
BOOL_TRUE_TEST(ast_compile_49, "var result = isNaN(0 / 0)")
BOOL_FALSE_TEST(ast_compile_50, "var result = isNaN('NaN')")
BOOL_FALSE_TEST(ast_compile_51, "var result = isNaN(undefined)")
BOOL_FALSE_TEST(ast_compile_52, "var result = isNaN({})")
BOOL_FALSE_TEST(ast_compile_53, "var result = isNaN(true)")
BOOL_FALSE_TEST(ast_compile_54, "var result = isNaN(37)")

MunitResult ast_compile_55(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = parseFloat();");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,         == , BRAMA_OK);
    munit_assert_int(var_info->type, == , CONST_DOUBLE);
    munit_assert_int(var_info->double_,== , NAN);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_56(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = Number.parseFloat()");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,         == , BRAMA_OK);
    munit_assert_int(var_info->type, == , CONST_DOUBLE);
    munit_assert_int(var_info->double_,== , NAN);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_57(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = parseFloat('3.14')");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,           == , BRAMA_OK);
    munit_assert_int(var_info->type,   == , CONST_DOUBLE);
    munit_assert_true(fabs(var_info->double_ - 3.14) < FLT_EPSILON);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_58(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = parseFloat('    3.14   ')");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,           == , BRAMA_OK);
    munit_assert_int(var_info->type,   == , CONST_DOUBLE);
    munit_assert_true(fabs(var_info->double_ - 3.14) < FLT_EPSILON);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_59(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = parseFloat('    3.14  3 ')");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,           == , BRAMA_OK);
    munit_assert_int(var_info->type,   == , CONST_DOUBLE);
    munit_assert_true(fabs(var_info->double_ - 3.14) < FLT_EPSILON);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_60(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = parseFloat('314e-2')");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,           == , BRAMA_OK);
    munit_assert_int(var_info->type,   == , CONST_DOUBLE);
    munit_assert_true(fabs(var_info->double_ - 3.14) < FLT_EPSILON);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_61(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = parseFloat('0.0314E+2')");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,           == , BRAMA_OK);
    munit_assert_int(var_info->type,   == , CONST_DOUBLE);
    munit_assert_true(fabs(var_info->double_ - 3.14) < FLT_EPSILON);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_compile_62(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var result = parseFloat('3.14some non-digit characters')");
    brama_run(context);
    t_get_var_info_ptr var_info = NULL;
    brama_status status         = brama_get_var(context, "result", &var_info);

    munit_assert_int(status,           == , BRAMA_OK);
    munit_assert_int(var_info->type,   == , CONST_DOUBLE);
    munit_assert_true(fabs(var_info->double_ - 3.14) < FLT_EPSILON);
    brama_destroy_get_var(context, &var_info);
    brama_destroy(context);
    return MUNIT_OK;
}

BOOL_TRUE_TEST(ast_compile_63,  "var result = Number.parseInt(-15.1) == -15;")
BOOL_TRUE_TEST(ast_compile_64,  "var result = Number.parseInt(123.456) == 123;")

MunitTest COMPILE_TESTS[] = {

        ADD_TEST(ast_compile_1),
        ADD_TEST(ast_compile_2),
        ADD_TEST(ast_compile_3),
        ADD_TEST(ast_compile_4),
        ADD_TEST(ast_compile_5),
        ADD_TEST(ast_compile_6),
        ADD_TEST(ast_compile_7),
        ADD_TEST(ast_compile_8),
        ADD_TEST(ast_compile_9),
        ADD_TEST(ast_compile_10),
        ADD_TEST(ast_compile_11),
        ADD_TEST(ast_compile_12),
        ADD_TEST(ast_compile_13),
        ADD_TEST(ast_compile_14),
        ADD_TEST(ast_compile_15),
        ADD_TEST(ast_compile_16),
        ADD_TEST(ast_compile_17),
        ADD_TEST(ast_compile_18),
        ADD_TEST(ast_compile_19),
        ADD_TEST(ast_compile_20),
        ADD_TEST(ast_compile_21),
        ADD_TEST(ast_compile_22),
        ADD_TEST(ast_compile_23),
        ADD_TEST(ast_compile_24),
        ADD_TEST(ast_compile_25),
        ADD_TEST(ast_compile_26),
        ADD_TEST(ast_compile_27),
        ADD_TEST(ast_compile_28),
        ADD_TEST(ast_compile_29),
        ADD_TEST(ast_compile_30),
        ADD_TEST(ast_compile_31),
        ADD_TEST(ast_compile_32),
        ADD_TEST(ast_compile_33),
        ADD_TEST(ast_compile_34),
        ADD_TEST(ast_compile_35),
        ADD_TEST(ast_compile_36),
        ADD_TEST(ast_compile_37),
        ADD_TEST(ast_compile_38),
        ADD_TEST(ast_compile_39),
        ADD_TEST(ast_compile_40),
        ADD_TEST(ast_compile_41),
        ADD_TEST(ast_compile_42),
        ADD_TEST(ast_compile_43),
        ADD_TEST(ast_compile_44),
        ADD_TEST(ast_compile_45),

        ADD_TEST(ast_compile_46),
        ADD_TEST(ast_compile_47),
        ADD_TEST(ast_compile_48),
        ADD_TEST(ast_compile_49),
        ADD_TEST(ast_compile_50),
        ADD_TEST(ast_compile_51),
        ADD_TEST(ast_compile_52),
        ADD_TEST(ast_compile_53),
        ADD_TEST(ast_compile_54),
        ADD_TEST(ast_compile_55),
        ADD_TEST(ast_compile_56),
        ADD_TEST(ast_compile_57),
        ADD_TEST(ast_compile_58),
        ADD_TEST(ast_compile_59),
        ADD_TEST(ast_compile_60),
        ADD_TEST(ast_compile_61),
        ADD_TEST(ast_compile_62),
        ADD_TEST(ast_compile_63),
        ADD_TEST(ast_compile_64),

        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_COMPILE_H
