#ifndef TESTS_AST_H
#define TESTS_AST_H

#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"

MunitResult ast_peek_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_peek(context));

    brama_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    munit_assert_ptr_not_null(ast_peek(context));
    munit_assert_int(ast_peek(context)->type, ==, TOKEN_KEYWORD);
    munit_assert_int(ast_peek(context)->int_, ==, KEYWORD_VAR);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_previous_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_peek(context));

    brama_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    munit_assert_ptr_null(ast_previous(context));

    context->parser->index = 1;
    munit_assert_ptr_not_null(ast_previous(context));
    munit_assert_string_equal(ast_peek(context)->char_ptr, "rows");
    munit_assert_int         (ast_peek(context)->type, ==, TOKEN_SYMBOL);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));

    brama_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    munit_assert_int(context->tokinizer->tokens->count, ==, 8);

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
    ast_consume(context);
    ast_consume(context);
    ast_consume(context);
    ast_consume(context);
    ast_consume(context);
    munit_assert_ptr_null(ast_consume(context)); // All tokens consumed so return value should be NULL

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_keyword_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));

    brama_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    t_token* token         = ast_consume_keyword(context, KEYWORD_VAR);

    munit_assert_ptr_not_null(token);
    munit_assert_int(token->type, ==, TOKEN_KEYWORD);
    munit_assert_int(token->int_, ==, KEYWORD_VAR);

    munit_assert_ptr_null(ast_consume_keyword(context, KEYWORD_BREAK));

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_token_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));

    brama_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    t_token* token         = ast_consume_token(context, TOKEN_KEYWORD);

    munit_assert_ptr_not_null(token);
    munit_assert_int(token->type, ==, TOKEN_KEYWORD);
    munit_assert_int(token->int_, ==, KEYWORD_VAR);

    munit_assert_ptr_null(ast_consume_token(context, TOKEN_INTEGER));

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_consume_operator_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));

    brama_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
    context->parser->index = 0;
    munit_assert_ptr_null(ast_consume_operator(context, OPERATOR_ASSIGN));

    ast_consume(context);
    ast_consume(context);

    t_token* token = ast_consume_token(context, TOKEN_OPERATOR);
    munit_assert_int(token->type, ==, TOKEN_OPERATOR);
    munit_assert_int(token->int_, ==, OPERATOR_ASSIGN);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_match_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));

    brama_execute(context, "var rows = prompt('How many rows for your multiplication table?');");
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

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_match_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));

    brama_execute(context, "const array = [1, 2];");
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

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_check_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));

    brama_execute(context, "var companies = ['Spacex', \"Tesla\"];");
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

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primative_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));
    brama_execute(context, "'hello world' \"hi all\" 1024 true null 3.14 =");
    context->parser->index = 0;
    t_ast* ast = NULL;
    munit_assert_int         (as_primative(ast_consume(context), &ast), ==, BRAMA_OK);
    munit_assert_ptr_not_null(ast);
    munit_assert_int         (ast->type, ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->primative_ptr);
    munit_assert_int         (ast->primative_ptr->type, ==, PRIMATIVE_STRING);
    munit_assert_string_equal(ast->primative_ptr->char_ptr, "hello world");

    ast = NULL;
    munit_assert_int         (as_primative(ast_consume(context), &ast), ==, BRAMA_OK);
    munit_assert_ptr_not_null(ast);
    munit_assert_int         (ast->type, ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->primative_ptr);
    munit_assert_int         (ast->primative_ptr->type, ==, PRIMATIVE_STRING);
    munit_assert_string_equal(ast->primative_ptr->char_ptr, "hi all");

    ast = NULL;
    munit_assert_int         (as_primative(ast_consume(context), &ast), ==, BRAMA_OK);
    munit_assert_ptr_not_null(ast);
    munit_assert_int         (ast->type, ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->primative_ptr);
    munit_assert_int         (ast->primative_ptr->type, ==, PRIMATIVE_INTEGER);
    munit_assert_int         (ast->primative_ptr->int_, ==, 1024);

    ast = NULL;
    munit_assert_int         (as_primative(ast_consume(context), &ast), ==, BRAMA_OK);
    munit_assert_ptr_not_null(ast);
    munit_assert_int         (ast->type, ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->primative_ptr);
    munit_assert_int         (ast->primative_ptr->type,  ==, PRIMATIVE_BOOL);
    munit_assert_int         (ast->primative_ptr->bool_, ==, true);

    ast = NULL;
    munit_assert_int         (as_primative(ast_consume(context), &ast), ==, BRAMA_OK);
    munit_assert_ptr_not_null(ast);
    munit_assert_int         (ast->type, ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->primative_ptr);
    munit_assert_int         (ast->primative_ptr->type, ==, PRIMATIVE_NULL);

    ast = NULL;
    munit_assert_int         (as_primative(ast_consume(context), &ast), ==, BRAMA_OK);
    munit_assert_ptr_not_null(ast);
    munit_assert_int         (ast->type, ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->primative_ptr);
    munit_assert_int         (ast->primative_ptr->type,    ==, PRIMATIVE_DOUBLE);
    munit_assert_double      (ast->primative_ptr->double_, ==, 3.14);

    ast = NULL;
    munit_assert_int         (as_primative(ast_consume(context), &ast), ==, BRAMA_PARSE_ERROR);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_is_primative_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    munit_assert_ptr_null(ast_consume(context));
    brama_execute(context, "'hello world' \"hi all\" 1024 true null 3.14 {}");
    context->parser->index = 0;
    t_token_ptr item_1 = ast_consume(context);
    t_token_ptr item_2 = ast_consume(context);
    t_token_ptr item_3 = ast_consume(context);
    t_token_ptr item_4 = ast_consume(context);
    t_token_ptr item_5 = ast_consume(context);
    t_token_ptr item_6 = ast_consume(context);
    t_token_ptr item_7 = ast_consume(context);
    t_token_ptr item_8 = ast_consume(context);

    munit_assert_int(is_primative(item_1), ==, true);
    munit_assert_int(is_primative(item_2), ==, true);
    munit_assert_int(is_primative(item_3), ==, true);
    munit_assert_int(is_primative(item_4), ==, true);
    munit_assert_int(is_primative(item_5), ==, true);
    munit_assert_int(is_primative(item_6), ==, true);
    munit_assert_int(is_primative(item_7), ==, false);
    munit_assert_int(is_primative(item_8), ==, false);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "10 10.1 true false null 'hello' \"world\" var");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->int_, ==, 10);
    munit_assert_int(ast->primative_ptr->type, ==, PRIMATIVE_INTEGER);

    ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->double_, ==, 10.1);
    munit_assert_int(ast->primative_ptr->type,    ==, PRIMATIVE_DOUBLE);

    ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->bool_, ==, true);
    munit_assert_int(ast->primative_ptr->type, ==, PRIMATIVE_BOOL);

    ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->bool_, ==, false);
    munit_assert_int(ast->primative_ptr->type, ==, PRIMATIVE_BOOL);

    ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->type, ==, PRIMATIVE_NULL);

    ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_string_equal(ast->primative_ptr->char_ptr, "hello");
    munit_assert_int(ast->primative_ptr->type, ==, PRIMATIVE_STRING);

    ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_string_equal(ast->primative_ptr->char_ptr, "world");
    munit_assert_int(ast->primative_ptr->type, ==, PRIMATIVE_STRING);

    ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_EXPRESSION_NOT_VALID);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "[1, true, null, 1.1, [], 'hello', \"world\"]");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->array->count, ==, 7);
    munit_assert_int(ast->primative_ptr->type,         ==, PRIMATIVE_ARRAY);

    t_vector_ptr vector = ast->primative_ptr->array;
    munit_assert_int(((t_primative_ptr)vector_get(vector, 0))->type, ==, PRIMATIVE_INTEGER);
    munit_assert_int(((t_primative_ptr)vector_get(vector, 0))->int_, ==, 1);

    munit_assert_int(((t_primative_ptr)vector_get(vector, 1))->type,  ==, PRIMATIVE_BOOL);
    munit_assert_int(((t_primative_ptr)vector_get(vector, 1))->bool_, ==, true);

    munit_assert_int(((t_primative_ptr)vector_get(vector, 2))->type, ==, PRIMATIVE_NULL);

    munit_assert_int(((t_primative_ptr)vector_get(vector, 3))->type,    ==, PRIMATIVE_DOUBLE);
    munit_assert_int(((t_primative_ptr)vector_get(vector, 3))->double_, ==, 1.1);

    munit_assert_int(((t_primative_ptr)vector_get(vector, 4))->type,         ==, PRIMATIVE_ARRAY);
    munit_assert_int(((t_primative_ptr)vector_get(vector, 4))->array->count, ==, 0);

    munit_assert_int         (((t_primative_ptr)vector_get(vector, 5))->type, ==, PRIMATIVE_STRING);
    munit_assert_string_equal(((t_primative_ptr)vector_get(vector, 5))->char_ptr, "hello");

    munit_assert_int         (((t_primative_ptr)vector_get(vector, 6))->type, ==, PRIMATIVE_STRING);
    munit_assert_string_equal(((t_primative_ptr)vector_get(vector, 6))->char_ptr, "world");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->type,         ==, PRIMATIVE_DICTIONARY);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{'hello': 'world'}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->type,         ==, PRIMATIVE_DICTIONARY);

    map_ast_t_ptr dictionary = ast->primative_ptr->dict;
    munit_assert_ptr_not_null(map_get(dictionary, "hello"));
    munit_assert_int         (((t_ast_ptr)*map_get(dictionary, "hello"))->type,                ==, AST_PRIMATIVE);
    munit_assert_int         (((t_ast_ptr)*map_get(dictionary, "hello"))->primative_ptr->type, ==, PRIMATIVE_STRING);
    munit_assert_string_equal(((t_ast_ptr)*map_get(dictionary, "hello"))->primative_ptr->char_ptr, "world");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{'hi': 'all', test: true, 'dict': {'empty': false}, 'array': [1,2,3]}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_OK);
    munit_assert_int(ast->primative_ptr->type,         ==, PRIMATIVE_DICTIONARY);

    map_ast_t_ptr main_dict = ast->primative_ptr->dict;
    munit_assert_ptr_not_null(map_get(main_dict, "hi"));
    munit_assert_ptr_not_null(map_get(main_dict, "test"));
    munit_assert_ptr_not_null(map_get(main_dict, "dict"));
    munit_assert_ptr_not_null(map_get(main_dict, "array"));

    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "hi"))->type,                ==, AST_PRIMATIVE);
    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "hi"))->primative_ptr->type, ==, PRIMATIVE_STRING);
    munit_assert_string_equal(((t_ast_ptr)*map_get(main_dict, "hi"))->primative_ptr->char_ptr, "all");

    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "test"))->type,                 ==, AST_PRIMATIVE);
    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "test"))->primative_ptr->type,  ==, PRIMATIVE_BOOL);
    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "test"))->primative_ptr->bool_, ==, true);

    /* Sub dict */
    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "dict"))->type,                 ==, AST_PRIMATIVE);
    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "dict"))->primative_ptr->type,  ==, PRIMATIVE_DICTIONARY);
    map_ast_t_ptr sub_dict = ((t_ast_ptr)*map_get(main_dict, "dict"))->primative_ptr->dict;
    munit_assert_ptr_not_null(map_get(sub_dict, "empty"));

    munit_assert_int         (((t_ast_ptr)*map_get(sub_dict, "empty"))->type,                 ==, AST_PRIMATIVE);
    munit_assert_int         (((t_ast_ptr)*map_get(sub_dict, "empty"))->primative_ptr->type,  ==, PRIMATIVE_BOOL);
    munit_assert_int         (((t_ast_ptr)*map_get(sub_dict, "empty"))->primative_ptr->bool_, ==, false);

    /* Array */
    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "array"))->type,                 ==, AST_PRIMATIVE);
    munit_assert_int         (((t_ast_ptr)*map_get(main_dict, "array"))->primative_ptr->type,  ==, PRIMATIVE_ARRAY);
    t_vector_ptr array = ((t_ast_ptr)*map_get(main_dict, "array"))->primative_ptr->array;
    munit_assert_int         (((t_primative_ptr)vector_get(array, 0))->type, ==, PRIMATIVE_INTEGER);
    munit_assert_int         (((t_primative_ptr)vector_get(array, 0))->int_, ==, 1);
    munit_assert_int         (((t_primative_ptr)vector_get(array, 1))->type, ==, PRIMATIVE_INTEGER);
    munit_assert_int         (((t_primative_ptr)vector_get(array, 1))->int_, ==, 2);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{'hi' 'all'}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_EXPRESSION_NOT_VALID);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_7(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{1+1: 'all'}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_EXPRESSION_NOT_VALID);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{'hi': 1+1}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_EXPRESSION_NOT_VALID);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_9(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{'hi': *}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_EXPRESSION_NOT_VALID);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_10(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "{'hi': var}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_EXPRESSION_NOT_VALID);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_primary_expr_test_11(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "[var]");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast),  ==, BRAMA_EXPRESSION_NOT_VALID);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_symbol_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "hello_world");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int         (ast->type,                      ==, AST_SYMBOL);
    munit_assert_string_equal(ast->char_ptr,                  "hello_world");
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_symbol_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "10");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int(ast->type, ==, AST_PRIMATIVE);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_call_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "test(true)");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_FUNCTION_CALL);
    munit_assert_ptr_not_null(ast->func_call_ptr);
    munit_assert_int         (ast->func_call_ptr->function->count, ==, 1);
    munit_assert_string_equal((char_ptr)vector_get(ast->func_call_ptr->function, 0), "test");
    munit_assert_int         (ast->func_call_ptr->args->count, ==, 1);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_call_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "test_2.print({data:1})");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), ==, BRAMA_OK);
    munit_assert_int         (ast->type,               ==, AST_FUNCTION_CALL);
    munit_assert_ptr_not_null(ast->func_call_ptr);
    munit_assert_int         (ast->func_call_ptr->function->count, ==, 2);
    munit_assert_string_equal((char_ptr)vector_get(ast->func_call_ptr->function, 0), "test_2");
    munit_assert_string_equal((char_ptr)vector_get(ast->func_call_ptr->function, 1), "print");
    munit_assert_int         (ast->func_call_ptr->args->count, ==, 1);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->func_call_ptr->args, 0))->type,                ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(((t_ast_ptr)vector_get(ast->func_call_ptr->args, 0))->primative_ptr);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->func_call_ptr->args, 0))->primative_ptr->type, == , PRIMATIVE_DICTIONARY);
    

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_multiplication_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "10 * 20");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_MULTIPLICATION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_multiplication_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "10 / 20");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_DIVISION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_multiplication_expr_test_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "_ten / _twelve");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_DIVISION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);
    munit_assert_int         (ast->binary_ptr->left->type, == , AST_SYMBOL);
    munit_assert_int         (ast->binary_ptr->right->type, == , AST_SYMBOL);
    munit_assert_string_equal(ast->binary_ptr->left->char_ptr, "_ten");
    munit_assert_string_equal(ast->binary_ptr->right->char_ptr, "_twelve");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_multiplication_expr_test_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "_ten / 124");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_DIVISION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);
    munit_assert_int         (ast->binary_ptr->left->type,  == , AST_SYMBOL);
    munit_assert_int         (ast->binary_ptr->right->type, == , AST_PRIMATIVE);
    munit_assert_string_equal(ast->binary_ptr->left->char_ptr, "_ten");
    munit_assert_int         (ast->binary_ptr->right->primative_ptr->int_, == , 124);

    brama_destroy(context);
    return MUNIT_OK;
}


