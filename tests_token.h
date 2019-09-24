#ifndef TESTS_TOKEN_H
#define TESTS_TOKEN_H

#include "tests_core.h"
#include "static_py.h"

#define KEYWORD_EQUAL(INDEX, KEYWORD) \
    if ((t_token*)vector_get(&context->tokinizer->tokens, INDEX ) == NULL) return MUNIT_FAIL;\
    munit_assert_int (((t_token*)vector_get(&context->tokinizer->tokens, INDEX ))->type, ==, TOKEN_KEYWORD); \
    munit_assert_int ((((t_token*)vector_get(&context->tokinizer->tokens, INDEX ))->int_), ==, KEYWORD );

#define OPERATOR_EQUAL_CHECK(INDEX, OPERATOR) \
    if ((t_token*)vector_get(&context->tokinizer->tokens, INDEX ) == NULL) return MUNIT_FAIL;\
    munit_assert_int (((t_token*)vector_get(&context->tokinizer->tokens, INDEX ))->type, ==, TOKEN_OPERATOR); \
    munit_assert_int ((((t_token*)vector_get(&context->tokinizer->tokens, INDEX ))->int_), ==, OPERATOR );


/* STRING TESTS BEGIN --> */
MunitResult string_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "\"hello world\"");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world");

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult string_token_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "'hello world'");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world");

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult string_token_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "'hello world\\\''");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world'");

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult string_token_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "\"hello world\\\"\"");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world\"");

    static_py_destroy(context);
    return MUNIT_OK;
}
/* <-- STRING TESTS END */

/* KEYWORD TESTS BEGIN --> */

MunitResult keyword_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "do if in for let new try var case else enum eval null this true void with break catch class const false super throw while yield delete export import public return static switch typeof default extends finally package private continue debugger function arguments interface protected implements instanceof");
    munit_assert_int (context->tokinizer->tokens.count, ==, 47);
    KEYWORD_EQUAL    (0,  KEYWORD_DO);
    KEYWORD_EQUAL    (1,  KEYWORD_IF);
    KEYWORD_EQUAL    (2,  KEYWORD_IN);
    KEYWORD_EQUAL    (3,  KEYWORD_FOR);
    KEYWORD_EQUAL    (4,  KEYWORD_LET);
    KEYWORD_EQUAL    (5,  KEYWORD_NEW);
    KEYWORD_EQUAL    (6,  KEYWORD_TRY);
    KEYWORD_EQUAL    (7,  KEYWORD_VAR);
    KEYWORD_EQUAL    (8,  KEYWORD_CASE);
    KEYWORD_EQUAL    (9,  KEYWORD_ELSE);
    KEYWORD_EQUAL    (10, KEYWORD_ENUM);
    KEYWORD_EQUAL    (11, KEYWORD_EVAL);
    KEYWORD_EQUAL    (12, KEYWORD_NULL);
    KEYWORD_EQUAL    (13, KEYWORD_THIS);
    KEYWORD_EQUAL    (14, KEYWORD_TRUE);
    KEYWORD_EQUAL    (15, KEYWORD_VOID);
    KEYWORD_EQUAL    (16, KEYWORD_WITH);
    KEYWORD_EQUAL    (17, KEYWORD_BREAK);
    KEYWORD_EQUAL    (18, KEYWORD_CATCH);
    KEYWORD_EQUAL    (19, KEYWORD_CLASS);
    KEYWORD_EQUAL    (20, KEYWORD_CONST);
    KEYWORD_EQUAL    (21, KEYWORD_FALSE);
    KEYWORD_EQUAL    (22, KEYWORD_SUPER);
    KEYWORD_EQUAL    (23, KEYWORD_THROW);
    KEYWORD_EQUAL    (24, KEYWORD_WHILE);
    KEYWORD_EQUAL    (25, KEYWORD_YIELD);
    KEYWORD_EQUAL    (26, KEYWORD_DELETE);
    KEYWORD_EQUAL    (27, KEYWORD_EXPORT);
    KEYWORD_EQUAL    (28, KEYWORD_IMPORT);
    KEYWORD_EQUAL    (29, KEYWORD_PUBLIC);
    KEYWORD_EQUAL    (30, KEYWORD_RETURN);
    KEYWORD_EQUAL    (31, KEYWORD_STATIC);
    KEYWORD_EQUAL    (32, KEYWORD_SWITCH);
    KEYWORD_EQUAL    (33, KEYWORD_TYPEOF);
    KEYWORD_EQUAL    (34, KEYWORD_DEFAULT);
    KEYWORD_EQUAL    (35, KEYWORD_EXTENDS);
    KEYWORD_EQUAL    (36, KEYWORD_FINALLY);
    KEYWORD_EQUAL    (37, KEYWORD_PACKAGE);
    KEYWORD_EQUAL    (38, KEYWORD_PRIVATE);
    KEYWORD_EQUAL    (39, KEYWORD_CONTINUE);
    KEYWORD_EQUAL    (40, KEYWORD_DEBUGGER);
    KEYWORD_EQUAL    (41, KEYWORD_FUNCTION);
    KEYWORD_EQUAL    (42, KEYWORD_ARGUMENTS);
    KEYWORD_EQUAL    (43, KEYWORD_INTERFACE);
    KEYWORD_EQUAL    (44, KEYWORD_PROTECTED);
    KEYWORD_EQUAL    (45, KEYWORD_IMPLEMENTS);
    KEYWORD_EQUAL    (46, KEYWORD_INSTANCEOF);

    static_py_destroy(context);
    return MUNIT_OK;
}

