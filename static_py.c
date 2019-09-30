#include "static_py.h"
#include "brama_internal.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>


/* TOKINIZER OPERATIONS START */
bool isEnd(t_tokinizer_ptr tokinizer) {
    return tokinizer->contentLength <= tokinizer->index;
}

char getChar(t_tokinizer_ptr tokinizer) {
    if (!isEnd(tokinizer))
        return tokinizer->content[tokinizer->index];

    return '\0';
}

char getNextChar(t_tokinizer_ptr tokinizer) {
    if (tokinizer->contentLength > (tokinizer->index + 1))
        return tokinizer->content[tokinizer->index + 1];

    return '\0';
}

char getThirdChar(t_tokinizer_ptr tokinizer) {
    if (tokinizer->contentLength > (tokinizer->index + 2))
        return tokinizer->content[tokinizer->index + 2];

    return '\0';
}


void increase(t_tokinizer_ptr tokinizer) {
    ++tokinizer->index;
    ++tokinizer->column;
}

bool isNewLine(char ch) {
    return ch == '\n';
}

bool isWhitespace(char ch) {
    return (ch == ' ' || ch == '\r' || isNewLine(ch) || ch == '\t');
}

bool isInteger(char ch) {
    return (ch >= '0' && ch <= '9');
}

bool isSymbol(char ch) {
    return ((ch >= 'a' && ch <= 'z') ||
            (ch >= 'A' && ch <= 'Z')) || ch == '_';
}

int getSymbol(t_tokinizer_ptr tokinizer) {
    t_string_stream_ptr stream = string_stream_init();

    char ch;
    while (!isEnd(tokinizer)) {
        ch = getChar(tokinizer);

        if (!isSymbol(ch) && !isInteger(ch) && ch != '_')
            break;

        if (isWhitespace(ch) || ch == '\'' || ch == '"')
            break;

        string_stream_add_char(stream, ch);
        increase(tokinizer);
    }

    char_ptr data       = string_stream_get(stream);
    int_ptr keywordInfo = (int_ptr)map_get(&tokinizer->keywords, data);

    if (keywordInfo) {
        t_token_ptr token = (t_token_ptr)malloc(sizeof (t_token));
        token->type       = TOKEN_KEYWORD;
        token->current    = tokinizer->column;
        token->line       = tokinizer->line;
        token->int_       = *keywordInfo;

        vector_add(tokinizer->tokens, token);
        free(data);
    } else {
        t_token_ptr token  = (t_token_ptr)malloc(sizeof (t_token));
        token->type        = TOKEN_SYMBOL;
        token->current     = tokinizer->column;
        token->line        = tokinizer->line;
        token->char_ptr    = data;

        vector_add(tokinizer->tokens, token);
    }

    string_stream_destroy(stream);
    return STATIC_PY_OK;
}

int getText(t_tokinizer_ptr tokinizer, char symbol) {
    t_string_stream_ptr stream = string_stream_init();

    increase(tokinizer);
    char ch     = getChar(tokinizer);
    char chNext = getNextChar(tokinizer);

    while (!isEnd(tokinizer) && ch != symbol) {
        ch = getChar(tokinizer);
        chNext = getNextChar(tokinizer);

        if (ch == '\\' && chNext == symbol) {
            string_stream_add_char(stream, symbol);
            increase(tokinizer);
        }
        else if (ch == symbol) {
            increase(tokinizer);
            break;
        }
        else
            string_stream_add_char(stream, ch);

        increase(tokinizer);
    }

    if (ch != symbol)
        return STATIC_PY_MISSING_TEXT_DELIMITER;

    t_token_ptr token  = (t_token_ptr)malloc(sizeof (t_token));
    token->type        = TOKEN_TEXT;
    token->current     = tokinizer->column;
    token->line        = tokinizer->line;
    token->char_ptr    = string_stream_get(stream);
    vector_add(tokinizer->tokens, token);

    if (stream->text_length == 0)
        increase(tokinizer);

    string_stream_destroy(stream);
    return STATIC_PY_OK;
}

