#ifndef TESTS_TOKEN_H
#define TESTS_TOKEN_H

#include <brama.h>
#include "tests_core.h"
#include "brama.h"

#define KEYWORD_EQUAL(INDEX, KEYWORD) \
    if (context->tokinizer->tokens->length <= INDEX ) return MUNIT_FAIL;\
    munit_assert_int (context->tokinizer->tokens->data[ INDEX ]->type, ==, TOKEN_KEYWORD); \
    munit_assert_int (context->tokinizer->tokens->data[ INDEX ]->keyword, ==, KEYWORD );

#define SYMBOL_EQUAL(INDEX, SYMBOL) \
    if (context->tokinizer->tokens->length <= INDEX ) return MUNIT_FAIL;\
    munit_assert_int         (context->tokinizer->tokens->data[ INDEX ]->type, ==, TOKEN_SYMBOL); \
    munit_assert_string_equal(context->tokinizer->tokens->data[ INDEX ]->char_ptr, SYMBOL );

#define TEXT_EQUAL(INDEX, TEXT) \
    if (context->tokinizer->tokens->length <= INDEX ) return MUNIT_FAIL;\
    munit_assert_int         (context->tokinizer->tokens->data[ INDEX ]->type, ==, TOKEN_TEXT); \
    munit_assert_string_equal(context->tokinizer->tokens->data[ INDEX ]->char_ptr, TEXT );

#define OPERATOR_CHECK(INDEX, OPERATOR) \
    if (context->tokinizer->tokens->length <= INDEX ) return MUNIT_FAIL;\
    munit_assert_int(context->tokinizer->tokens->data[ INDEX ]->type, ==, TOKEN_OPERATOR); \
    munit_assert_int(context->tokinizer->tokens->data[ INDEX ]->opt, ==, OPERATOR );

#define INTEGER_CHECK(INDEX, NUMBER) \
    if (context->tokinizer->tokens->length <= INDEX ) return MUNIT_FAIL;\
    munit_assert_int(context->tokinizer->tokens->data[ INDEX ] ->type, ==, TOKEN_INTEGER); \
    munit_assert_int(context->tokinizer->tokens->data[ INDEX ]->double_,  ==, NUMBER );

#define DOUBLE_CHECK(INDEX, NUMBER) \
    if (context->tokinizer->tokens->length <= INDEX ) return MUNIT_FAIL;\
    munit_assert_int   (context->tokinizer->tokens->data[ INDEX ] ->type,   ==, TOKEN_DOUBLE); \
    munit_assert_double(context->tokinizer->tokens->data[ INDEX ]->double_, ==, NUMBER );


/* STRING TESTS BEGIN --> */
MunitResult string_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "\"hello world\"");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult string_token_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "'hello world'");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult string_token_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "'hello world\\\''");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world'");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult string_token_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "\"hello world\\\"\"");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->char_ptr, "hello world\"");

    brama_destroy(context);
    return MUNIT_OK;
}
/* <-- STRING TESTS END */

/* KEYWORD TESTS BEGIN --> */

MunitResult keyword_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "do if in for let new try var case else enum eval null this true void with break catch class const false super throw while yield delete export import public return static switch typeof default extends finally package private continue debugger function arguments interface protected implements instanceof undefined");
    munit_assert_int (context->tokinizer->tokens->length, ==, 48);
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
    KEYWORD_EQUAL    (47, KEYWORD_UNDEFINED);

    brama_destroy(context);
    return MUNIT_OK;
}

/* <-- KEYWORD TESTS END */

/* NUMBER TESTS BEGIN --> */

