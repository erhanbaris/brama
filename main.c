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
    t_context* context = static_py_init();
    static_py_execute(context, "+ - * / % ++ --");
    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    static_py_destroy(context);

    return munit_suite_main(&TOKEN_SUITE, (void*) "µnit", NULL, NULL);
}