int getNumber(t_tokinizer_ptr tokinizer) {
    bool isMinus       = false;
    int dotPlace       = 0;
    int beforeTheComma = 0;
    int afterTheComma  = 0;
    size_t start       = tokinizer->column;
    bool isDouble      = false;
    char ch            = getChar(tokinizer);
    char chNext        = getNextChar(tokinizer);

    while (!isEnd(tokinizer)) {
        if (ch == '-') {
            if (isMinus || (beforeTheComma > 0 || afterTheComma > 0))
                break;

            isMinus = true;
        }
        else if (ch == '.') {
            /*if (chNext == '.')
                break;*/

            if (isDouble) {
                return STATIC_PY_MULTIPLE_DOT_ON_DOUBLE;
            }

            isDouble = true;
        }
        else if ((ch >= '0' && ch <= '9')) {
            if (isDouble) {
                ++dotPlace;

                afterTheComma *= pow(10, 1);
                afterTheComma += ch - '0';
            }
            else {
                beforeTheComma *= pow(10, 1);
                beforeTheComma += ch - '0';
            }
        }
        else
            break;

        increase(tokinizer);
        ch     = getChar(tokinizer);
        chNext = getNextChar(tokinizer);
    }

    t_token_ptr token = (t_token_ptr)malloc(sizeof (t_token));

    if (!isDouble){
        token->type = TOKEN_INTEGER;
        token->int_ = beforeTheComma;
    } else {
        token->type    = TOKEN_DOUBLE;
        token->double_ = (beforeTheComma + (afterTheComma * pow(10, -1 * dotPlace)));
    }

    token->current = start;
    token->line    = tokinizer->line;

    if (isMinus) {
        t_token_ptr token = (t_token_ptr)malloc(sizeof (t_token));
        token->type    = TOKEN_OPERATOR;
        token->current = tokinizer->column;
        token->line    = tokinizer->line;
        token->int_    = OPERATOR_SUBTRACTION;

        vector_add(tokinizer->tokens, token);
    }

    vector_add(tokinizer->tokens, token);
    return STATIC_PY_OK;
}

