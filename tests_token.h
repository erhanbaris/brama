#ifndef TESTS_TOKEN_H
#define TESTS_TOKEN_H

#include "tests_core.h"
#include "static_py.h"

MunitResult string_token_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "\"hello world\"");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->data, "hello world");

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
    munit_assert_string_equal((char*)token->data, "hello world");

    static_py_destroy(context);
    return MUNIT_OK;
}

MunitResult string_token_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = static_py_init();
    static_py_execute(context, "'hello world\''");
    munit_assert_int(context->tokinizer->tokens.count, ==, 1);

    t_token* token = (t_token*)vector_get(&context->tokinizer->tokens, 0);
    if (token == NULL)
        return MUNIT_FAIL;

    munit_assert_int         (token->type, ==, TOKEN_TEXT);
    munit_assert_string_equal((char*)token->data, "hello world'");

    static_py_destroy(context);
    return MUNIT_OK;
}


MunitTest TOKEN_TESTS[] = {
    ADD_TEST(string_token_1),
    ADD_TEST(string_token_2),
    ADD_TEST(string_token_3),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};
static const MunitSuite TOKEN_SUITE = {
  "/token-tests", /* name */
  TOKEN_TESTS, /* tests */
  NULL, /* suites */
  1, /* iterations */
  MUNIT_SUITE_OPTION_NONE /* options */
};

/*do if in for let new try var case else enum eval null this true void with break catch class const false super throw while yield delete export import public return static switch typeof default extends finally package private continue debugger function arguments interface protected implements instanceof*/
#endif // TESTS_TOKEN_H
