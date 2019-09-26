# include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

#include "tests.h"
#include "munit.h"

#define STATIC_PY_VERBOSE_MODE 1

#include "static_py.h"


int main(int argc, const char* argv[])
{
    t_vector* vector = vector_init();
    assert(vector->length == 32);
    assert(vector->count == 0);
    assert(vector->data != NULL);
    vector_destroy(vector);

    return munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
}