STATIC_PY_STATUS getOperator(t_tokinizer_ptr tokinizer) {
    char ch      = getChar(tokinizer);
    char chNext  = getNextChar(tokinizer);
    char chThird = getThirdChar(tokinizer);

    if (ch == '-' && isInteger(chNext))
        return getNumber(tokinizer);

    increase(tokinizer);
    t_token_ptr token = (t_token_ptr)malloc(sizeof (t_token));
    token->type       = TOKEN_OPERATOR;
    token->current    = tokinizer->column;
    token->line       = tokinizer->line;
    token->int_       = OPERATOR_NONE;

    switch (ch) {
        OPERATOR_CASE_DOUBLE_START_WITH_FOUR('/', '=', '*', '/', OPERATOR_DIVISION, OPERATOR_ASSIGN_DIVISION, OPERATOR_COMMENT_MULTILINE_START, OPERATOR_COMMENT_LINE);

        OPERATOR_CASE_DOUBLE_START_WITH('+', '+', '=', OPERATOR_ADDITION,       OPERATOR_INCREMENT,             OPERATOR_ASSIGN_ADDITION);
        OPERATOR_CASE_DOUBLE_START_WITH('-', '-', '=', OPERATOR_SUBTRACTION,    OPERATOR_DECCREMENT,            OPERATOR_ASSIGN_SUBTRACTION);
        OPERATOR_CASE_DOUBLE_START_WITH('>', '=', '>', OPERATOR_GREATER_THAN,   OPERATOR_GREATER_EQUAL_THAN,    OPERATOR_BITWISE_RIGHT_SHIFT);
        OPERATOR_CASE_DOUBLE_START_WITH('<', '=', '<', OPERATOR_LESS_THAN,      OPERATOR_LESS_EQUAL_THAN,       OPERATOR_BITWISE_LEFT_SHIFT);
        OPERATOR_CASE_DOUBLE_START_WITH('&', '&', '=', OPERATOR_BITWISE_AND,    OPERATOR_AND,                   OPERATOR_BITWISE_AND_ASSIGN);
        OPERATOR_CASE_DOUBLE_START_WITH('|', '|', '=', OPERATOR_BITWISE_OR,     OPERATOR_OR,                    OPERATOR_BITWISE_OR_ASSIGN);
        OPERATOR_CASE_DOUBLE_START_WITH('*', '=', '/', OPERATOR_MULTIPLICATION, OPERATOR_ASSIGN_MULTIPLICATION, OPERATOR_COMMENT_MULTILINE_END);

        OPERATOR_CASE_TRIBLE('=', '=', '=', OPERATOR_ASSIGN, OPERATOR_EQUAL,     OPERATOR_EQUAL_VALUE);
        OPERATOR_CASE_TRIBLE('!', '=', '=', OPERATOR_NOT,    OPERATOR_NOT_EQUAL, OPERATOR_NOT_EQUAL_VALUE);

        OPERATOR_CASE_DOUBLE('%', '=', OPERATOR_MODULES,        OPERATOR_ASSIGN_MODULUS);
        OPERATOR_CASE_DOUBLE('^', '=', OPERATOR_BITWISE_XOR,    OPERATOR_BITWISE_XOR_ASSIGN);

        OPERATOR_CASE_SINGLE('?', OPERATOR_QUESTION_MARK);
        OPERATOR_CASE_SINGLE(':', OPERATOR_COLON_MARK);
        OPERATOR_CASE_SINGLE('~', OPERATOR_BITWISE_NOT);
        OPERATOR_CASE_SINGLE('(', OPERATOR_LEFT_PARENTHESES);
        OPERATOR_CASE_SINGLE(')', OPERATOR_RIGHT_PARENTHESES);
        OPERATOR_CASE_SINGLE('[', OPERATOR_SQUARE_BRACKET_START);
        OPERATOR_CASE_SINGLE(']', OPERATOR_SQUARE_BRACKET_END);
        OPERATOR_CASE_SINGLE('{', OPERATOR_CURVE_BRACKET_START);
        OPERATOR_CASE_SINGLE('}', OPERATOR_CURVE_BRACKET_END);
        OPERATOR_CASE_SINGLE(',', OPERATOR_COMMA);
        OPERATOR_CASE_SINGLE(';', OPERATOR_SEMICOLON);
        OPERATOR_CASE_SINGLE('.', OPERATOR_DOT);
    }
    if (token->int_ == OPERATOR_NONE)
        return STATIC_PY_NOK;

    vector_add(tokinizer->tokens, token);
    return STATIC_PY_OK;
}

int static_py_tokinize(t_context_ptr context, char_ptr data) {
    t_tokinizer_ptr tokinizer = ((t_context_ptr)context)->tokinizer;
    tokinizer->content        = data;
    tokinizer->contentLength  = strlen(data);

    while (!isEnd(tokinizer)) {
        char ch = getChar(tokinizer);

        if (isWhitespace(ch)) {
            while (!isEnd(tokinizer) && isWhitespace(ch)) {
                increase(tokinizer);

                if (isNewLine(ch)) {
                    tokinizer->column = 0;
                    ++tokinizer->line;
                }

                ch = getChar(tokinizer);
            }

            continue;
        } else if (isSymbol(ch)) {
            int state = getSymbol(tokinizer);
            RESULT_CHECK(state);
            continue;
        } else if (ch == '"') {
            int state = getText(tokinizer, '"');
            RESULT_CHECK(state);
            continue;
        } else if (ch == '\'') {
            int state = getText(tokinizer, '\'');
            RESULT_CHECK(state);
            continue;
        } else if (ch >= '0' && ch <= '9') {
            int state = getNumber(tokinizer);
            RESULT_CHECK(state);
            continue;
        } else {
            int state = getOperator(tokinizer);
            RESULT_CHECK(state);
            continue;
        }
    }

    return STATIC_PY_OK;
}

