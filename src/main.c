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
    brama_execute(context,  "var person = {};\n"
                            "person['firstname'] = 'Mario';\n"
                            "person['lastname'] = 'Rossi';\n"
                            "\n"
                            "console.log(person.firstname);\n"
                            "// expected output: \"Mario\"\n"
                            "\n"
                            "person = {'firstname': 'John', 'lastname': 'Doe'}\n"
                            "\n"
                            "console.log(person['lastname']);\n"
                            "// expected output: \"Doe\"");
    munit_assert_int(context->status, == , BRAMA_OK);

    brama_destroy(context);

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