MunitResult number_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "1024");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int(token->type, ==, TOKEN_INTEGER);
    munit_assert_int(token->double_, ==, 1024);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "1024.11");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int    (token->type, ==, TOKEN_DOUBLE);
    munit_assert_double (token->double_, ==, 1024.11);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "        2048        ");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int(token->type, ==, TOKEN_INTEGER);
    munit_assert_int(token->double_, ==, 2048);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "      1024.1234567        ");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int    (token->type, ==, TOKEN_DOUBLE);
    munit_assert_double (token->double_, ==, 1024.1234567);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "      -1024.1234567     -1024   ");
    munit_assert_int(context->tokinizer->tokens->length, ==, 2);

    t_token* token_1 = context->tokinizer->tokens->data[0];
    t_token* token_2 = context->tokinizer->tokens->data[1];

    munit_assert_int    (token_1->type,    ==, TOKEN_DOUBLE);
    munit_assert_double (token_1->double_, ==, -1024.1234567);

    munit_assert_int    (token_2->type, ==, TOKEN_INTEGER);
    munit_assert_double (token_2->double_, ==, -1024);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "9007199254740992");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];

    munit_assert_int    (token->type, ==, TOKEN_INTEGER);
    munit_assert_double (token->double_, ==, 9007199254740992.0);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_7(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "0.00314e2");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];

    munit_assert_int    (token->type, ==, TOKEN_DOUBLE);
    munit_assert_true (fabs(token->double_-0.314) < 0.0000001);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "314e2");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];

    munit_assert_int    (token->type, ==, TOKEN_INTEGER);
    munit_assert_double (token->double_, ==, 31400);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult number_token_9(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "0.314e-2");
    munit_assert_int(context->tokinizer->tokens->length, ==, 1);

    t_token* token = context->tokinizer->tokens->data[0];

    munit_assert_int  (token->type, ==, TOKEN_DOUBLE);
    munit_assert_true (fabs(token->double_-0.00314) < 0.00001);

    brama_destroy(context);
    return MUNIT_OK;
}

/* <-- NUMBER TESTS END */


/* SYMBOL TESTS BEGIN --> */

MunitResult symbol_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "erhan baris");
    munit_assert_int(context->tokinizer->tokens->length, ==, 2);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "erhan");

    token = context->tokinizer->tokens->data[1];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "baris");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult symbol_token_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "erhan baris test _test 2048 2048.1");
    munit_assert_int(context->tokinizer->tokens->length, ==, 6);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "erhan");

    token = context->tokinizer->tokens->data[1];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "baris");

    token = context->tokinizer->tokens->data[2];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "test");

    token = context->tokinizer->tokens->data[3];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, == , TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "_test");

    token = context->tokinizer->tokens->data[4];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int(token->type, ==, TOKEN_INTEGER);
    munit_assert_int(token->double_, ==, 2048);

    token = context->tokinizer->tokens->data[5];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int   (token->type,    ==, TOKEN_DOUBLE);
    munit_assert_double(token->double_, ==, 2048.1);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult symbol_token_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "_1_ _ $ test$ $test test$test");
    munit_assert_int(context->tokinizer->tokens->length, ==, 6);

    t_token* token = context->tokinizer->tokens->data[0];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "_1_");

    token = context->tokinizer->tokens->data[1];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "_");

    token = context->tokinizer->tokens->data[2];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "$");

    token = context->tokinizer->tokens->data[3];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, == , TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "test$");

    token = context->tokinizer->tokens->data[4];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type,   ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "$test");

    token = context->tokinizer->tokens->data[5];
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type,   ==, TOKEN_SYMBOL);
    munit_assert_string_equal(token->char_ptr, "test$test");

    brama_destroy(context);
    return MUNIT_OK;
}

/* <-- SYMBOL TESTS END */

/* OPERATOR TESTS BEGIN --> */