/* TOKINIZER OPERATIONS END */


/* AST PARSER OPERATIONS START */

IS_ITEM(keyword,  TOKEN_KEYWORD)
IS_ITEM(integer,  TOKEN_INTEGER)
IS_ITEM(double,   TOKEN_DOUBLE)
IS_ITEM(text,     TOKEN_TEXT)
IS_ITEM(symbol,   TOKEN_SYMBOL)
IS_ITEM(operator, TOKEN_OPERATOR)

GET_ITEM(keyword,  int_,     int)
GET_ITEM(integer,  int_,     int)
GET_ITEM(double,   double_,  double)
GET_ITEM(text,     char_ptr, char_ptr)
GET_ITEM(symbol,   char_ptr, char_ptr)
GET_ITEM(operator, int_,     int)

#define NEW_AST_DEF(NAME, INPUT, STR_TYPE, TYPE)  \
    t_ast_ptr new_##NAME##_ast(INPUT variable) {  \
        t_ast_ptr ast = malloc(sizeof (t_ast));   \
        ast->type     = STR_TYPE;                 \
        ast-> TYPE    = variable;                 \
        return ast;                               \
    }

NEW_PRIMATIVE_DEF(int,    int,           PRIMATIVE_INTEGER,    int_)
NEW_PRIMATIVE_DEF(double, double,        PRIMATIVE_DOUBLE,     double_)
NEW_PRIMATIVE_DEF(text,   char_ptr,      PRIMATIVE_STRING,     char_ptr)
NEW_PRIMATIVE_DEF(bool,   bool,          PRIMATIVE_BOOL,       bool_)
NEW_PRIMATIVE_DEF(empty,  int,           PRIMATIVE_NULL,       int_)
NEW_PRIMATIVE_DEF(array,  t_vector_ptr,  PRIMATIVE_ARRAY,      array)
NEW_PRIMATIVE_DEF(dict,   map_ast_t_ptr, PRIMATIVE_DICTIONARY, dict)

NEW_AST_DEF(symbol,    char_ptr,        AST_SYMBOL,           char_ptr)
NEW_AST_DEF(unary,     t_unary_ptr,     AST_UNARY,            unary_ptr)
NEW_AST_DEF(binary,    t_binary_ptr,    AST_BINARY_OPERATION, binary_ptr)
NEW_AST_DEF(control,   t_control_ptr,   AST_CONTROL_OPERATION,control_ptr)
NEW_AST_DEF(func_call, t_func_call_ptr, AST_FUNCTION_CALL,    func_call_ptr)

STATIC_PY_STATUS as_primative(t_token_ptr token, t_ast_ptr_ptr ast)
{
    switch (token->type)
    {
    case TOKEN_INTEGER:
        *ast = new_primative_ast_int(get_integer_via_token(token));
        break;

    case TOKEN_DOUBLE:
        *ast = new_primative_ast_double(get_double_via_token(token));
        break;

    case TOKEN_TEXT:
        *ast = new_primative_ast_text(get_text_via_token(token));
        break;

    case TOKEN_KEYWORD:
        if (get_keyword_type(token) == KEYWORD_TRUE)
            *ast = new_primative_ast_bool(true);
        else if (get_keyword_type(token) == KEYWORD_FALSE)
            *ast = new_primative_ast_bool(false);
        else if (get_keyword_type(token) == KEYWORD_NULL)
            *ast = new_primative_ast_empty(0);
        break;

    default:
        return STATIC_PY_PARSE_ERROR;
    }

    return STATIC_PY_OK;
}


bool is_primative(t_token_ptr token)
{
    return token != NULL && (is_text_via_token(token)     ||
                             is_double_via_token(token)   ||
                             is_integer_via_token(token)  ||
                             (is_keyword_via_token(token) && get_keyword_type(token) == KEYWORD_TRUE)  ||
                             (is_keyword_via_token(token) && get_keyword_type(token) == KEYWORD_FALSE) ||
                             (is_keyword_via_token(token) && get_keyword_type(token) == KEYWORD_NULL));
}

