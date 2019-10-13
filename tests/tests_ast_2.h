#ifndef TESTS_AST_2_H
#define TESTS_AST_2_H

#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"


MunitResult ast_while_loop_1(const MunitParameter params[], void* user_data_or_fixture) {
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
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_while_loop_2(const MunitParameter params[], void* user_data_or_fixture) {
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
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_while_loop_3(const MunitParameter params[], void* user_data_or_fixture) {
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
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_while_loop_4(const MunitParameter params[], void* user_data_or_fixture) {
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
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_while_loop_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "while (count < 10) {\n"
                            "break;\n"
                            "}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type,                               ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type,              ==, AST_BLOCK);
    munit_assert_int         (ast->while_ptr->body->vector_ptr->count, ==, 1);
    munit_assert_int         (((t_ast_ptr)vector_get(ast->while_ptr->body->vector_ptr, 0))->type,                    ==, AST_BREAK);
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type,                    ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->opt,        ==, OPERATOR_LESS_THAN);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_if_stmt_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "if (true == true) { console.log(true) } else console.log(false)");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_IF_STATEMENT);
    munit_assert_ptr_not_null(ast->if_stmt_ptr);
    munit_assert_ptr_not_null(ast->if_stmt_ptr->condition);
    munit_assert_int         (ast->if_stmt_ptr->condition->type, ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->left->type,  ==, AST_PRIMATIVE);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->right->type, ==, AST_PRIMATIVE);
    
    munit_assert_ptr_not_null(ast->if_stmt_ptr->true_body);
    munit_assert_int         (ast->if_stmt_ptr->true_body->type, ==, AST_BLOCK);

    munit_assert_ptr_not_null(ast->if_stmt_ptr->false_body);
    munit_assert_int         (ast->if_stmt_ptr->false_body->type, ==, AST_FUNCTION_CALL);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_if_stmt_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "if (true == true) test = true");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_IF_STATEMENT);
    munit_assert_ptr_not_null(ast->if_stmt_ptr);
    munit_assert_ptr_not_null(ast->if_stmt_ptr->condition);
    munit_assert_int         (ast->if_stmt_ptr->condition->type, ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->left->type,  ==, AST_PRIMATIVE);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->right->type, ==, AST_PRIMATIVE);
    
    munit_assert_ptr_not_null(ast->if_stmt_ptr->true_body);
    munit_assert_int         (ast->if_stmt_ptr->true_body->type, ==, AST_ASSIGNMENT);

    munit_assert_ptr_null    (ast->if_stmt_ptr->false_body);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_if_stmt_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "if ((function() {test = true})() == true) test = true");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int         (ast->type, ==, AST_IF_STATEMENT);
    munit_assert_ptr_not_null(ast->if_stmt_ptr);
    munit_assert_ptr_not_null(ast->if_stmt_ptr->condition);
    munit_assert_int         (ast->if_stmt_ptr->condition->type, ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->left->type,  ==, AST_FUNCTION_CALL);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->right->type, ==, AST_PRIMATIVE);
    
    munit_assert_ptr_not_null(ast->if_stmt_ptr->true_body);
    munit_assert_int         (ast->if_stmt_ptr->true_body->type, ==, AST_ASSIGNMENT);

    munit_assert_ptr_null    (ast->if_stmt_ptr->false_body);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_if_stmt_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "var test = 1;\n"
                            "if (test == -1)\n"
                            "    console.log(\"Test is -1\");\n"
                            "else if (test == 0)\n"
                            "    console.log(\"Test is 0\");\n"
                            "else\n"
                            "    console.log(\"Test is 1\");");
    munit_assert_int         (context->parser->asts->count, ==, 2);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                                 ==, AST_ASSIGNMENT);

    ast = (t_ast_ptr)vector_get(context->parser->asts, 1);
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

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_if_stmt_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "var test = 1;\n"
                            "if (test == -1)\n"
                            "    console.log(\"Test is -1\");\n"
                            "    console.log(\"Test is -1\");\n"
                            "else if (test == 0)\n"
                            "    console.log(\"Test is 0\");\n"
                            "else\n"
                            "    console.log(\"Test is 1\");");
    munit_assert_int(context->status, ==, BRAMA_DOES_NOT_MATCH_AST);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "return true");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "var func = (function() {return true})");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);
    munit_assert_int(ast->type, ==, AST_ASSIGNMENT);
    munit_assert_int(((t_ast_ptr)vector_get(ast->assign_ptr->assignment->func_decl_ptr->body->vector_ptr, 0))->type, ==, AST_RETURN);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "if (true) return false");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "function test () {\n"
                            "    if (true) \n"
                            "        return false;\n"
                            "    return true;\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_OK);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "function test () {\n"
                            "    while(true) \n"
                            "        return false;\n"
                            "    return true;\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_OK);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "    while(true) \n"
                            "        return false;\n"
                            "    return true;\n");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_7(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "{ \n"
                            "return false;\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "break");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_BREAK_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "{ \n"
                            "break\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_BREAK_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "if (true) break;");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_BREAK_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "while (true) {\n"
                            "    (function(){\n"
                            "        break;\n"
                            "    })();\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_BREAK_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_execute(context,  "var data = 1;\n"
                            "while (true) {\n"
                            "\n"
                            "    if (data != 10)\n"
                            "        console.log(data);\n"
                            "    else\n"
                            "        ++data;\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int         (context->parser->asts->count, ==, 2);

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,  ==, AST_ASSIGNMENT);

    ast = (t_ast_ptr)vector_get(context->parser->asts, 1);
    munit_assert_int         (ast->type,  ==, AST_WHILE);
    munit_assert_int         (ast->while_ptr->condition->type, ==, AST_PRIMATIVE);
    munit_assert_int         (ast->while_ptr->body->type,      ==, AST_BLOCK);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitTest AST_TESTS_2[] = {

    ADD_TEST(ast_while_loop_1),
    ADD_TEST(ast_while_loop_2),
    ADD_TEST(ast_while_loop_3),
    ADD_TEST(ast_while_loop_4),
    ADD_TEST(ast_while_loop_5),
    ADD_TEST(ast_if_stmt_1),
    ADD_TEST(ast_if_stmt_2),
    ADD_TEST(ast_if_stmt_3),
    ADD_TEST(ast_if_stmt_4),
    ADD_TEST(ast_if_stmt_5),
    ADD_TEST(ast_return_1),
    ADD_TEST(ast_return_2),
    ADD_TEST(ast_return_3),
    ADD_TEST(ast_return_4),
    ADD_TEST(ast_return_5),
    ADD_TEST(ast_return_6),
    ADD_TEST(ast_return_7),
    ADD_TEST(ast_break_1),
    ADD_TEST(ast_break_2),
    ADD_TEST(ast_break_3),
    ADD_TEST(ast_break_4),
    ADD_TEST(ast_break_5),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_AST_2_H
