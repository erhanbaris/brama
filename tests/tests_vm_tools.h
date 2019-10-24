#ifndef TESTS_VM_TOOLS_H
#define TESTS_VM_TOOLS_H

#include "tests_core.h"
#include "tests_token.h"
#include "tools.h"

MunitResult encode_decode_1(const MunitParameter params[], void* user_data_or_fixture) {
    
    t_brama_vmdata vmdata;
    t_brama_vmdata vmdata_decoded;
    vmdata.op   = VM_OPT_BITWISE_OR;
    vmdata.reg1 = 1;
    vmdata.reg2 = 2;
    vmdata.reg3 = 3;
    vmdata.scal = 0;

    t_brama_byte code = vm_encode(&vmdata);
    vm_decode(code, &vmdata_decoded);
    munit_assert_int(vmdata_decoded.op,   ==, vmdata.op);
    munit_assert_int(vmdata_decoded.reg1, ==, vmdata.reg1);
    munit_assert_int(vmdata_decoded.reg2, ==, vmdata.reg2);
    munit_assert_int(vmdata_decoded.reg3, ==, vmdata.reg3);

    return MUNIT_OK;
}

MunitResult encode_decode_2(const MunitParameter params[], void* user_data_or_fixture) {
    
    t_brama_vmdata vmdata;
    t_brama_vmdata vmdata_decoded;
    vmdata.op   = VM_OPT_APPEND;
    vmdata.reg1 = 1;
    vmdata.scal = 60;

    t_brama_byte code = vm_encode(&vmdata);
    vm_decode(code, &vmdata_decoded);
    munit_assert_int(vmdata_decoded.op,   ==, vmdata.op);
    munit_assert_int(vmdata_decoded.scal, ==, vmdata.scal);

    return MUNIT_OK;
}

MunitTest VM_TOOLS_TESTS[] = {
    ADD_TEST(encode_decode_1),
    ADD_TEST(encode_decode_2),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_VM_TOOLS_H