/* <-- KEYWORD TESTS END */

/* NUMBER TESTS BEGIN --> */

MunitResult number_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "1024");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int(token->type, ==, TOKEN_INTEGER);
    munit_assert_int(token->int_, ==, 1024);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "1024.11");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int    (token->type, ==, TOKEN_DOUBLE);
    munit_assert_double (token->double_, ==, 1024.11);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "        2048        ");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int(token->type, ==, TOKEN_INTEGER);
    munit_assert_int(token->int_, ==, 2048);

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "      1024.1234567        ");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int    (token->type, ==, TOKEN_DOUBLE);
    munit_assert_double (token->double_, ==, 1024.1234567);

    static_py_destroy(context);
    return MUNIT_OK;
}

/* <-- NUMBER TESTS END */


/* SYMBOL TESTS BEGIN --> */

MunitResult symbol_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "erhan baris");
    munit_assert_int(context->tokinizer->tokens.count, ==, 2);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "erhan");

    token = (t_token*)vector_get(&context->tokinizer->tokens, 1);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "baris");

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult symbol_token_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "erhan baris test 2048 2048.1");
    munit_assert_int(context->tokinizer->tokens.count, ==, 5);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "erhan");

    token = (t_token*)vector_get(&context->tokinizer->tokens, 1);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "baris");

    token = (t_token*)vector_get(&context->tokinizer->tokens, 2);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "test");

    token = (t_token*)vector_get(&context->tokinizer->tokens, 3);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int(token->type, ==, TOKEN_INTEGER);
    munit_assert_int(token->int_, ==, 2048);

    token = (t_token*)vector_get(&context->tokinizer->tokens, 4);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int   (token->type,    ==, TOKEN_DOUBLE);
    munit_assert_double(token->double_, ==, 2048.1);

    static_py_destroy(context);
    return MUNIT_OK;
}

/* <-- SYMBOL TESTS END */

/* OPERATOR TESTS BEGIN --> */

