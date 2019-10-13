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
    brama_execute(context,  "var test = 1;\n"
                            "if (test == -1)\n"
                            "    console.log(\"Test is -1\");\n"
                            "else if (test == 0)\n"
                            "    console.log(\"Test is 0\");\n"
                            "else\n"
                            "    console.log(\"Test is 1\");");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type,                                 ==, AST_ASSIGNMENT);
    CLEAR_AST(ast);

    ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type,                                 ==, AST_IF_STATEMENT);
    munit_assert_ptr_not_null(ast->if_stmt_ptr);
    munit_assert_ptr_not_null(ast->if_stmt_ptr->condition);
    munit_assert_int         (ast->if_stmt_ptr->condition->type, ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->left->type,  ==, AST_SYMBOL);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->right->type, ==, AST_UNARY);

    munit_assert_ptr_not_null(ast->if_stmt_ptr->true_body);
    munit_assert_int         (ast->if_stmt_ptr->true_body->type, ==, AST_FUNCTION_CALL);

    munit_assert_ptr_not_null(ast->if_stmt_ptr->false_body);
    munit_assert_int         (ast->if_stmt_ptr->false_body->type, ==, AST_IF_STATEMENT);
    munit_assert_ptr_not_null(ast->if_stmt_ptr->false_body->if_stmt_ptr->condition);
    munit_assert_int         (ast->if_stmt_ptr->false_body->if_stmt_ptr->condition->type, ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->if_stmt_ptr->false_body->if_stmt_ptr->condition->control_ptr->left->type,  ==, AST_SYMBOL);
    munit_assert_int         (ast->if_stmt_ptr->false_body->if_stmt_ptr->condition->control_ptr->right->type, ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->if_stmt_ptr->false_body->if_stmt_ptr->false_body);

    CLEAR_AST(ast);

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