STATIC_PY_STATUS ast_primary_expr(t_context_ptr context, t_ast_ptr_ptr ast)
{
    if (is_primative(ast_peek(context)))
    {
        ast_consume(context);
        return as_primative(ast_previous(context), ast);
    }

    if (ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES))
    {
        STATIC_PY_STATUS status = ast_expression(context, ast);
        if (status != STATIC_PY_OK)
            return status;

        if (ast_consume_operator(context, OPERATOR_RIGHT_PARENTHESES) == NULL)
            return STATIC_PY_EXPRESSION_NOT_VALID;

        return STATIC_PY_OK;
    }

    if (ast_match_operator(context, 1, OPERATOR_CURVE_BRACKET_START)) // Parse dictionary
    {
        map_void_t* dictionary = malloc(sizeof (map_void_t));
        map_init(dictionary);
        *ast = new_primative_ast_dict(dictionary);

        if (!ast_check_operator(context, OPERATOR_CURVE_BRACKET_END)) {
            do {
                t_ast_ptr item = NULL;
                char_ptr key   = NULL;

                if (ast_check_token(context, TOKEN_TEXT)) // Should be 'key' or "key"
                    key = get_text_via_token(ast_consume(context));
                else if (ast_check_token(context, TOKEN_SYMBOL)) // should be key
                    key = get_symbol_via_token(ast_consume(context));
                else
                    return STATIC_PY_EXPRESSION_NOT_VALID; // todo: It could accept expression (exp: 1+2)

                if (ast_consume_operator(context, OPERATOR_COLON_MARK) == NULL) // Require ':' operator
                    return STATIC_PY_EXPRESSION_NOT_VALID;

                STATIC_PY_STATUS status = ast_primary_expr(context, &item); // todo: should be also function decleration
                if (status != STATIC_PY_OK)
                    return status;

                map_set(dictionary, key, item);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));
        }

        if (ast_match_operator(context, 1, OPERATOR_CURVE_BRACKET_END) == false)
            return STATIC_PY_EXPRESSION_NOT_VALID;

        return STATIC_PY_OK;
    }

    if (ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_START))
    {
        t_vector_ptr args = vector_init();
        if (!ast_check_operator(context, OPERATOR_SQUARE_BRACKET_END)) {
            do {
                t_ast_ptr item = NULL;
                STATIC_PY_STATUS status = ast_primary_expr(context, &item);
                if (status != STATIC_PY_OK)
                    return status;

                vector_add(args, item->primative_ptr);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));
        }

        ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_END);

        *ast = new_primative_ast_array(args);
        return STATIC_PY_OK;
    }

    return STATIC_PY_EXPRESSION_NOT_VALID;
}

STATIC_PY_STATUS ast_symbol_expr(t_context_ptr context, t_ast_ptr_ptr ast) {
    if (is_symbol_via_token(ast_peek(context)))
    {
        *ast = new_symbol_ast(get_symbol_via_token(ast_consume(context)));
        return STATIC_PY_OK;
    }

    return STATIC_PY_EXPRESSION_NOT_VALID;
}

STATIC_PY_STATUS ast_call(t_context_ptr context, t_ast_ptr_ptr ast) {
    STATIC_PY_STATUS status = ast_primary_expr(context, ast);
    if (status == STATIC_PY_OK)
        return status;

    status = ast_symbol_expr(context, ast);
    if (status != STATIC_PY_OK)
        return status;

    if (ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES))
    {
        char_ptr function = (*ast)->char_ptr;
        t_vector_ptr args = vector_init();

        if (!ast_check_operator(context, OPERATOR_RIGHT_PARENTHESES)) {
            do {
                t_ast_ptr arg = NULL;
                STATIC_PY_STATUS status = ast_primary_expr(context, &arg);
                if (status != STATIC_PY_OK)
                    return status;

                vector_add(args, arg);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));
        }

        if (ast_consume_operator(context, OPERATOR_RIGHT_PARENTHESES) == NULL)
            return STATIC_PY_EXPRESSION_NOT_VALID;

        t_func_call* func_call = malloc(sizeof (t_func_call));
        func_call->args     = args;
        func_call->function = function;
        *ast                = new_func_call_ast(func_call);
    }

    return STATIC_PY_OK;
}


