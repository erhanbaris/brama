#ifndef TESTS_BITWISE2_H
#define TESTS_BITWISE2_H

#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"

MunitResult ast_bitwise_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "~a");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int(context->parser->asts->length, ==, 1);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                      ==, AST_UNARY);
    munit_assert_int         (ast->unary_ptr->opt,            ==, OPERATOR_BITWISE_NOT);
    munit_assert_int         (ast->unary_ptr->operand_type,   ==, UNARY_OPERAND_BEFORE);
    munit_assert_int         (ast->unary_ptr->content->type,  ==, AST_SYMBOL);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_bitwise_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "(a & b)");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int(context->parser->asts->length, ==, 1);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                        ==, AST_BINARY_OPERATION);
    munit_assert_int         (ast->control_ptr->opt,            ==, OPERATOR_BITWISE_AND);
    munit_assert_int         (ast->control_ptr->left->type,     ==, AST_SYMBOL);
    munit_assert_int         (ast->control_ptr->right->type,    ==, AST_SYMBOL);

    munit_assert_string_equal(ast->control_ptr->left->char_ptr,  "a");
    munit_assert_string_equal(ast->control_ptr->right->char_ptr, "b");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_bitwise_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "(a | b)");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int(context->parser->asts->length, ==, 1);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                        ==, AST_BINARY_OPERATION);
    munit_assert_int         (ast->control_ptr->opt,            ==, OPERATOR_BITWISE_OR);
    munit_assert_int         (ast->control_ptr->left->type,     ==, AST_SYMBOL);
    munit_assert_int         (ast->control_ptr->right->type,    ==, AST_SYMBOL);

    munit_assert_string_equal(ast->control_ptr->left->char_ptr,  "a");
    munit_assert_string_equal(ast->control_ptr->right->char_ptr, "b");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_bitwise_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "(a ^ b)");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int(context->parser->asts->length, ==, 1);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                        ==, AST_BINARY_OPERATION);
    munit_assert_int         (ast->control_ptr->opt,            ==, OPERATOR_BITWISE_XOR);
    munit_assert_int         (ast->control_ptr->left->type,     ==, AST_SYMBOL);
    munit_assert_int         (ast->control_ptr->right->type,    ==, AST_SYMBOL);

    munit_assert_string_equal(ast->control_ptr->left->char_ptr,  "a");
    munit_assert_string_equal(ast->control_ptr->right->char_ptr, "b");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_bitwise_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "(a << b)");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int(context->parser->asts->length, ==, 1);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                        ==, AST_BINARY_OPERATION);
    munit_assert_int         (ast->control_ptr->opt,            ==, OPERATOR_BITWISE_LEFT_SHIFT);
    munit_assert_int         (ast->control_ptr->left->type,     ==, AST_SYMBOL);
    munit_assert_int         (ast->control_ptr->right->type,    ==, AST_SYMBOL);

    munit_assert_string_equal(ast->control_ptr->left->char_ptr,  "a");
    munit_assert_string_equal(ast->control_ptr->right->char_ptr, "b");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_bitwise_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "(a >>> b)");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int(context->parser->asts->length, ==, 1);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                        ==, AST_BINARY_OPERATION);
    munit_assert_int         (ast->control_ptr->opt,            ==, OPERATOR_BITWISE_UNSIGNED_RIGHT_SHIFT);
    munit_assert_int         (ast->control_ptr->left->type,     ==, AST_SYMBOL);
    munit_assert_int         (ast->control_ptr->right->type,    ==, AST_SYMBOL);

    munit_assert_string_equal(ast->control_ptr->left->char_ptr,  "a");
    munit_assert_string_equal(ast->control_ptr->right->char_ptr, "b");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_bitwise_7(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "(a >> b)");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int(context->parser->asts->length, ==, 1);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                        ==, AST_BINARY_OPERATION);
    munit_assert_int         (ast->control_ptr->opt,            ==, OPERATOR_BITWISE_RIGHT_SHIFT);
    munit_assert_int         (ast->control_ptr->left->type,     ==, AST_SYMBOL);
    munit_assert_int         (ast->control_ptr->right->type,    ==, AST_SYMBOL);

    munit_assert_string_equal(ast->control_ptr->left->char_ptr,  "a");
    munit_assert_string_equal(ast->control_ptr->right->char_ptr, "b");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_bitwise_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "~(ld | cols | rd) & all;");

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                                 ==, AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr);
    munit_assert_ptr_not_null(ast->control_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right);
    munit_assert_int         (ast->control_ptr->opt,                     ==, OPERATOR_BITWISE_AND);
    munit_assert_int         (ast->control_ptr->left->type,              ==, AST_UNARY);
    munit_assert_int         (ast->control_ptr->left->unary_ptr->opt,    ==, OPERATOR_BITWISE_NOT);
    munit_assert_int         (ast->control_ptr->left->unary_ptr->operand_type,  ==, UNARY_OPERAND_BEFORE);
    munit_assert_int         (ast->control_ptr->left->unary_ptr->content->type, ==, AST_BINARY_OPERATION);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitTest BITWISE_TESTS[] = {

    ADD_TEST(ast_bitwise_1),
    ADD_TEST(ast_bitwise_2),
    ADD_TEST(ast_bitwise_3),
    ADD_TEST(ast_bitwise_4),
    ADD_TEST(ast_bitwise_5),
    ADD_TEST(ast_bitwise_6),
    ADD_TEST(ast_bitwise_7),
    ADD_TEST(ast_bitwise_8),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_BITWISE2_H
