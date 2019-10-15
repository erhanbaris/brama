#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>

#include <assert.h>
#include <stdint.h>

#include "tests.h"
#include "munit.h"

#include "brama.h"

int main(int argc, const char* argv[]) {
    t_context* context = brama_init();
    //brama_execute(context, "findSolutions((ld | bit) << 1, cols | bit, (rd | bit) >> 1, all)");

    brama_execute(context, "findSolutions(e.data[0], e.data[1], e.data[2], e.data[3]);");
    
    
    brama_destroy(context);

#ifdef _WIN32
    int test_status = munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
    getchar();
    return test_status;
#else
    return 0;
#endif
}
