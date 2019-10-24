#ifndef TESTS_H
#define TESTS_H

#include "tests_token.h"
#include "tests_tools.h"
#include "tests_ast.h"
#include "tests_ast_2.h"
#include "tests_bitwise.h"
#include "tests_compile.h"
#include "tests_vm_tools.h"

static MunitSuite other_suites[] = {
  { "/vm-tests",       VM_TOOLS_TESTS,NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/token-tests",   TOKEN_TESTS,   NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/tools-tests",   TOOLS_TESTS,   NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/ast-tests",     AST_TESTS,     NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/ast-tests-2",   AST_TESTS_2,   NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/bitwise-tests", BITWISE_TESTS, NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/compile-tests", COMPILE_TESTS, NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
 };

static const MunitSuite ALL_SUITE = { "", NULL, other_suites, 1, MUNIT_SUITE_OPTION_NONE };

#endif // TESTS_H
