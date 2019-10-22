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
    brama_execute(context, "_ten - _twelve");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->opt, == , OPERATOR_SUBTRACTION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);
    munit_assert_int         (ast->binary_ptr->left->type, == , AST_SYMBOL);
    munit_assert_int         (ast->binary_ptr->right->type, == , AST_SYMBOL);
    munit_assert_string_equal(ast->binary_ptr->left->char_ptr, "_ten");
    munit_assert_string_equal(ast->binary_ptr->right->char_ptr, "_twelve");
    destroy_ast(ast);
    BRAMA_FREE(ast);

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