MunitResult keyword_operator(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "+ - * / % ++ -- = += -= *= /= %= == === != !== ! && || & | ~ ^ << >> > < >= <= ? : &= |= ^= () [],;. // /* */");
    munit_assert_int (context->tokinizer->tokens.count, ==, 45);
    OPERATOR_EQUAL_CHECK(0,  OPERATOR_ADDITION);
    OPERATOR_EQUAL_CHECK(1,  OPERATOR_SUBTRACTION);
    OPERATOR_EQUAL_CHECK(2,  OPERATOR_MULTIPLICATION);
    OPERATOR_EQUAL_CHECK(3,  OPERATOR_DIVISION);
    OPERATOR_EQUAL_CHECK(4,  OPERATOR_MODULES);
    OPERATOR_EQUAL_CHECK(5,  OPERATOR_INCREMENT);
    OPERATOR_EQUAL_CHECK(6,  OPERATOR_DECCREMENT);
    OPERATOR_EQUAL_CHECK(7,  OPERATOR_ASSIGN);
    OPERATOR_EQUAL_CHECK(8,  OPERATOR_ASSIGN_ADDITION);
    OPERATOR_EQUAL_CHECK(9,  OPERATOR_ASSIGN_SUBTRACTION);
    OPERATOR_EQUAL_CHECK(10, OPERATOR_ASSIGN_MULTIPLICATION);
    OPERATOR_EQUAL_CHECK(11, OPERATOR_ASSIGN_DIVISION);
    OPERATOR_EQUAL_CHECK(12, OPERATOR_ASSIGN_MODULUS);
    OPERATOR_EQUAL_CHECK(13, OPERATOR_EQUAL);
    OPERATOR_EQUAL_CHECK(14, OPERATOR_EQUAL_VALUE);
    OPERATOR_EQUAL_CHECK(15, OPERATOR_NOT_EQUAL);
    OPERATOR_EQUAL_CHECK(16, OPERATOR_NOT_EQUAL_VALUE);
    OPERATOR_EQUAL_CHECK(17, OPERATOR_NOT);
    OPERATOR_EQUAL_CHECK(18, OPERATOR_AND);
    OPERATOR_EQUAL_CHECK(19, OPERATOR_OR);
    OPERATOR_EQUAL_CHECK(20, OPERATOR_BITWISE_AND);
    OPERATOR_EQUAL_CHECK(21, OPERATOR_BITWISE_OR);
    OPERATOR_EQUAL_CHECK(22, OPERATOR_BITWISE_NOT);
    OPERATOR_EQUAL_CHECK(23, OPERATOR_BITWISE_XOR);
    OPERATOR_EQUAL_CHECK(24, OPERATOR_BITWISE_LEFT_SHIFT);
    OPERATOR_EQUAL_CHECK(25, OPERATOR_BITWISE_RIGHT_SHIFT);
    OPERATOR_EQUAL_CHECK(26, OPERATOR_GREATER_THAN);
    OPERATOR_EQUAL_CHECK(27, OPERATOR_LESS_THAN);
    OPERATOR_EQUAL_CHECK(28, OPERATOR_GREATER_EQUAL_THAN);
    OPERATOR_EQUAL_CHECK(29, OPERATOR_LESS_EQUAL_THAN);
    OPERATOR_EQUAL_CHECK(30, OPERATOR_QUESTION_MARK);
    OPERATOR_EQUAL_CHECK(31, OPERATOR_COLON_MARK);
    OPERATOR_EQUAL_CHECK(32, OPERATOR_BITWISE_AND_ASSIGN);
    OPERATOR_EQUAL_CHECK(33, OPERATOR_BITWISE_OR_ASSIGN);
    OPERATOR_EQUAL_CHECK(34, OPERATOR_BITWISE_XOR_ASSIGN);
    OPERATOR_EQUAL_CHECK(35, OPERATOR_LEFT_PARENTHESES);
    OPERATOR_EQUAL_CHECK(36, OPERATOR_RIGHT_PARENTHESES);
    OPERATOR_EQUAL_CHECK(37, OPERATOR_SQUARE_BRACKET_START);
    OPERATOR_EQUAL_CHECK(38, OPERATOR_SQUARE_BRACKET_END);
    OPERATOR_EQUAL_CHECK(39, OPERATOR_COMMA);
    OPERATOR_EQUAL_CHECK(40, OPERATOR_SEMICOLON);
    OPERATOR_EQUAL_CHECK(41, OPERATOR_DOT);
    OPERATOR_EQUAL_CHECK(42, OPERATOR_COMMENT_LINE);
    OPERATOR_EQUAL_CHECK(43, OPERATOR_COMMENT_MULTILINE_START);
    OPERATOR_EQUAL_CHECK(44, OPERATOR_COMMENT_MULTILINE_END);


    static_py_destroy(context);
    return MUNIT_OK;
}

/* <-- OPERATOR TESTS END */


/* GENERAL TESTS BEGIN --> */

MunitResult keyword_general_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    munit_assert_int (context->tokinizer->tokens.count, ==, 8);

    static_py_destroy(context);
    return MUNIT_OK;
}

/* <-- OPERATOR TESTS END */

MunitTest TOKEN_TESTS[] = {
    ADD_TEST(string_token_1),
    ADD_TEST(string_token_2),
    ADD_TEST(string_token_3),
    ADD_TEST(string_token_4),

    ADD_TEST(keyword_token_1),

    ADD_TEST(number_token_1),
    ADD_TEST(number_token_2),
    ADD_TEST(number_token_3),
    ADD_TEST(number_token_4),

    ADD_TEST(symbol_token_1),
    ADD_TEST(symbol_token_2),

    ADD_TEST(keyword_operator),

    ADD_TEST(keyword_general_1),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
static const MunitSuite TOKEN_SUITE = {
  "/token-tests", /* name */
  TOKEN_TESTS, /* tests */
  NULL, /* suites */
  1, /* iterations */
  MUNIT_SUITE_OPTION_NONE /* options */
};

#endif // TESTS_TOKEN_H
