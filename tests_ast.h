#ifndef TESTS_AST_H
#define TESTS_AST_H

#include "tests_core.h"
#include "static_py.h"
#include "brama_internal.h"

MunitResult ast_peek_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_peek(context));

    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    munit_assert_ptr_not_null(ast_peek(context));
    munit_assert_int(ast_peek(context)->type, ==, TOKEN_KEYWORD);
    munit_assert_int(ast_peek(context)->int_, ==, KEYWORD_VAR);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_previous_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_peek(context));

    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    munit_assert_ptr_null(ast_previous(context));

    context->parser->index = 1;
    munit_assert_ptr_not_null(ast_previous(context));
    munit_assert_string_equal(ast_peek(context)->char_ptr, "rows");
    munit_assert_int         (ast_peek(context)->type, ==, TOKEN_SYMBOL);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_consume(context));

    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    t_token* token1        = ast_consume(context);
    t_token* token2        = ast_consume(context);
    t_token* token3        = ast_consume(context);

    munit_assert_ptr_not_null(token1);
    munit_assert_ptr_not_null(token2);
    munit_assert_ptr_not_null(token3);

    munit_assert_int(token1->type, ==, TOKEN_KEYWORD);
    munit_assert_int(token1->int_, ==, KEYWORD_VAR);

    munit_assert_string_equal(token2->char_ptr, "rows");
    munit_assert_int         (token2->type, ==, TOKEN_SYMBOL);

    munit_assert_int(token3->int_, ==, OPERATOR_ASSIGN);
    munit_assert_int(token3->type, ==, TOKEN_OPERATOR);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_keyword_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_consume(context));

    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    t_token* token         = ast_consume_keyword(context, KEYWORD_VAR);

    munit_assert_ptr_not_null(token);
    munit_assert_int(token->type, ==, TOKEN_KEYWORD);
    munit_assert_int(token->int_, ==, KEYWORD_VAR);

    munit_assert_ptr_null(ast_consume_keyword(context, KEYWORD_BREAK));

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_token_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_consume(context));

    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    t_token* token         = ast_consume_token(context, TOKEN_KEYWORD);

    munit_assert_ptr_not_null(token);
    munit_assert_int(token->type, ==, TOKEN_KEYWORD);
    munit_assert_int(token->int_, ==, KEYWORD_VAR);

    munit_assert_ptr_null(ast_consume_token(context, TOKEN_INTEGER));

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_operator_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_consume(context));

    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    munit_assert_ptr_null(ast_consume_operator(context, OPERATOR_ASSIGN));

    ast_consume(context);
    ast_consume(context);

    t_token* token = ast_consume_token(context, TOKEN_OPERATOR);
    munit_assert_int(token->type, ==, TOKEN_OPERATOR);
    munit_assert_int(token->int_, ==, OPERATOR_ASSIGN);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_match_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_consume(context));

    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    munit_assert_int(ast_match_keyword(context, 1, KEYWORD_BREAK), ==, 0);
    munit_assert_int(ast_match_keyword(context, 1, KEYWORD_VAR), ==, 1);

    ast_consume(context);
    munit_assert_int(ast_match_operator(context, 1, OPERATOR_ASSIGN), ==, 1);

    ast_consume(context);
    munit_assert_int(ast_match_operator(context, 2, OPERATOR_LEFT_PARENTHESES, OPERATOR_RIGHT_PARENTHESES), ==, 1);

    ast_consume(context);
    munit_assert_int(ast_match_operator(context, 2, OPERATOR_LEFT_PARENTHESES, OPERATOR_RIGHT_PARENTHESES), ==, 1);
    munit_assert_int(ast_match_operator(context, 2, OPERATOR_LEFT_PARENTHESES, OPERATOR_RIGHT_PARENTHESES), ==, 0);
    munit_assert_int(ast_match_operator(context, 2, OPERATOR_SEMICOLON), ==, 1);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_match_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_consume(context));

    static_py_execute(context, "const array = [1, 2];");
    context->parser->index = 0;

    munit_assert_int(ast_is_at_end     (context),                    ==, 0);
    munit_assert_int(ast_match_token   (context, 1, TOKEN_OPERATOR), ==, 0);
    munit_assert_int(ast_match_operator(context, 1, OPERATOR_AND),   ==, 0);
    munit_assert_int(ast_match_keyword (context, 1, KEYWORD_CONST),  ==, 1);

    munit_assert_int(ast_match_token   (context, 1, TOKEN_OPERATOR), ==, 0);
    munit_assert_int(ast_match_token   (context, 1, TOKEN_KEYWORD),  ==, 0);
    munit_assert_int(ast_match_token   (context, 1, TOKEN_SYMBOL),   ==, 1);

    munit_assert_int(ast_match_operator(context, 1, OPERATOR_ASSIGN),               ==, 1);
    munit_assert_int(ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_END),   ==, 0);
    munit_assert_int(ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_START), ==, 1);

    munit_assert_int(ast_match_token   (context, 1, TOKEN_INTEGER),               ==, 1);
    munit_assert_int(ast_match_operator(context, 1, OPERATOR_COMMA),              ==, 1);
    munit_assert_int(ast_match_token   (context, 1, TOKEN_DOUBLE),                ==, 0);
    munit_assert_int(ast_match_token   (context, 1, TOKEN_INTEGER),               ==, 1);
    munit_assert_int(ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_END), ==, 1);
    munit_assert_int(ast_match_token   (context, 1, TOKEN_DOUBLE),                ==, 0);
    munit_assert_int(ast_match_operator(context, 1, OPERATOR_SEMICOLON),          ==, 1);
    munit_assert_int(ast_is_at_end     (context),                                 ==, 1);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_check_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    munit_assert_ptr_null(ast_consume(context));

    static_py_execute(context, "var companies = ['Spacex', \"Tesla\"];");
    context->parser->index = 0;

    munit_assert_int(ast_is_at_end     (context),                 ==, 0);
    munit_assert_int(ast_check_token   (context, TOKEN_OPERATOR), ==, 0);
    munit_assert_int(ast_check_keyword (context, KEYWORD_DO),     ==, 0);
    munit_assert_int(ast_check_keyword (context, KEYWORD_VAR),    ==, 1);
    ast_consume(context);

    munit_assert_int(ast_check_token   (context, TOKEN_SYMBOL),   ==, 1);
    munit_assert_string_equal(ast_peek (context)->char_ptr, "companies");
    ast_consume(context);

    munit_assert_int(ast_check_token   (context, TOKEN_OPERATOR), ==, 1);
    ast_consume(context);

    munit_assert_int(ast_check_operator(context, OPERATOR_SQUARE_BRACKET_START), ==, 1);
    ast_consume(context);

    munit_assert_int(ast_check_token   (context, TOKEN_TEXT), ==, 1);
    munit_assert_string_equal(ast_peek (context)->char_ptr, "Spacex");
    ast_consume(context);

    munit_assert_int(ast_check_operator(context, OPERATOR_COMMA), ==, 1);
    ast_consume(context);

    munit_assert_int(ast_check_token   (context, TOKEN_TEXT),   ==, 1);
    munit_assert_string_equal(ast_peek (context)->char_ptr, "Tesla");
    ast_consume(context);

    munit_assert_int(ast_check_operator(context, OPERATOR_BITWISE_AND),        ==, 0);
    munit_assert_int(ast_check_operator(context, OPERATOR_SQUARE_BRACKET_END), ==, 1);
    ast_consume(context);

    munit_assert_int(ast_check_operator(context, OPERATOR_BITWISE_OR), ==, 0);
    munit_assert_int(ast_check_operator(context, OPERATOR_SEMICOLON),  ==, 1);
    ast_consume(context);

    munit_assert_int(ast_is_at_end(context), ==, 1);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitTest AST_TESTS[] = {
    ADD_TEST(ast_peek_test),
    ADD_TEST(ast_previous_test),
    ADD_TEST(ast_consume_test),
    ADD_TEST(ast_consume_keyword_test),
    ADD_TEST(ast_consume_token_test),
    ADD_TEST(ast_consume_operator_test),
    ADD_TEST(ast_match_test_1),
    ADD_TEST(ast_match_test_2),
    ADD_TEST(ast_check_test_1),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_AST_H
