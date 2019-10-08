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
    brama_execute(context,  "while ((elem = document.getElementById('id-' + a)) !== null) {\n"
                            "    //Some code\n"
                            "    a++;\n"
                            "}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type,                               ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type,              ==, AST_BLOCK);
    munit_assert_int         (ast->while_ptr->body->vector_ptr->count, ==, 1);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->type,                    ==, AST_UNARY);
    munit_assert_ptr_not_null(((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->unary_ptr);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->unary_ptr->opt,          ==, OPERATOR_INCREMENT);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->unary_ptr->operand_type, ==, UNARY_OPERAND_AFTER);
    munit_assert_ptr_not_null(((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->unary_ptr->content);
    munit_assert_string_equal(((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->unary_ptr->content->char_ptr, "a");
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type,                    ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->opt,        ==, OPERATOR_NOT_EQUAL_VALUE);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->type, ==, AST_ASSIGNMENT);
    munit_assert_string_equal(ast->while_ptr->condition->control_ptr->left->assign_ptr->symbol, "elem");
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->assign_ptr->opt,      ==, OPERATOR_ASSIGN);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->assign_ptr->def_type, ==, KEYWORD_VAR);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->assign_ptr->assignment->type, ==, AST_FUNCTION_CALL);

    brama_destroy(context);
#ifdef _WIN32
    int test_status = munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
    getchar();
    return test_status;
#else
    return munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
#endif
}
