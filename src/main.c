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
    brama_compile(context, "var test = 'ERHAN BARIS' + ' AYSEL BARIS'"); /* -6.66666666667 */
    brama_run(context);

    brama_vm_const_type type;
    void*               data;
    brama_status status = brama_get_var(context, "test", &type, &data);
    //printf("test : %f\r\n", *((double*)data));

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
