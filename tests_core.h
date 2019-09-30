#ifndef TESTS_CORE_H
#define TESTS_CORE_H

#include "munit.h"

#define ADD_TEST(TEST)  { "/" #TEST, TEST, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

#endif // TESTS_CORE_H