MunitResult keyword_operator(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "+ - * / % ++ -- = += -= *= /= %= == === != !== ! && || & | ~ ^ << >>> >> > < >= <= ? : &= |= ^= () [],;. // /* */");
    munit_assert_int (context->tokinizer->tokens->length, ==, 43);
    OPERATOR_CHECK(0,  OPERATOR_ADDITION);
    OPERATOR_CHECK(1,  OPERATOR_SUBTRACTION);
    OPERATOR_CHECK(2,  OPERATOR_MULTIPLICATION);
    OPERATOR_CHECK(3,  OPERATOR_DIVISION);
    OPERATOR_CHECK(4,  OPERATOR_MODULO);
    OPERATOR_CHECK(5,  OPERATOR_INCREMENT);
    OPERATOR_CHECK(6,  OPERATOR_DECCREMENT);
    OPERATOR_CHECK(7,  OPERATOR_ASSIGN);
    OPERATOR_CHECK(8,  OPERATOR_ASSIGN_ADDITION);
    OPERATOR_CHECK(9,  OPERATOR_ASSIGN_SUBTRACTION);
    OPERATOR_CHECK(10, OPERATOR_ASSIGN_MULTIPLICATION);
    OPERATOR_CHECK(11, OPERATOR_ASSIGN_DIVISION);
    OPERATOR_CHECK(12, OPERATOR_ASSIGN_MODULUS);
    OPERATOR_CHECK(13, OPERATOR_EQUAL);
    OPERATOR_CHECK(14, OPERATOR_EQUAL_VALUE);
    OPERATOR_CHECK(15, OPERATOR_NOT_EQUAL);
    OPERATOR_CHECK(16, OPERATOR_NOT_EQUAL_VALUE);
    OPERATOR_CHECK(17, OPERATOR_NOT);
    OPERATOR_CHECK(18, OPERATOR_AND);
    OPERATOR_CHECK(19, OPERATOR_OR);
    OPERATOR_CHECK(20, OPERATOR_BITWISE_AND);
    OPERATOR_CHECK(21, OPERATOR_BITWISE_OR);
    OPERATOR_CHECK(22, OPERATOR_BITWISE_NOT);
    OPERATOR_CHECK(23, OPERATOR_BITWISE_XOR);
    OPERATOR_CHECK(24, OPERATOR_BITWISE_LEFT_SHIFT);
    OPERATOR_CHECK(25, OPERATOR_BITWISE_UNSIGNED_RIGHT_SHIFT);
    OPERATOR_CHECK(26, OPERATOR_BITWISE_RIGHT_SHIFT);
    OPERATOR_CHECK(27, OPERATOR_GREATER_THAN);
    OPERATOR_CHECK(28, OPERATOR_LESS_THAN);
    OPERATOR_CHECK(29, OPERATOR_GREATER_EQUAL_THAN);
    OPERATOR_CHECK(30, OPERATOR_LESS_EQUAL_THAN);
    OPERATOR_CHECK(31, OPERATOR_QUESTION_MARK);
    OPERATOR_CHECK(32, OPERATOR_COLON_MARK);
    OPERATOR_CHECK(33, OPERATOR_BITWISE_AND_ASSIGN);
    OPERATOR_CHECK(34, OPERATOR_BITWISE_OR_ASSIGN);
    OPERATOR_CHECK(35, OPERATOR_BITWISE_XOR_ASSIGN);
    OPERATOR_CHECK(36, OPERATOR_LEFT_PARENTHESES);
    OPERATOR_CHECK(37, OPERATOR_RIGHT_PARENTHESES);
    OPERATOR_CHECK(38, OPERATOR_SQUARE_BRACKET_START);
    OPERATOR_CHECK(39, OPERATOR_SQUARE_BRACKET_END);
    OPERATOR_CHECK(40, OPERATOR_COMMA);
    OPERATOR_CHECK(41, OPERATOR_SEMICOLON);
    OPERATOR_CHECK(42, OPERATOR_DOT);


    brama_destroy(context);
    return MUNIT_OK;
}

/* <-- OPERATOR TESTS END */


/* GENERAL TESTS BEGIN --> */