STATIC_PY_STATUS ast_unary_expr(t_context_ptr context, t_ast_ptr_ptr ast) {
    if (ast_match_operator(context, 1, OPERATOR_SUBTRACTION)){
        int operator_type       = get_operator_type(ast_previous(context));
        t_ast_ptr right         = NULL;
        STATIC_PY_STATUS status = ast_unary_expr(context, &right);
        if (status != STATIC_PY_OK)
            return status;

        t_unary_ptr unary  = malloc(sizeof (t_unary));
        unary->operator    = operator_type;
        unary->right       = right;
        *ast = new_unary_ast(unary);
    }

    return ast_call(context, ast);
}

STATIC_PY_STATUS ast_declaration_stmt(t_context_ptr context, t_ast_ptr_ptr ast) {
    ++context->parser->index;
    *ast = malloc(sizeof (t_ast));
    return STATIC_PY_OK;
}

STATIC_PY_STATUS ast_control_expr(t_context_ptr context, t_ast_ptr_ptr ast) {
    STATIC_PY_STATUS left_status = ast_addition_expr(context, ast);
    if (left_status != STATIC_PY_OK)
        return left_status;

    while (ast_match_operator(context, 8, OPERATOR_EQUAL, OPERATOR_EQUAL_VALUE, OPERATOR_GREATER_THAN, OPERATOR_GREATER_EQUAL_THAN, OPERATOR_LESS_THAN, OPERATOR_LESS_EQUAL_THAN, OPERATOR_NOT_EQUAL, OPERATOR_NOT_EQUAL_VALUE)) {
        int opt                       = get_operator_via_token(ast_previous(context));
        t_ast_ptr right               = NULL;
        STATIC_PY_STATUS right_status = ast_addition_expr(context, &right);
        if (right_status != STATIC_PY_OK)
            return right_status;

        t_control_ptr binary = malloc(sizeof(t_control));
        binary->left        = *ast;
        binary->operator    = opt;
        binary->right       = right;
        *ast = new_control_ast(binary);
    }

    return STATIC_PY_OK;
}

STATIC_PY_STATUS ast_addition_expr(t_context_ptr context, t_ast_ptr_ptr ast) {
    STATIC_PY_STATUS left_status = ast_multiplication_expr(context, ast);
    if (left_status != STATIC_PY_OK)
        return left_status;

    while (ast_match_operator(context, 2, OPERATOR_ADDITION, OPERATOR_SUBTRACTION)) {
        int opt                       = get_operator_via_token(ast_previous(context));
        t_ast_ptr right               = NULL;
        STATIC_PY_STATUS right_status = ast_multiplication_expr(context, &right);
        if (right_status != STATIC_PY_OK)
            return right_status;

        t_binary_ptr binary = malloc(sizeof(t_binary));
        binary->left        = *ast;
        binary->operator    = opt;
        binary->right       = right;
        *ast = new_binary_ast(binary);
    }

    return STATIC_PY_OK;
}

STATIC_PY_STATUS ast_multiplication_expr(t_context_ptr context, t_ast_ptr_ptr ast) {
    STATIC_PY_STATUS left_status = ast_unary_expr(context, ast);
    if (left_status != STATIC_PY_OK)
        return left_status;

    while (ast_match_operator(context, 2, OPERATOR_DIVISION, OPERATOR_MULTIPLICATION)) {
        int opt                       = get_operator_via_token(ast_previous(context));
        t_ast_ptr right               = NULL;
        STATIC_PY_STATUS right_status = ast_unary_expr(context, &right);
        if (right_status != STATIC_PY_OK)
            return right_status;

        t_binary_ptr binary = malloc(sizeof(t_binary));
        binary->left        = *ast;
        binary->operator    = opt;
        binary->right       = right;
        *ast = new_binary_ast(binary);
    }

    return STATIC_PY_OK;
}

