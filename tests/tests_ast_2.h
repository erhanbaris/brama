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
    munit_assert_int         (ast_while_loop(context, &ast, NULL), == , BRAMA_OK);
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
    munit_assert_int         (ast_while_loop(context, &ast, NULL), == , BRAMA_OK);
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
    munit_assert_int         (ast_while_loop(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type, ==, AST_BLOCK);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitTest AST_TESTS_2[] = {

    ADD_TEST(ast_ast_while_loop_1),
    ADD_TEST(ast_ast_while_loop_2),
    ADD_TEST(ast_ast_while_loop_3),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_AST_2_H
