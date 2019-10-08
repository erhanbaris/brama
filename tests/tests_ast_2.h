#ifndef TESTS_AST_2_H
#define TESTS_AST_2_H

#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"


MunitResult ast_ast_while_loop_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "while(true) console.log('forever');");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr);
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type, ==, AST_PRIMATIVE);
    munit_assert_int         (ast->while_ptr->condition->primative_ptr->bool_, ==, true);

    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type, ==, AST_FUNCTION_CALL);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_ast_while_loop_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "while(true) {}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr);
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type, ==, AST_PRIMATIVE);
    munit_assert_int         (ast->while_ptr->condition->primative_ptr->bool_, ==, true);

    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type, ==, AST_BLOCK);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_ast_while_loop_3(const MunitParameter params[], void* user_data_or_fixture) {
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
    return MUNIT_OK;
}

MunitResult ast_ast_while_loop_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "while (count < 10) {\n"
                            "    document.write(\"Current Count : \" + count + \"<br />\");\n"
                            "    count++;\n"
                            "}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type,                               ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type,              ==, AST_BLOCK);
    munit_assert_int         (ast->while_ptr->body->vector_ptr->count, ==, 2);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->type,                    ==, AST_FUNCTION_CALL);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 1))->type,                    ==, AST_UNARY);
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type,                    ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->opt,        ==, OPERATOR_LESS_THAN);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitTest AST_TESTS_2[] = {

    ADD_TEST(ast_ast_while_loop_1),
    ADD_TEST(ast_ast_while_loop_2),
    ADD_TEST(ast_ast_while_loop_3),
    ADD_TEST(ast_ast_while_loop_4),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_AST_2_H