STATIC_PY_STATUS ast_expression(t_context_ptr context, t_ast_ptr_ptr ast) {
    return ast_addition_expr(context, ast);
}

t_token_ptr ast_consume(t_context_ptr context) {
    if (!ast_is_at_end(context))
    {
        ++context->parser->index;
        return ast_previous(context);
    }
    return NULL;
}

bool ast_is_at_end(t_context_ptr context) {
    return ast_peek(context) == NULL;
}

t_token_ptr ast_previous(t_context_ptr context) {
    return vector_get(context->tokinizer->tokens, context->parser->index - 1);
}

t_token_ptr ast_peek(t_context_ptr context) {
    return vector_get(context->tokinizer->tokens, context->parser->index);
}

bool ast_check_token(t_context_ptr context, int token_type) {
    t_token_ptr token = ast_peek(context);
    return token != NULL && token->type == token_type;
}

bool ast_check_operator(t_context_ptr context, int operator_type) {
    t_token_ptr token = ast_peek(context);
    return token != NULL && token->type == TOKEN_OPERATOR && token->int_ == operator_type;
}

bool ast_check_keyword(t_context_ptr context, int keyword_type) {
    t_token_ptr token = ast_peek(context);
    return token != NULL && token->type == TOKEN_KEYWORD && token->int_ == keyword_type;
}

bool ast_match_token(t_context_ptr context, int count, ...) {
    va_list a_list;
    va_start(a_list, count);

    for (size_t i = 0; i < count; ++i) {
        int arg = va_arg (a_list, int);
        if (ast_check_token(context, arg)) {
            ast_consume(context);
            return 1;
        }
    }
    return 0;
}

bool ast_match_operator(t_context_ptr context, int count, ...) {
    va_list a_list;
    va_start(a_list, count);

    for (size_t i = 0; i < count; ++i) {
        int arg = va_arg (a_list, int);
        if (ast_check_operator(context, arg)) {
            ast_consume(context);
            return 1;
        }
    }
    return 0;
}

bool ast_match_keyword(t_context_ptr context, int count, ...) {
    va_list a_list;
    va_start(a_list, count);

    for (size_t i = 0; i < count; ++i) {
        int arg = va_arg (a_list, int);
        if (ast_check_keyword(context, arg)) {
            ast_consume(context);
            return 1;
        }
    }
    return 0;
}

t_token_ptr ast_consume_operator(t_context_ptr context, int operator_type) {
    if (ast_check_operator(context, operator_type)) return ast_consume(context);
    return NULL;
}

t_token_ptr ast_consume_token(t_context_ptr context, int token_type) {
    if (ast_check_token(context, token_type)) return ast_consume(context);
    return NULL;
}

t_token_ptr ast_consume_keyword(t_context_ptr context, int keyword_type) {
    if (ast_check_keyword(context, keyword_type)) return ast_consume(context);
    return NULL;
}

STATIC_PY_STATUS ast_parser(t_context_ptr context) {
    context->parser->index = 0;
    while (!ast_is_at_end(context)) {
        t_ast_ptr ast              = NULL;
        STATIC_PY_STATUS status = ast_declaration_stmt(context, &ast);
        if (status == STATIC_PY_OK)
            vector_add(context->parser->asts, ast);
        else
            return status;
    }

    return STATIC_PY_OK;
}


/* AST PARSER OPERATIONS END */

