#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>

#include <assert.h>
#include <stdint.h>

#include "tests.h"
#include "munit.h"
#include <float.h>

#include "brama.h"

int main(int argc, const char* argv[]) {
    t_context* context = brama_init();
    brama_compile(context, "function fib(n) { "
"  if (n < 2){ "
"    return n "
"  } "
"  return fib(n - 1) + fib (n - 2) "
"}; var test = fib(10);");
    brama_run(context);
    brama_compile_dump(context);

    brama_destroy(context);
    getchar();
    return 0;
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
