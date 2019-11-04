#ifndef TESTS_AST_2_H
#define TESTS_AST_2_H

#include "tests_core.h"
#include "brama.h"
#include "brama_internal.h"


MunitResult ast_while_loop_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "while(true) console.log('forever');");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
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
    brama_compile(context,  "while(true) {}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
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
    brama_compile(context,  "while ((elem = document.getElementById('id-' + a)) !== null) {\n"
                            "    //Some code\n"
                            "    a++;\n"
                            "}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
    munit_assert_int         (ast->type,                               ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type,              ==, AST_BLOCK);
    munit_assert_int         (ast->while_ptr->body->vector_ptr->length, ==, 1);
    munit_assert_int         (((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->type,                    ==, AST_UNARY);
    munit_assert_ptr_not_null(((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->unary_ptr);
    munit_assert_int         (((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->unary_ptr->opt,          ==, OPERATOR_INCREMENT);
    munit_assert_int         (((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->unary_ptr->operand_type, ==, UNARY_OPERAND_AFTER);
    munit_assert_ptr_not_null(((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->unary_ptr->content);
    munit_assert_string_equal(((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->unary_ptr->content->char_ptr, "a");
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type,                    ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->opt,        ==, OPERATOR_NOT_EQUAL_VALUE);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->type, ==, AST_ASSIGNMENT);
    munit_assert_string_equal(ast->while_ptr->condition->control_ptr->left->assign_ptr->object->char_ptr, "elem");
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->assign_ptr->opt,      ==, OPERATOR_ASSIGN);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->assign_ptr->def_type, ==, KEYWORD_VAR);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->left->assign_ptr->assignment->type, ==, AST_FUNCTION_CALL);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_while_loop_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "while (count < 10) {\n"
                            "    document.write(\"Current Count : \" + count + \"<br />\");\n"
                            "    count++;\n"
                            "}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
    munit_assert_int         (ast->type,                               ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type,              ==, AST_BLOCK);
    munit_assert_int         (ast->while_ptr->body->vector_ptr->length, ==, 2);
    munit_assert_int         (((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->type,                    ==, AST_FUNCTION_CALL);
    munit_assert_int         (((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[1])->type,                    ==, AST_UNARY);
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type,                    ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->opt,        ==, OPERATOR_LESS_THAN);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_while_loop_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "while (count < 10) {\n"
                            "break;\n"
                            "}");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
    munit_assert_int         (ast->type,                               ==, AST_WHILE);
    munit_assert_ptr_not_null(ast->while_ptr->body);
    munit_assert_int         (ast->while_ptr->body->type,              ==, AST_BLOCK);
    munit_assert_int         (ast->while_ptr->body->vector_ptr->length, ==, 1);
    munit_assert_int         (((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->type,    ==, AST_KEYWORD);
    munit_assert_int         (((t_ast_ptr)ast->while_ptr->body->vector_ptr->data[0])->keyword, ==, KEYWORD_BREAK);
    munit_assert_ptr_not_null(ast->while_ptr->condition);
    munit_assert_int         (ast->while_ptr->condition->type,                    ==, AST_CONTROL_OPERATION);
    munit_assert_int         (ast->while_ptr->condition->control_ptr->opt,        ==, OPERATOR_LESS_THAN);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_if_stmt_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "if (true == true) { console.log(true) } else console.log(false)");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
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
    brama_compile(context,  "if (true == true) test = true");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
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
    brama_compile(context,  "if ((function() {test = true})() == true) test = true");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int         (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
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
    brama_compile(context,  "var test = 1;\n"
                            "if (test == -1)\n"
                            "    console.log(\"Test is -1\");\n"
                            "else if (test == 0)\n"
                            "    console.log(\"Test is 0\");\n"
                            "else\n"
                            "    console.log(\"Test is 1\");");
    munit_assert_int         (context->parser->asts->length, ==, 2);

    t_ast_ptr ast = context->parser->asts->data[0];
    munit_assert_int         (ast->type,                                 ==, AST_ASSIGNMENT);

    ast = context->parser->asts->data[1];
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->left->type,  ==, AST_SYMBOL);
    munit_assert_int         (ast->if_stmt_ptr->condition->control_ptr->right->type, ==, AST_PRIMATIVE);

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
    brama_compile(context,  "var test = 1;\n"
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
    brama_compile(context,  "return true");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "var func = (function() {return true})");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
    munit_assert_int(ast->type, ==, AST_ASSIGNMENT);
    munit_assert_int(((t_ast_ptr)ast->assign_ptr->assignment->func_decl_ptr->body->vector_ptr->data[0])->type, ==, AST_RETURN);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "if (true) return false");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "function test () {\n"
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
    brama_compile(context,  "function test () {\n"
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
    brama_compile(context,  "    while(true) \n"
                            "        return false;\n"
                            "    return true;\n");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_7(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "{ \n"
                            "return false;\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_RETURN_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "var func = (function() {return })");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int     (ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
    munit_assert_int     (ast->type, ==, AST_ASSIGNMENT);
    munit_assert_int     (((t_ast_ptr)ast->assign_ptr->assignment->func_decl_ptr->body->vector_ptr->data[0])->type, ==, AST_RETURN);
    munit_assert_ptr_null(((t_ast_ptr)ast->assign_ptr->assignment->func_decl_ptr->body->vector_ptr->data[0])->ast_ptr);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_return_9(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "var func = (function(data) { if (data == null) return; console.log('not null'); })");
    context->parser->index = 0;

    t_ast_ptr ast = NULL;
    munit_assert_int(ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);
    munit_assert_int(ast->type, ==, AST_ASSIGNMENT);
    munit_assert_int(((t_ast_ptr)ast->assign_ptr->assignment->func_decl_ptr->body->vector_ptr->data[0])->type, ==, AST_RETURN);
    CLEAR_AST(ast);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "break");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_BREAK_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "{ \n"
                            "break\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_BREAK_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "if (true) break;");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_BREAK_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_break_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "while (true) {\n"
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
    brama_compile(context,  "var data = 1;\n"
                            "while (true) {\n"
                            "\n"
                            "    if (data != 10)\n"
                            "        console.log(data);\n"
                            "    else\n"
                            "        ++data;\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int         (context->parser->asts->length, ==, 2);

    t_ast_ptr ast = context->parser->asts->data[0];
    munit_assert_int         (ast->type,  ==, AST_ASSIGNMENT);

    ast = context->parser->asts->data[1];
    munit_assert_int         (ast->type,  ==, AST_WHILE);
    munit_assert_int         (ast->while_ptr->condition->type, ==, AST_PRIMATIVE);
    munit_assert_int         (ast->while_ptr->body->type,      ==, AST_BLOCK);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_continue_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "var data = 1;\n"
                            "while (true) {\n"
                            "    continue\n"
                            "}");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int         (context->parser->asts->length, ==, 2);

    t_ast_ptr ast = context->parser->asts->data[0];
    munit_assert_int         (ast->type,  ==, AST_ASSIGNMENT);

    ast = context->parser->asts->data[1];
    munit_assert_int         (ast->type,  ==, AST_WHILE);
    munit_assert_int         (ast->while_ptr->condition->type, ==, AST_PRIMATIVE);
    munit_assert_int         (ast->while_ptr->body->type,      ==, AST_BLOCK);
    munit_assert_int         (ast->while_ptr->body->vector_ptr->length, ==, 1);

    ast = ast->while_ptr->body->vector_ptr->data[0];
    munit_assert_int         (ast->type,    ==, AST_KEYWORD);
    munit_assert_int         (ast->keyword, ==, KEYWORD_CONTINUE);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_continue_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "continue");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_CONTINUE_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_continue_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "if (true) continue");
    munit_assert_int(context->status, == , BRAMA_ILLEGAL_CONTINUE_STATEMENT);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_accessor_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "e.data[0]");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int         (context->parser->asts->length, ==, 1);

    t_ast_ptr ast = context->parser->asts->data[0];
    munit_assert_int         (ast->type,  ==, AST_ACCESSOR);
    munit_assert_ptr_not_null(ast->accessor_ptr);
    munit_assert_ptr_not_null(ast->accessor_ptr->property);
    munit_assert_int         (ast->accessor_ptr->property->type,  ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->accessor_ptr->property->primative_ptr);
    munit_assert_int         (ast->accessor_ptr->property->primative_ptr->type, ==, PRIMATIVE_INTEGER);
    munit_assert_int         (ast->accessor_ptr->property->primative_ptr->int_, ==, 0);

    munit_assert_ptr_not_null(ast->accessor_ptr->object);
    munit_assert_int         (ast->accessor_ptr->object->type,  ==, AST_ACCESSOR);
    munit_assert_ptr_not_null(ast->accessor_ptr->object->accessor_ptr);
    munit_assert_ptr_not_null(ast->accessor_ptr->object->accessor_ptr->object);
    munit_assert_int         (ast->accessor_ptr->object->accessor_ptr->object->type, ==, AST_SYMBOL);
    munit_assert_string_equal(ast->accessor_ptr->object->accessor_ptr->object->char_ptr, "e");

    munit_assert_ptr_not_null(ast->accessor_ptr->object->accessor_ptr->property);
    munit_assert_int         (ast->accessor_ptr->object->accessor_ptr->property->type, ==, AST_SYMBOL);
    munit_assert_string_equal(ast->accessor_ptr->object->accessor_ptr->property->char_ptr, "data");

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_accessor_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "this.data[0]");
    munit_assert_int(context->status, == , BRAMA_OK);
    munit_assert_int         (context->parser->asts->length, ==, 1);

    t_ast_ptr ast = context->parser->asts->data[0];
    munit_assert_int         (ast->type,  ==, AST_ACCESSOR);
    munit_assert_ptr_not_null(ast->accessor_ptr);
    munit_assert_ptr_not_null(ast->accessor_ptr->property);
    munit_assert_int         (ast->accessor_ptr->property->type,  ==, AST_PRIMATIVE);
    munit_assert_ptr_not_null(ast->accessor_ptr->property->primative_ptr);
    munit_assert_int         (ast->accessor_ptr->property->primative_ptr->type, ==, PRIMATIVE_INTEGER);
    munit_assert_int         (ast->accessor_ptr->property->primative_ptr->int_, ==, 0);

    munit_assert_ptr_not_null(ast->accessor_ptr->object);
    munit_assert_int         (ast->accessor_ptr->object->type,  ==, AST_ACCESSOR);
    munit_assert_ptr_not_null(ast->accessor_ptr->object->accessor_ptr);
    munit_assert_ptr_not_null(ast->accessor_ptr->object->accessor_ptr->object);
    munit_assert_int         (ast->accessor_ptr->object->accessor_ptr->object->type, ==, AST_KEYWORD);
    munit_assert_int         (ast->accessor_ptr->object->accessor_ptr->object->keyword, ==, KEYWORD_THIS);

    munit_assert_ptr_not_null(ast->accessor_ptr->object->accessor_ptr->property);
    munit_assert_int         (ast->accessor_ptr->object->accessor_ptr->property->type, ==, AST_SYMBOL);
    munit_assert_string_equal(ast->accessor_ptr->object->accessor_ptr->property->char_ptr, "data");
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_accessor_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context,  "this.this[0]");
    munit_assert_int(context->status, == , BRAMA_FUNCTION_CALL_NOT_VALID);
    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_switch_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_context* context = brama_init();
    brama_compile(context, "var a1=5;\n"
                           "var b1=6;\n"
                           "var r1=0;\n"
                           "\n"
                           "switch(4){\n"
                           "  case 6:\n"
                           "    break;\n"
                           "  case 4:\n"
                           "    break;\n"
                           "  case 7:\n"
                           "    break;\n"
                           "  default:\n"
                           "    break;\n"
                           "}");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "r1", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 0);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_switch_2(const MunitParameter params[], void* user_data_or_fixture) {
t_context* context = brama_init();
    brama_compile(context, "var a1=5;\n"
                           "var b1=6;\n"
                           "var r1=0;\n"
                           "\n"
                           "switch(4){\n"
                           "  default:\n"
                           "    r1 = 1024;\n"
                           "    break;\n"
                           "  case 6:\n"
                           "    r1 = 2;\n"
                           "    break;\n"
                           "  case 4:\n"
                           "    r1 = 42;\n"
                           "    break;\n"
                           "  case 7:\n"
                           "    r1 = 22;\n"
                           "    break;\n"
                           "}");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "r1", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 1024);
    brama_destroy_get_var(context, &var_info);

    brama_destroy(context);
    return MUNIT_OK;
}

MunitResult ast_switch_3(const MunitParameter params[], void* user_data_or_fixture) {
t_context* context = brama_init();
    brama_compile(context, "var a1=5;\n"
                           "var b1=6;\n"
                           "var r1=0;\n"
                           "\n"
                           "switch(6){\n"
                           "  case 6:\n"
                           "    r1 = 4;\n"
                           "  case 4:\n"
                           "    r1 = 42;\n"
                           "    break;\n"
                           "  case 7:\n"
                           "    r1 = 22;\n"
                           "    break;\n"
                           "}");
    brama_run(context);

    t_get_var_info_ptr var_info = NULL;
    brama_status status = brama_get_var(context, "r1", &var_info);
    munit_assert_int   (status,            == , BRAMA_OK);
    munit_assert_int   (var_info->type,    == , CONST_INTEGER);
    munit_assert_int   (var_info->double_, == , 42);
    brama_destroy_get_var(context, &var_info);

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
    ADD_TEST(ast_return_8),
    ADD_TEST(ast_break_1),
    ADD_TEST(ast_break_2),
    ADD_TEST(ast_break_3),
    ADD_TEST(ast_break_4),
    ADD_TEST(ast_break_5),
    ADD_TEST(ast_continue_1),
    ADD_TEST(ast_continue_2),
    ADD_TEST(ast_accessor_1),
    ADD_TEST(ast_accessor_2),
    ADD_TEST(ast_accessor_3),
    ADD_TEST(ast_switch_1),
    ADD_TEST(ast_switch_2),
    ADD_TEST(ast_switch_3),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_AST_2_H