t_context_ptr static_py_init() {
    t_context_ptr context             = (t_context_ptr)malloc(sizeof(t_context));
    context->error_message            = NULL;

    /* tokinizer */
    context->tokinizer                = (t_tokinizer_ptr)malloc(sizeof(t_tokinizer));
    context->tokinizer->column        = 0;
    context->tokinizer->index         = 0;
    context->tokinizer->line          = 1;
    context->tokinizer->tokens        = vector_init();

    /* parser */
    context->parser                   = (t_parser_ptr)malloc(sizeof (t_parser));
    context->parser->asts             = vector_init();

    /* keywords */
    map_init(&context->tokinizer->keywords);

    size_t keywordCount = sizeof (KEYWORDS_PAIR) / sizeof(KeywordPair);
    for (size_t i = 0; i < keywordCount; ++i)
        map_set(&context->tokinizer->keywords, KEYWORDS_PAIR[i].name,  KEYWORDS_PAIR[i].keyword);

    return context;
}

char_ptr static_py_set_error(t_context_ptr context, int error) {
    if (error == STATIC_PY_MISSING_TEXT_DELIMITER) {
        char_ptr buffer = malloc(sizeof(char) * 128);
        sprintf(buffer, "Missing Delimiter at Line: %d, Column: %d", context->tokinizer->line, context->tokinizer->column);
        return buffer;
    } else if (error == STATIC_PY_MULTIPLE_DOT_ON_DOUBLE) {
        char_ptr buffer = malloc(sizeof(char) * 128);
        sprintf(buffer, "Multiple dot used for double: %d, Column: %d", context->tokinizer->line, context->tokinizer->column);
        return buffer;
    }

    return NULL;
}

void static_py_execute(t_context_ptr context, char_ptr data) {
    STATIC_PY_STATUS tokinizer_status = static_py_tokinize(context, data);
    if (tokinizer_status != STATIC_PY_OK) {
        context->status = tokinizer_status;
        if (context->error_message != NULL && strlen(context->error_message) > 0)
            free(context->error_message);

        context->error_message = static_py_set_error(context, tokinizer_status);
        return;
    }

    STATIC_PY_STATUS ast_status = ast_parser(context);
    if (ast_status != STATIC_PY_OK) {
        context->status        = ast_status;
        context->error_message = static_py_set_error(context, ast_status);
        return;
    }
    context->status = STATIC_PY_OK;
}

void static_py_dump(t_context_ptr context) {
    t_context_ptr _context = (t_context_ptr)context;
    int i;
    int totalToken = _context->tokinizer->tokens->count;
    for (i = 0; i < totalToken; i++) {
        t_token_ptr token = (t_token_ptr)vector_get(_context->tokinizer->tokens, i);
        if (token->type == TOKEN_TEXT)
            printf("TEXT     = '%s'\r\n", token->char_ptr);
        else if (token->type == TOKEN_OPERATOR)
            printf("OPERATOR = '%s'\r\n", OPERATORS[token->int_].operator);
        else if (token->type == TOKEN_SYMBOL)
            printf("SYMBOL   = '%s'\r\n", token->char_ptr);
        else if (token->type == TOKEN_KEYWORD)
            printf("KEYWORD  = '%s'\r\n", KEYWORDS[token->int_]);
        else if (token->type == TOKEN_INTEGER)
            printf("INTEGER  = '%d'\r\n", token->int_);
        else if (token->type == TOKEN_DOUBLE)
            printf("DOUBLE   = '%d'\r\n", token->double_);
    }
}

void static_py_destroy(t_context_ptr context) {
    t_context_ptr _context = (t_context_ptr)context;
    size_t i;
    size_t totalToken = _context->tokinizer->tokens->count;
    for (i = 0; i < totalToken; i++) {
        t_token_ptr token = (t_token_ptr)vector_get(_context->tokinizer->tokens, i);
        if (token->type == TOKEN_TEXT ||
            token->type == TOKEN_SYMBOL)
            free((char_ptr)token->char_ptr);
    }

    map_deinit(&_context->tokinizer->keywords);
    free(_context->tokinizer);
    free(_context);
}
