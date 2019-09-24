#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

#include "tests.h"
#include "munit.h"

#include "static_py.h"

int main(int argc, const char* argv[])
{
    return munit_suite_main(&TOKEN_SUITE, (void*) "µnit", argc, argv);
}
