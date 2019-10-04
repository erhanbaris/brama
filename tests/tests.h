#ifndef TESTS_H
#define TESTS_H

#include "tests_token.h"
#include "tests_tools.h"
#include "tests_ast.h"

static MunitSuite other_suites[] = {
   { "/token-tests",  TOKEN_TESTS, NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/tools-tests",  TOOLS_TESTS, NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { "/ast-tests",    AST_TESTS,   NULL, 1, MUNIT_SUITE_OPTION_NONE },
   { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
 };

static const MunitSuite ALL_SUITE = { "", NULL, other_suites, 1, MUNIT_SUITE_OPTION_NONE };

#endif // TESTS_H