MunitResult keyword_general_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var rows = prompt('How many rows for your multiplication table?');");
    munit_assert_int (context->tokinizer->tokens->length, ==, 8);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult keyword_general_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "var rows = 10;");
    munit_assert_int (context->tokinizer->tokens->length, ==, 5);
    KEYWORD_EQUAL (0, KEYWORD_VAR);
    SYMBOL_EQUAL  (1, "rows");
    OPERATOR_CHECK(2, OPERATOR_ASSIGN);
    INTEGER_CHECK (3, 10);
    OPERATOR_CHECK(4, OPERATOR_SEMICOLON);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult keyword_general_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "document.getElementById('msg').innerHTML = Math.random( 1, 100 );");
    munit_assert_int (context->tokinizer->tokens->length, ==, 18);
    SYMBOL_EQUAL  (0, "document");
    OPERATOR_CHECK(1, OPERATOR_DOT);
    SYMBOL_EQUAL  (2, "getElementById");
    OPERATOR_CHECK(3, OPERATOR_LEFT_PARENTHESES);
    TEXT_EQUAL    (4, "msg");
    OPERATOR_CHECK(5, OPERATOR_RIGHT_PARENTHESES);
    OPERATOR_CHECK(6, OPERATOR_DOT);
    SYMBOL_EQUAL  (7, "innerHTML");
    OPERATOR_CHECK(8, OPERATOR_ASSIGN);
    SYMBOL_EQUAL  (9, "Math");
    OPERATOR_CHECK(10, OPERATOR_DOT);
    SYMBOL_EQUAL  (11, "random");
    OPERATOR_CHECK(12, OPERATOR_LEFT_PARENTHESES);
    INTEGER_CHECK (13, 1);
    OPERATOR_CHECK(14, OPERATOR_COMMA);
    INTEGER_CHECK (15, 100);
    OPERATOR_CHECK(16, OPERATOR_RIGHT_PARENTHESES);
    OPERATOR_CHECK(17, OPERATOR_SEMICOLON);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult keyword_general_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "'test");
    munit_assert_int (context->tokinizer->tokens->length, ==, 0);
    munit_assert_string_equal("Missing Delimiter at Line: 1, Column: 5", context->error_message);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult keyword_general_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "10..1");
    munit_assert_string_equal("Multiple dot used for double: 1, Column: 3", context->error_message);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult keyword_general_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init(0);
    brama_compile(context, "'hello world' \"hi all\" 1024 true false null 3.14 {} _test");
    munit_assert_int (context->tokinizer->tokens->length, ==, 10);
    TEXT_EQUAL    (0, "hello world");
    TEXT_EQUAL    (1, "hi all");
    INTEGER_CHECK (2, 1024);
    KEYWORD_EQUAL (3, KEYWORD_TRUE);
    KEYWORD_EQUAL (4, KEYWORD_FALSE);
    KEYWORD_EQUAL (5, KEYWORD_NULL);
    DOUBLE_CHECK  (6, 3.14);
    OPERATOR_CHECK(7, OPERATOR_CURVE_BRACKET_START);
    OPERATOR_CHECK(8, OPERATOR_CURVE_BRACKET_END);
    SYMBOL_EQUAL  (9, "_test");

    brama_destroy(context);
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
    ADD_TEST(number_token_5),
    ADD_TEST(number_token_6),
    ADD_TEST(number_token_7),
    ADD_TEST(number_token_8),
    ADD_TEST(number_token_9),

    ADD_TEST(symbol_token_1),
    ADD_TEST(symbol_token_2),
    ADD_TEST(symbol_token_3),

    ADD_TEST(keyword_operator),

    ADD_TEST(keyword_general_1),
    ADD_TEST(keyword_general_2),
    ADD_TEST(keyword_general_3),
    ADD_TEST(keyword_general_4),
    ADD_TEST(keyword_general_5),
    ADD_TEST(keyword_general_6),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_TOKEN_H