MunitResult ast_addition_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "10 - 20");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int          (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int          (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_SUBTRACTION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_addition_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "10 + 20");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_ADDITION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_addition_expr_test_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "_ten - _twelve");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_SUBTRACTION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);
    munit_assert_int         (ast->binary_ptr->left->type, == , AST_SYMBOL);
    munit_assert_int         (ast->binary_ptr->right->type, == , AST_SYMBOL);
    munit_assert_string_equal(ast->binary_ptr->left->char_ptr, "_ten");
    munit_assert_string_equal(ast->binary_ptr->right->char_ptr, "_twelve");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_addition_expr_test_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "_ten + 124");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_ADDITION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);
    munit_assert_int         (ast->binary_ptr->left->type,  == , AST_SYMBOL);
    munit_assert_int         (ast->binary_ptr->right->type, == , AST_PRIMATIVE);
    munit_assert_string_equal(ast->binary_ptr->left->char_ptr, "_ten");
    munit_assert_int         (ast->binary_ptr->right->primative_ptr->int_, == , 124);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_addition_expr_test_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "1024 - 1000 * 2");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_SUBTRACTION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);
    munit_assert_int         (ast->binary_ptr->left->type,  == , AST_PRIMATIVE);
    munit_assert_int         (ast->binary_ptr->right->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr->left->primative_ptr);
    munit_assert_int         (ast->binary_ptr->left->primative_ptr->int_, == , 1024);
    munit_assert_ptr_not_null(ast->binary_ptr->right->binary_ptr);
    munit_assert_int         (ast->binary_ptr->right->binary_ptr->operator, ==, OPERATOR_MULTIPLICATION);
    munit_assert_int         (ast->binary_ptr->right->binary_ptr->left->primative_ptr->int_,  ==, 1000);
    munit_assert_int         (ast->binary_ptr->right->binary_ptr->right->primative_ptr->int_, ==, 2);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_addition_expr_test_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "(2 * 1000) - (1024 + 2)");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr);
    munit_assert_int         (ast->binary_ptr->operator, == , OPERATOR_SUBTRACTION);
    munit_assert_ptr_not_null(ast->binary_ptr->left);
    munit_assert_ptr_not_null(ast->binary_ptr->right);
    munit_assert_int         (ast->binary_ptr->left->type,  == , AST_BINARY_OPERATION);
    munit_assert_int         (ast->binary_ptr->right->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->binary_ptr->left->binary_ptr);
    munit_assert_int         (ast->binary_ptr->left->binary_ptr->operator, ==, OPERATOR_MULTIPLICATION);
    munit_assert_int         (ast->binary_ptr->left->binary_ptr->left->primative_ptr->int_,  ==, 2);
    munit_assert_int         (ast->binary_ptr->left->binary_ptr->right->primative_ptr->int_, ==, 1000);
    munit_assert_ptr_not_null(ast->binary_ptr->right->binary_ptr);
    munit_assert_int         (ast->binary_ptr->right->binary_ptr->operator, ==, OPERATOR_ADDITION);
    munit_assert_int         (ast->binary_ptr->right->binary_ptr->left->primative_ptr->int_,  ==, 1024);
    munit_assert_int         (ast->binary_ptr->right->binary_ptr->right->primative_ptr->int_, ==, 2);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_control_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "true >= false");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_CONTROL_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr);
    munit_assert_int         (ast->control_ptr->operator, == , OPERATOR_GREATER_EQUAL_THAN);
    munit_assert_ptr_not_null(ast->control_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right);
    munit_assert_int         (ast->control_ptr->left->type,  == , AST_PRIMATIVE);
    munit_assert_int         (ast->control_ptr->right->type, == , AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->control_ptr->left->primative_ptr);
    munit_assert_int         (ast->control_ptr->left->primative_ptr->bool_, ==, true);
    munit_assert_ptr_not_null(ast->control_ptr->right->primative_ptr);
    munit_assert_int         (ast->control_ptr->right->primative_ptr->bool_, ==, false);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_control_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "512 * 2 >= 256 * 4");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_CONTROL_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr);
    munit_assert_int         (ast->control_ptr->operator, == , OPERATOR_GREATER_EQUAL_THAN);
    munit_assert_ptr_not_null(ast->control_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right);
    munit_assert_int         (ast->control_ptr->left->type,  == , AST_BINARY_OPERATION);
    munit_assert_int         (ast->control_ptr->right->type, == , AST_BINARY_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr->left->binary_ptr);
    munit_assert_int         (ast->control_ptr->left->binary_ptr->operator, ==, OPERATOR_MULTIPLICATION);
    munit_assert_ptr_not_null(ast->control_ptr->left->binary_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->left->binary_ptr->right);
    munit_assert_int         (ast->control_ptr->left->binary_ptr->left->primative_ptr->int_, ==, 512);
    munit_assert_int         (ast->control_ptr->left->binary_ptr->right->primative_ptr->int_, ==, 2);
    munit_assert_ptr_not_null(ast->control_ptr->left->binary_ptr->right);
    munit_assert_int         (ast->control_ptr->right->binary_ptr->operator, ==, OPERATOR_MULTIPLICATION);
    munit_assert_ptr_not_null(ast->control_ptr->right->binary_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right->binary_ptr->right);
    munit_assert_int         (ast->control_ptr->right->binary_ptr->left->primative_ptr->int_, ==, 256);
    munit_assert_int         (ast->control_ptr->right->binary_ptr->right->primative_ptr->int_, ==, 4);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_equality_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "true === true");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_CONTROL_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr);
    munit_assert_int         (ast->control_ptr->operator, == , OPERATOR_EQUAL_VALUE);
    munit_assert_ptr_not_null(ast->control_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right);
    munit_assert_int         (ast->control_ptr->left->type,  == , AST_PRIMATIVE);
    munit_assert_int         (ast->control_ptr->right->type, == , AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->control_ptr->left->primative_ptr);
    munit_assert_int         (ast->control_ptr->left->primative_ptr->bool_, ==, true);
    munit_assert_ptr_not_null(ast->control_ptr->right->primative_ptr);
    munit_assert_int         (ast->control_ptr->right->primative_ptr->bool_, ==, true);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_equality_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "true !== false");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_CONTROL_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr);
    munit_assert_int         (ast->control_ptr->operator, == , OPERATOR_NOT_EQUAL_VALUE);
    munit_assert_ptr_not_null(ast->control_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right);
    munit_assert_int         (ast->control_ptr->left->type,  == , AST_PRIMATIVE);
    munit_assert_int         (ast->control_ptr->right->type, == , AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->control_ptr->left->primative_ptr);
    munit_assert_int         (ast->control_ptr->left->primative_ptr->bool_, ==, true);
    munit_assert_ptr_not_null(ast->control_ptr->right->primative_ptr);
    munit_assert_int         (ast->control_ptr->right->primative_ptr->bool_, ==, false);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_and_expr_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "true && true");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_CONTROL_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr);
    munit_assert_int         (ast->control_ptr->operator, == , OPERATOR_AND);
    munit_assert_ptr_not_null(ast->control_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right);
    munit_assert_int         (ast->control_ptr->left->type,  == , AST_PRIMATIVE);
    munit_assert_int         (ast->control_ptr->right->type, == , AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->control_ptr->left->primative_ptr);
    munit_assert_int         (ast->control_ptr->left->primative_ptr->bool_, ==, true);
    munit_assert_ptr_not_null(ast->control_ptr->right->primative_ptr);
    munit_assert_int         (ast->control_ptr->right->primative_ptr->bool_, ==, true);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_or_expr_test(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "true || true");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_CONTROL_OPERATION);
    munit_assert_ptr_not_null(ast->control_ptr);
    munit_assert_int         (ast->control_ptr->operator, == , OPERATOR_OR);
    munit_assert_ptr_not_null(ast->control_ptr->left);
    munit_assert_ptr_not_null(ast->control_ptr->right);
    munit_assert_int         (ast->control_ptr->left->type,  == , AST_PRIMATIVE);
    munit_assert_int         (ast->control_ptr->right->type, == , AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->control_ptr->left->primative_ptr);
    munit_assert_int         (ast->control_ptr->left->primative_ptr->bool_, ==, true);
    munit_assert_ptr_not_null(ast->control_ptr->right->primative_ptr);
    munit_assert_int         (ast->control_ptr->right->primative_ptr->bool_, ==, true);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_assignment_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "erhan = 1024");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_ASSIGNMENT);
    munit_assert_ptr_not_null(ast->assign_ptr);
    munit_assert_int         (ast->assign_ptr->operator, == , OPERATOR_ASSIGN);
    munit_assert_int         (ast->assign_ptr->def_type, == , KEYWORD_VAR);
    munit_assert_ptr_not_null(ast->assign_ptr->symbol);
    munit_assert_string_equal(ast->assign_ptr->symbol, "erhan");
    munit_assert_ptr_not_null(ast->assign_ptr->assignment);
    munit_assert_ptr_not_null(ast->assign_ptr->assignment->primative_ptr);
    munit_assert_int         (ast->assign_ptr->assignment->primative_ptr->type, ==, PRIMATIVE_INTEGER);
    munit_assert_int         (ast->assign_ptr->assignment->primative_ptr->int_, ==, 1024);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_assignment_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "let data_test = 'hello world'");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_ASSIGNMENT);
    munit_assert_ptr_not_null(ast->assign_ptr);
    munit_assert_int         (ast->assign_ptr->operator, == , OPERATOR_ASSIGN);
    munit_assert_int         (ast->assign_ptr->def_type, == , KEYWORD_LET);
    munit_assert_ptr_not_null(ast->assign_ptr->symbol);
    munit_assert_string_equal(ast->assign_ptr->symbol, "data_test");
    munit_assert_ptr_not_null(ast->assign_ptr->assignment);
    munit_assert_ptr_not_null(ast->assign_ptr->assignment->primative_ptr);
    munit_assert_int         (ast->assign_ptr->assignment->primative_ptr->type,     ==, PRIMATIVE_STRING);
    munit_assert_string_equal(ast->assign_ptr->assignment->primative_ptr->char_ptr, "hello world");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_unary_expr_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "-test");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_UNARY);
    munit_assert_ptr_not_null(ast->unary_ptr);
    munit_assert_int         (ast->unary_ptr->operator,    == , OPERATOR_SUBTRACTION);
    munit_assert_int         (ast->unary_ptr->right->type, == , AST_SYMBOL);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_unary_expr_test_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "++test");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_UNARY);
    munit_assert_ptr_not_null(ast->unary_ptr);
    munit_assert_int         (ast->unary_ptr->operator,    == , OPERATOR_INCREMENT);
    munit_assert_int         (ast->unary_ptr->right->type, == , AST_SYMBOL);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_unary_expr_test_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "!test");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_expression(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_UNARY);
    munit_assert_ptr_not_null(ast->unary_ptr);
    munit_assert_int         (ast->unary_ptr->operator,    == , OPERATOR_NOT);
    munit_assert_int         (ast->unary_ptr->right->type, == , AST_SYMBOL);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_unary_expr_test_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "--10");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_expression(context, &ast), == , BRAMA_EXPRESSION_NOT_VALID);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_func_decl_test_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context, "function test(data) {}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast), == , BRAMA_OK);
    munit_assert_int         (ast->type, == , AST_FUNCTION_DECLARATION);
    munit_assert_ptr_not_null(ast->func_decl_ptr);
    munit_assert_string_equal(ast->func_decl_ptr->name, "test");
    munit_assert_int         (ast->func_decl_ptr->args->count, == , 1);
    brama_destroy(context);
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
    ADD_TEST(ast_primative_test_1),
    ADD_TEST(ast_is_primative_test_1),
    ADD_TEST(ast_primary_expr_test_1),
    ADD_TEST(ast_primary_expr_test_2),
    ADD_TEST(ast_primary_expr_test_4),
    ADD_TEST(ast_primary_expr_test_5),
    ADD_TEST(ast_primary_expr_test_6),
    ADD_TEST(ast_primary_expr_test_7),
    ADD_TEST(ast_primary_expr_test_8),
    ADD_TEST(ast_primary_expr_test_9),
    ADD_TEST(ast_primary_expr_test_10),
    ADD_TEST(ast_primary_expr_test_11),
    ADD_TEST(ast_symbol_expr_test_1),
    ADD_TEST(ast_symbol_expr_test_2),
    ADD_TEST(ast_call_expr_test_1),
    ADD_TEST(ast_call_expr_test_2),
    ADD_TEST(ast_multiplication_expr_test_1),
    ADD_TEST(ast_multiplication_expr_test_2),
    ADD_TEST(ast_multiplication_expr_test_3),
    ADD_TEST(ast_multiplication_expr_test_4),
    ADD_TEST(ast_addition_expr_test_1),
    ADD_TEST(ast_addition_expr_test_2),
    ADD_TEST(ast_addition_expr_test_3),
    ADD_TEST(ast_addition_expr_test_4),
    ADD_TEST(ast_addition_expr_test_5),
    ADD_TEST(ast_addition_expr_test_6),
    ADD_TEST(ast_control_expr_test_1),
    ADD_TEST(ast_control_expr_test_2),
    ADD_TEST(ast_equality_expr_test_1),
    ADD_TEST(ast_equality_expr_test_2),
    ADD_TEST(ast_and_expr_test),
    ADD_TEST(ast_or_expr_test),
    ADD_TEST(ast_assignment_expr_test_1),
    ADD_TEST(ast_assignment_expr_test_2),
    ADD_TEST(ast_unary_expr_test_1),
    ADD_TEST(ast_unary_expr_test_2),
    ADD_TEST(ast_unary_expr_test_3),
    ADD_TEST(ast_unary_expr_test_4),
    ADD_TEST(ast_func_decl_test_1),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_AST_H
