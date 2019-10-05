#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>

#include "tests.h"
#include "munit.h"

#include "brama.h"

int main(int argc, const char* argv[]) {
    t_context* context = brama_init();
    brama_execute(context,  "var obj = new test()");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_ASSIGNMENT);
    munit_assert_ptr_not_null(ast->assign_ptr);
    munit_assert_string_equal(ast->assign_ptr->symbol, "obj");
    munit_assert_ptr_not_null(ast->assign_ptr->def_type);
    munit_assert_ptr_not_null(ast->assign_ptr->assignment);
    munit_assert_ptr_not_null(ast->assign_ptr->assignment->object_creation_ptr);
    munit_assert_string_equal(ast->assign_ptr->assignment->object_creation_ptr->object_name, "test");
    munit_assert_int         (ast->assign_ptr->assignment->object_creation_ptr->args->count, ==, 0);

    brama_destroy(context);
#ifdef _WIN32
    int test_status = munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
    getchar();
    return test_status;
#else
    return munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
#endif
}
