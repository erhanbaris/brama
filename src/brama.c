#include "brama.h"
#include "brama_internal.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <brama.h>


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
    return (ch == ' ' || ch == '\r' || ch == '\t');
}

bool isInteger(char ch) {
    return (ch >= '0' && ch <= '9');
}

bool isSymbol(char ch) {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
            ch == '_'               ||
            ch == '$';
}

int getSymbol(t_tokinizer_ptr tokinizer) {
    t_string_stream_ptr stream = string_stream_init();

    char ch;
    while (!isEnd(tokinizer)) {
        ch = getChar(tokinizer);

        if (!isSymbol(ch) && !isInteger(ch))
            break;

        if (isWhitespace(ch) || ch == '\'' || ch == '"')
            break;

        string_stream_add_char(stream, ch);
        increase(tokinizer);
    }

    char_ptr data       = NULL;
    string_stream_get(stream, &data);
    int_ptr keywordInfo = (int_ptr)map_get(&tokinizer->keywords, data);

    if (keywordInfo) {
        t_token_ptr token = (t_token_ptr)BRAMA_MALLOC(sizeof (t_token));
        token->type       = TOKEN_KEYWORD;
        token->current    = tokinizer->column;
        token->line       = tokinizer->line;
        token->int_       = *keywordInfo;

        vec_push(tokinizer->tokens, token);
        BRAMA_FREE(data);
    } else {
        t_token_ptr token  = (t_token_ptr)BRAMA_MALLOC(sizeof (t_token));
        token->type        = TOKEN_SYMBOL;
        token->current     = tokinizer->column;
        token->line        = tokinizer->line;
        token->char_ptr    = data;

        vec_push(tokinizer->tokens, token);
    }

    string_stream_destroy(stream);
    BRAMA_FREE(stream);
    return BRAMA_OK;
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

    if (ch != symbol) {
        string_stream_destroy(stream);
        BRAMA_FREE(stream);
        return BRAMA_MISSING_TEXT_DELIMITER;
    }

    t_token_ptr token  = (t_token_ptr)BRAMA_MALLOC(sizeof (t_token));
    token->type        = TOKEN_TEXT;
    token->current     = tokinizer->column;
    token->line        = tokinizer->line;
    token->char_ptr    = NULL;
    string_stream_get(stream, &token->char_ptr);
    vec_push(tokinizer->tokens, token);

    if (stream->text_length == 0)
        increase(tokinizer);

    string_stream_destroy(stream);
    BRAMA_FREE(stream);
    return BRAMA_OK;
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
                return BRAMA_MULTIPLE_DOT_ON_DOUBLE;
            }

            isDouble = true;
        }
        else if ((ch >= '0' && ch <= '9')) {
            if (isDouble) {
                ++dotPlace;

                afterTheComma *= (int)pow(10, 1);
                afterTheComma += ch - '0';
            }
            else {
                beforeTheComma *= (int)pow(10, 1);
                beforeTheComma += ch - '0';
            }
        }
        else
            break;

        increase(tokinizer);
        ch     = getChar(tokinizer);
        chNext = getNextChar(tokinizer);
    }

    t_token_ptr token = (t_token_ptr)BRAMA_MALLOC(sizeof (t_token));

    if (!isDouble) {
        token->type = TOKEN_INTEGER;
        token->int_ = beforeTheComma;
    } else {
        token->type    = TOKEN_DOUBLE;
        token->double_ = (beforeTheComma + (afterTheComma * pow(10, -1 * dotPlace)));
    }

    token->current = start;
    token->line    = tokinizer->line;

    if (isMinus) {
        t_token_ptr token = (t_token_ptr)BRAMA_MALLOC(sizeof (t_token));
        token->type    = TOKEN_OPERATOR;
        token->current = tokinizer->column;
        token->line    = tokinizer->line;
        token->int_    = OPERATOR_SUBTRACTION;

        vec_push(tokinizer->tokens, token);
    }

    vec_push(tokinizer->tokens, token);
    return BRAMA_OK;
}

brama_status getOperator(t_tokinizer_ptr tokinizer) {
    char ch      = getChar(tokinizer);
    char chNext  = getNextChar(tokinizer);
    char chThird = getThirdChar(tokinizer);

    if (ch == '-' && isInteger(chNext))
        return getNumber(tokinizer);

    increase(tokinizer);
    t_token_ptr token = (t_token_ptr)BRAMA_MALLOC(sizeof (t_token));
    token->type       = TOKEN_OPERATOR;
    token->current    = tokinizer->column;
    token->line       = tokinizer->line;
    token->int_       = OPERATOR_NONE;

    switch (ch) {
        
        /* 
         * This is special case that is why coded like that.
         * '>>>' is requre all tree token could be same but others just looking first two char 
         */
        case '>' : {
            if (chNext == '>') {
                if (chThird == '>') {
                    token->int_ = OPERATOR_BITWISE_UNSIGNED_RIGHT_SHIFT;
                    increase(tokinizer);
                }
                else 
                    token->int_ = OPERATOR_BITWISE_RIGHT_SHIFT;
                increase(tokinizer);

            } else if (chNext == '=' ) {
                token->int_ = OPERATOR_GREATER_EQUAL_THAN;
                increase(tokinizer);
            } else token->int_ = OPERATOR_GREATER_THAN;
        }
        break;

        OPERATOR_CASE_DOUBLE_START_WITH_FOUR('/', '=', '*', '/', OPERATOR_DIVISION, OPERATOR_ASSIGN_DIVISION, OPERATOR_COMMENT_MULTILINE_START, OPERATOR_COMMENT_LINE);

        OPERATOR_CASE_DOUBLE_START_WITH('+', '+', '=', OPERATOR_ADDITION,       OPERATOR_INCREMENT,             OPERATOR_ASSIGN_ADDITION);
        OPERATOR_CASE_DOUBLE_START_WITH('-', '-', '=', OPERATOR_SUBTRACTION,    OPERATOR_DECCREMENT,            OPERATOR_ASSIGN_SUBTRACTION);
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
        return BRAMA_NOK;

    vec_push(tokinizer->tokens, token);
    return BRAMA_OK;
}

int brama_tokinize(t_context_ptr context, char_ptr data) {
    t_tokinizer_ptr tokinizer = ((t_context_ptr)context)->tokinizer;
    tokinizer->content        = data;
    tokinizer->contentLength  = strlen(data);

    while (!isEnd(tokinizer)) {
        char ch     = getChar(tokinizer);
        char chNext = getNextChar(tokinizer);

        if (isNewLine(ch)) {
            t_token_ptr token = (t_token_ptr)BRAMA_MALLOC(sizeof (t_token));
            token->type    = TOKEN_OPERATOR;
            token->current = tokinizer->column;
            token->line    = tokinizer->line;
            token->int_    = OPERATOR_NEW_LINE;

            vec_push(tokinizer->tokens, token);
            tokinizer->column = 0;
            ++tokinizer->line;
            increase(tokinizer);
        }
        else if (isWhitespace(ch)) {
            while (!isEnd(tokinizer) && isWhitespace(ch)) {
                increase(tokinizer);

                if (isNewLine(ch)) {
                    tokinizer->column = 0;
                    ++tokinizer->line;
                }

                ch = getChar(tokinizer);
            }

            continue;
        } else if (ch == '/' && chNext == '/') {
            while (!isEnd(tokinizer) && !isNewLine(ch)) {
                increase(tokinizer);

                if (isNewLine(ch)) {
                    tokinizer->column = 0;
                    ++tokinizer->line;
                }

                ch = getChar(tokinizer);
            }

            continue;
        } else if (ch == '/' && chNext == '*') {
            while (!isEnd(tokinizer) && ch != '*' && chNext != '/') {
                increase(tokinizer);

                if (isNewLine(ch)) {
                    tokinizer->column = 0;
                    ++tokinizer->line;
                }

                ch     = getChar(tokinizer);
                chNext = getNextChar(tokinizer);
            }

            continue;
        }
        else if (isSymbol(ch)) {
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

    return BRAMA_OK;
}

/* TOKINIZER OPERATIONS END */


/* AST PARSER OPERATIONS START */

IS_ITEM(keyword,  TOKEN_KEYWORD)
IS_ITEM(integer,  TOKEN_INTEGER)
IS_ITEM(double,   TOKEN_DOUBLE)
IS_ITEM(text,     TOKEN_TEXT)
IS_ITEM(symbol,   TOKEN_SYMBOL)
IS_ITEM(operator, TOKEN_OPERATOR)

GET_ITEM(keyword,  int_,     brama_keyword_type)
GET_ITEM(integer,  int_,     int)
GET_ITEM(double,   double_,  double)
GET_ITEM(text,     char_ptr, char_ptr)
GET_ITEM(symbol,   char_ptr, char_ptr)
GET_ITEM(operator, int_,     brama_operator_type)

NEW_PRIMATIVE_DEF(int,    int,                 PRIMATIVE_INTEGER,    int_)
NEW_PRIMATIVE_DEF(double, double,              PRIMATIVE_DOUBLE,     double_)
NEW_PRIMATIVE_DEF(text,   char_ptr,            PRIMATIVE_STRING,     char_ptr)
NEW_PRIMATIVE_DEF(bool,   bool,                PRIMATIVE_BOOL,       bool_)
NEW_PRIMATIVE_DEF(empty,  int,                 PRIMATIVE_NULL,       int_)
NEW_PRIMATIVE_DEF(array,  vec_t_ast_ptr_t_ptr, PRIMATIVE_ARRAY,      array)
NEW_PRIMATIVE_DEF(dict,   map_ast_t_ptr,       PRIMATIVE_DICTIONARY, dict)

NEW_AST_DEF(symbol,    char_ptr,              AST_SYMBOL,               char_ptr)
NEW_AST_DEF(unary,     t_unary_ptr,           AST_UNARY,                unary_ptr)
NEW_AST_DEF(binary,    t_binary_ptr,          AST_BINARY_OPERATION,     binary_ptr)
NEW_AST_DEF(control,   t_control_ptr,         AST_CONTROL_OPERATION,    control_ptr)
NEW_AST_DEF(assign,    t_assign_ptr,          AST_ASSIGNMENT,           assign_ptr)
NEW_AST_DEF(func_call, t_func_call_ptr,       AST_FUNCTION_CALL,        func_call_ptr)
NEW_AST_DEF(func_decl, t_func_decl_ptr,       AST_FUNCTION_DECLARATION, func_decl_ptr)
NEW_AST_DEF(block,     vec_t_ast_ptr_t_ptr ,  AST_BLOCK,                vector_ptr)
NEW_AST_DEF(object,    t_object_creation_ptr, AST_OBJECT_CREATION,      object_creation_ptr)
NEW_AST_DEF(while,     t_while_loop_ptr ,     AST_WHILE,                while_ptr)
NEW_AST_DEF(if,        t_if_stmt_ptr,         AST_IF_STATEMENT,         if_stmt_ptr)
NEW_AST_DEF(return,    t_ast_ptr,             AST_RETURN,               ast_ptr)
NEW_AST_DEF(accessor,  t_accessor_ptr,        AST_ACCESSOR,             accessor_ptr)
NEW_AST_DEF(keyword,   brama_keyword_type,    AST_KEYWORD,              keyword)

brama_status as_primative(t_token_ptr token, t_ast_ptr_ptr ast) {
    switch (token->type) {
    case TOKEN_INTEGER:
        *ast = new_primative_ast_int(get_integer(token));
        break;

    case TOKEN_DOUBLE:
        *ast = new_primative_ast_double(get_double(token));
        break;

    case TOKEN_TEXT:
        *ast = new_primative_ast_text(get_text(token));
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
        return BRAMA_PARSE_ERROR;
    }

    return BRAMA_OK;
}


bool is_primative(t_token_ptr token) {
    return token != NULL && (is_text(token)     ||
                             is_double(token)   ||
                             is_integer(token)  ||
                             (is_keyword(token) && get_keyword_type(token) == KEYWORD_TRUE)  ||
                             (is_keyword(token) && get_keyword_type(token) == KEYWORD_FALSE) ||
                             (is_keyword(token) && get_keyword_type(token) == KEYWORD_NULL));
}

brama_status ast_primary_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    if (is_primative(ast_peek(context))) {
        ast_consume(context);
        return as_primative(ast_previous(context), ast);
    }

    BACKUP_PARSER_INDEX();
    if (ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES)) {
        brama_status status = ast_expression(context, ast, extra_data);
        if (status != BRAMA_OK)
            DESTROY_AST_AND_RETURN(status, *ast);

        if (ast_consume_operator(context, OPERATOR_RIGHT_PARENTHESES) == NULL)
            DESTROY_AST_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND, *ast);

        return BRAMA_OK;
    }

    if (ast_match_operator(context, 1, OPERATOR_CURVE_BRACKET_START)) { // Parse dictionary
        map_void_t* dictionary = BRAMA_MALLOC(sizeof (map_void_t));
        map_init(dictionary);
        *ast = new_primative_ast_dict(dictionary);

        if (!ast_check_operator(context, OPERATOR_CURVE_BRACKET_END)) {
            do {
                t_ast_ptr item = NULL;
                char_ptr key   = NULL;

                if (ast_check_token(context, TOKEN_TEXT)) // Should be 'key' or "key"
                    key = get_text(ast_consume(context));
                else if (ast_check_token(context, TOKEN_SYMBOL)) // should be key
                    key = get_symbol(ast_consume(context));
                else {
                    CLEAR_AST(*ast);
                    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DICTIONARY_NOT_VALID); // todo: It could accept expression (exp: 1+2)
                }

                if (ast_consume_operator(context, OPERATOR_COLON_MARK) == NULL) {// Require ':' operator
                    CLEAR_AST(*ast);
                    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DICTIONARY_NOT_VALID);
                }

                brama_status status = ast_assignable(context, &item, extra_data); // todo: should be also function decleration
                if (status != BRAMA_OK) {
                    CLEAR_AST(*ast);
                    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DICTIONARY_NOT_VALID);
                }

                map_set(dictionary, key, item);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));
        }

        if (ast_match_operator(context, 1, OPERATOR_CURVE_BRACKET_END) == false) {
            CLEAR_AST(*ast);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND);
        }

        return BRAMA_OK;
    }

    if (ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_START)) {
        vec_t_ast_ptr_t_ptr args = BRAMA_MALLOC(sizeof (vec_t_ast_ptr_t));
        vec_init(args);

        if (!ast_check_operator(context, OPERATOR_SQUARE_BRACKET_END)) {
            do {
                t_ast_ptr item = NULL;
                brama_status status = ast_expression(context, &item, extra_data);
                if (status != BRAMA_OK) {
                    destroy_ast_vector(args);
                    BRAMA_FREE(args);
                    destroy_ast(item);
                    BRAMA_FREE(item);
                    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_ARRAY_NOT_VALID);
                }

                vec_push(args, item);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));
        }

        if (ast_consume_operator(context, OPERATOR_SQUARE_BRACKET_END) == NULL) {
            destroy_ast_vector(args);
            BRAMA_FREE(args);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND);
        }

        *ast = new_primative_ast_array(args);
        return BRAMA_OK;
    }

    return BRAMA_DOES_NOT_MATCH_AST;
}

brama_status ast_symbol_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    if (is_symbol(ast_peek(context))) {
        *ast = new_symbol_ast(get_symbol(ast_consume(context)));
        return BRAMA_OK;
    }

    return BRAMA_DOES_NOT_MATCH_AST;
}

brama_status ast_accessor_stmt(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    if (!is_symbol(ast_peek(context)) && (!is_keyword(ast_peek(context)) || get_keyword(ast_peek(context)) != KEYWORD_THIS))
        RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);

    brama_status status    = BRAMA_NOK;
    t_token_ptr token      = NULL;

    while (true) {
        t_ast_ptr tmp_ast = NULL;
        token = ast_consume(context);
        if (*ast != NULL != 0 && !is_symbol(token)) {
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_FUNCTION_CALL_NOT_VALID);
        }

        if (is_symbol(token))
            tmp_ast = new_symbol_ast(get_symbol(token));
        else
            tmp_ast = new_keyword_ast(get_keyword(token));

        if (*ast != NULL) {
            t_accessor_ptr accessor = BRAMA_MALLOC(sizeof (t_accessor));
            accessor->object        = *ast;
            accessor->property      = tmp_ast;
            *ast                    = new_accessor_ast(accessor);
        }
        else
            *ast = tmp_ast;

        status = BRAMA_OK;

        if (!ast_match_operator(context, 1, OPERATOR_DOT))
            break;
    }

    if (ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_START)) {
        if (ast_check_operator(context, OPERATOR_SQUARE_BRACKET_END)) {
            CLEAR_AST(*ast);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_ILLEGAL_ACCESSOR_STATEMENT);
        }

        t_ast_ptr indexer = NULL;
        brama_status inner_status = ast_assignable(context, &indexer, extra_data);
        if (inner_status != BRAMA_OK) {
            CLEAR_AST(*ast);
            RESTORE_PARSER_INDEX_AND_RETURN(inner_status);
        }

        if (ast_consume_operator(context, OPERATOR_SQUARE_BRACKET_END) == NULL) {
            CLEAR_AST(*ast);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_ILLEGAL_ACCESSOR_STATEMENT);
        }

        t_accessor_ptr accessor = BRAMA_MALLOC(sizeof (t_accessor));
        accessor->object        = *ast;
        accessor->property      = indexer;
        *ast                    = new_accessor_ast(accessor);
        status = BRAMA_OK;
    }

    if (status == BRAMA_OK)
        return BRAMA_OK;

    CLEAR_AST(*ast);
    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}

brama_status ast_call(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status status = ast_assignment_expr(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;

    status = ast_function_decleration(context, ast, (void*)FUNC_DEF_ASSIGNMENT);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_primary_expr(context, ast, extra_data);
    if (status == BRAMA_OK || status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    BACKUP_PARSER_INDEX();

    status = ast_accessor_stmt(context, ast, extra_data);

    if (status != BRAMA_OK && !is_symbol(ast_peek(context)))
        RESTORE_PARSER_INDEX_AND_RETURN(status);

    if (status != BRAMA_OK)
        *ast = ast_symbol_expr(context, ast, extra_data);

    /* We are parsing function parameters */
    if (ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES)) {
        vec_t_ast_ptr_t_ptr args = BRAMA_MALLOC(sizeof (vec_t_ast_ptr_t));
        vec_init(args);

        if (!ast_check_operator(context, OPERATOR_RIGHT_PARENTHESES)) {
            do {
                t_ast_ptr arg = NULL;
                brama_status inner_status = ast_assignable(context, &arg, extra_data);
                if (inner_status != BRAMA_OK) {
                    CLEAR_VECTOR(args);
                    RESTORE_PARSER_INDEX_AND_RETURN(inner_status);
                }

                vec_push(args, arg);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));
        }

        if (ast_consume_operator(context, OPERATOR_RIGHT_PARENTHESES) == NULL) {
            CLEAR_VECTOR(args);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND);
        }

        t_func_call* func_call = BRAMA_MALLOC(sizeof (t_func_call));
        func_call->args        = args;
        func_call->function    = *ast;
        func_call->type        = FUNC_CALL_NORMAL;
        *ast                   = new_func_call_ast(func_call);
        status = BRAMA_OK;
    }

    if (ast_match_operator(context, 1, OPERATOR_SQUARE_BRACKET_START)) {
        if (ast_check_operator(context, OPERATOR_SQUARE_BRACKET_END)) {
            CLEAR_AST(*ast);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_ILLEGAL_ACCESSOR_STATEMENT);
        }

        t_ast_ptr indexer = NULL;
        brama_status inner_status = ast_assignable(context, &indexer, extra_data);
        if (inner_status != BRAMA_OK) {
            CLEAR_AST(*ast);
            RESTORE_PARSER_INDEX_AND_RETURN(inner_status);
        }

        if (ast_consume_operator(context, OPERATOR_SQUARE_BRACKET_END) == NULL) {
            CLEAR_AST(*ast);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_ILLEGAL_ACCESSOR_STATEMENT);
        }

        t_accessor_ptr accessor = BRAMA_MALLOC(sizeof (t_accessor));
        accessor->object        = *ast;
        accessor->property      = indexer;
        *ast                    = new_accessor_ast(accessor);
        status = BRAMA_OK;
    }

    if (status == BRAMA_OK)
        return status;

    CLEAR_AST(*ast);
    RESTORE_PARSER_INDEX();

    status = ast_symbol_expr(context, ast, extra_data);
    if (status == BRAMA_OK)
        return status;

    CLEAR_AST(*ast);
    return BRAMA_DOES_NOT_MATCH_AST;
}

brama_status ast_block_multiline_stmt(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    if (ast_match_operator(context, 1, OPERATOR_CURVE_BRACKET_START)) { // Is it start with '{'
        vec_t_ast_ptr_t_ptr blocks = BRAMA_MALLOC(sizeof (vec_t_ast_ptr_t));
        vec_init(blocks);

        if (!ast_match_operator(context, 1, OPERATOR_CURVE_BRACKET_END)) {
            do {
                t_ast_ptr block = NULL;
                brama_status status = ast_declaration_stmt(context, &block, extra_data);
                if (status != BRAMA_OK) {
                    CLEAR_AST(block);
                    destroy_ast_vector(blocks);
                    BRAMA_FREE(blocks);
                    RESTORE_PARSER_INDEX_AND_RETURN(status);
                }

                if (!ast_match_operator(context, 1, OPERATOR_SEMICOLON) && !is_next_new_line(context)) { // Require semicolon or new line
                    if (!ast_is_at_end(context) && !ast_check_operator(context, OPERATOR_CURVE_BRACKET_END)) {
                        destroy_ast_vector(blocks);
                        BRAMA_FREE(blocks);
                        CLEAR_AST(block);

                        if (ast_match_operator(context, 1, OPERATOR_COLON_MARK)) /* Is it dictionary? Is current token ':'?*/
                            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
                        RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_SEMICOLON_REQUIRED);
                    }
                }

                if (block->type == AST_PRIMATIVE || block->type == AST_FUNCTION_CALL) {
                    if (!is_next_new_line(context) &&
                        !ast_check_operator(context, OPERATOR_CURVE_BRACKET_END) &&
                        !ast_check_operator(context, OPERATOR_SEMICOLON)) {
                        destroy_ast_vector(blocks);
                        BRAMA_FREE(blocks);
                        CLEAR_AST(block);
                        RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_BLOCK_NOT_VALID);
                    }
                }

                vec_push(blocks, block);
            } while (!ast_is_at_end(context) && !ast_match_operator(context, 1, OPERATOR_CURVE_BRACKET_END));

            if (!is_operator(ast_previous(context)) || ast_previous(context)->opt != OPERATOR_CURVE_BRACKET_END) {
                destroy_ast_vector(blocks);
                BRAMA_FREE(blocks);
                DESTROY_AST_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND, *ast);
            }
        }

        *ast = new_block_ast(blocks);
        return BRAMA_OK;
    }

    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}

brama_status ast_function_decleration(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    bool anony_func = false;
    if (ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES))
        anony_func = true;

    if (ast_match_keyword(context, 1, KEYWORD_FUNCTION)) {
        char_ptr function_name = NULL;
        if (is_symbol(ast_peek(context))) {
            function_name = get_symbol(ast_consume(context));
        }

        if ((extra_data == NULL || (int*)extra_data != FUNC_DEF_ASSIGNMENT) && !anony_func && (function_name == NULL || strlen(function_name) == 0))
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_FUNCTION_NAME_REQUIRED);

        if (!ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES))
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_OPEN_OPERATOR_NOT_FOUND);

        vec_t_ast_ptr_t_ptr args = BRAMA_MALLOC(sizeof(vec_t_ast_ptr_t));
        vec_init(args);

        if (!ast_check_operator(context, OPERATOR_RIGHT_PARENTHESES)) {
            do {
                t_ast_ptr arg = NULL;
                brama_status status = ast_symbol_expr(context, &arg, extra_data);
                if (status != BRAMA_OK) {
                    destroy_ast_vector(args);
                    BRAMA_FREE(args);
                    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_ILLEGAL_FUNCTION_ARGUMENT);
                }

                vec_push(args, arg);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));
        }

        if (ast_consume_operator(context, OPERATOR_RIGHT_PARENTHESES) == NULL) {
            destroy_ast_vector(args);
            BRAMA_FREE(args);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND);
        }

        t_ast_ptr body = NULL;
        brama_status body_status = ast_block_multiline_stmt(context, &body, AST_IN_FUNCTION);
        if (body_status != BRAMA_OK) {
            destroy_ast_vector(args);
            BRAMA_FREE(args);
            CLEAR_AST(body);
            RESTORE_PARSER_INDEX_AND_RETURN(body_status);
        }

        if (anony_func && ast_match_operator(context, 1, OPERATOR_RIGHT_PARENTHESES) == false) {
            destroy_ast_vector(args);
            BRAMA_FREE(args);
            CLEAR_AST(body);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND);
        }

        t_func_decl_ptr func_decl = (t_func_decl_ptr)BRAMA_MALLOC(sizeof(t_func_decl));
        func_decl->args           = args;
        func_decl->body           = body;
        func_decl->name           = function_name;
        *ast = new_func_decl_ast(func_decl);

        if (ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES)) { // If anonymous function directly calling
            vec_t_ast_ptr_t_ptr call_args = BRAMA_MALLOC(sizeof(vec_t_ast_ptr_t));
            vec_init(args);

            if (!ast_check_operator(context, OPERATOR_RIGHT_PARENTHESES)) {
                do {
                    t_ast_ptr arg = NULL;
                    brama_status status = ast_assignable(context, &arg, extra_data);
                    if (status != BRAMA_OK) {
                        destroy_ast_vector(call_args);
                        BRAMA_FREE(call_args);
                        CLEAR_AST(*ast);
                        CLEAR_AST(arg);
                        RESTORE_PARSER_INDEX_AND_RETURN(status);
                    }

                    vec_push(call_args, arg);
                } while (ast_match_operator(context, 1, OPERATOR_COMMA));
            }

            if (ast_consume_operator(context, OPERATOR_RIGHT_PARENTHESES) == NULL) {
                destroy_ast_vector(call_args);
                BRAMA_FREE(call_args);
                CLEAR_AST(*ast);
                RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND);
            }

            t_func_call_ptr func_call = (t_func_call_ptr)BRAMA_MALLOC(sizeof (t_func_call));
            func_call->args           = call_args;
            func_call->func_decl_ptr  = (*ast)->func_decl_ptr;
            func_call->type           = FUNC_CALL_ANONY;

            BRAMA_FREE((*ast)); // t_ast_ptr not need any more
            *ast                      = new_func_call_ast(func_call);
            return BRAMA_OK;
        }

        return BRAMA_OK;
    }

    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}


brama_status ast_unary_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    brama_operator_type operator_type   = OPERATOR_NONE;
    brama_unary_operant_type unary_type = UNARY_OPERAND_BEFORE;
    t_ast_ptr unary_content             = NULL;
    brama_status status                 = BRAMA_NOK;

    /* Looking for : --i ++i !i -i */
    if (ast_match_operator(context, 5, OPERATOR_SUBTRACTION, OPERATOR_INCREMENT, OPERATOR_DECCREMENT, OPERATOR_NOT, OPERATOR_BITWISE_NOT)) {
        operator_type = get_operator_type(ast_previous(context));

        /* Looking for: -10 -10.1 */
        if (operator_type == OPERATOR_SUBTRACTION &&
           (is_integer(ast_peek(context)) || is_double(ast_peek(context)))) {
            status = ast_primary_expr(context, &unary_content, extra_data);
        }
        else {
            status = ast_call(context, &unary_content, extra_data);
        }

        if (status != BRAMA_OK) {
            destroy_ast(unary_content);
            BRAMA_FREE(unary_content);
            RESTORE_PARSER_INDEX_AND_RETURN(status);
        }

        if ((operator_type == OPERATOR_INCREMENT || operator_type == OPERATOR_DECCREMENT) && unary_content->type != AST_SYMBOL) {
            destroy_ast(unary_content);
            BRAMA_FREE(unary_content);
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_EXPRESSION_NOT_VALID);
        }

        t_unary_ptr unary   = BRAMA_MALLOC(sizeof (t_unary));
        unary->operand_type = UNARY_OPERAND_BEFORE;
        unary->opt          = operator_type;
        unary->content      = unary_content;
        *ast = new_unary_ast(unary);
        return BRAMA_OK;
    }

    /* Looking for : i++ i--
    todo: not working, fix it*/

    status = ast_call(context, ast, extra_data);

    if (status == BRAMA_OK && (*ast)->type == AST_SYMBOL && (ast_check_operator(context, OPERATOR_INCREMENT) ||  ast_check_operator(context, OPERATOR_DECCREMENT))) {
        unary_type    = UNARY_OPERAND_AFTER;
        operator_type = get_operator_type(ast_consume(context));
        status = ast_primary_expr(context, &unary_content, extra_data);

        t_unary_ptr unary   = BRAMA_MALLOC(sizeof (t_unary));
        unary->operand_type = UNARY_OPERAND_AFTER;
        unary->opt          = operator_type;
        unary->content      = *ast;
        *ast = new_unary_ast(unary);
        return BRAMA_OK;
    }

    if (status == BRAMA_OK && (*ast)->type == AST_PRIMATIVE && (ast_check_operator(context, OPERATOR_INCREMENT) ||  ast_check_operator(context, OPERATOR_DECCREMENT))) {
        CLEAR_AST(*ast);
        RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_INVALID_UNARY_EXPRESSION);
    }
    return status;
}

brama_status ast_declaration_stmt(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status status = BRAMA_NOK;

    status = ast_function_decleration(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_block_multiline_stmt(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_while_loop(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_if_stmt(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_new_object(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_return_stmt(context, ast, extra_data);
    if (status == BRAMA_OK) {
        if (extra_data & AST_IN_FUNCTION)
            return BRAMA_OK;
        return BRAMA_ILLEGAL_RETURN_STATEMENT;
    }
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_continue(context, ast, extra_data);
    if (status == BRAMA_OK) {
        if (extra_data & AST_IN_LOOP)
            return BRAMA_OK;
        return BRAMA_ILLEGAL_CONTINUE_STATEMENT;
    }
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_break(context, ast, extra_data);
    if (status == BRAMA_OK) {
        if (extra_data & AST_IN_LOOP)
            return BRAMA_OK;
        return BRAMA_ILLEGAL_BREAK_STATEMENT;

    }
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_expression(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    return BRAMA_DOES_NOT_MATCH_AST;
}

brama_status ast_control_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_bitwise_shift_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 4, OPERATOR_GREATER_THAN, OPERATOR_GREATER_EQUAL_THAN, OPERATOR_LESS_THAN, OPERATOR_LESS_EQUAL_THAN)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_bitwise_shift_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_control_ptr binary = BRAMA_MALLOC(sizeof(t_control));
        binary->left         = *ast;
        binary->opt          = opt;
        binary->right        = right;
        *ast = new_control_ast(binary);
    }

    return BRAMA_OK;
}

brama_status ast_equality_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_control_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 4, OPERATOR_EQUAL, OPERATOR_EQUAL_VALUE, OPERATOR_NOT_EQUAL, OPERATOR_NOT_EQUAL_VALUE)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_control_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_control_ptr binary = BRAMA_MALLOC(sizeof(t_control));
        binary->left         = *ast;
        binary->opt          = opt;
        binary->right        = right;
        *ast = new_control_ast(binary);
    }

    return BRAMA_OK;
}


brama_status ast_and_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_bitwise_or_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 1, OPERATOR_AND)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_bitwise_or_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_control_ptr control = BRAMA_MALLOC(sizeof(t_control));
        control->left         = *ast;
        control->opt          = opt;
        control->right        = right;
        *ast = new_control_ast(control);
    }

    return BRAMA_OK;
}

brama_status ast_bitwise_and_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_equality_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 1, OPERATOR_BITWISE_AND)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_equality_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_binary_ptr binary = BRAMA_MALLOC(sizeof(t_binary));
        binary->left         = *ast;
        binary->opt          = opt;
        binary->right        = right;
        *ast = new_binary_ast(binary);
    }

    return BRAMA_OK;
}

brama_status ast_bitwise_or_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_bitwise_xor_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 1, OPERATOR_BITWISE_OR)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_bitwise_xor_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_binary_ptr binary = BRAMA_MALLOC(sizeof(t_binary));
        binary->left         = *ast;
        binary->opt          = opt;
        binary->right        = right;
        *ast = new_binary_ast(binary);
    }

    return BRAMA_OK;
}

brama_status ast_bitwise_xor_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_bitwise_and_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 1, OPERATOR_BITWISE_XOR)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_bitwise_and_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_binary_ptr binary = BRAMA_MALLOC(sizeof(t_binary));
        binary->left         = *ast;
        binary->opt          = opt;
        binary->right        = right;
        *ast = new_binary_ast(binary);
    }

    return BRAMA_OK;
}

brama_status ast_or_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_and_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 1, OPERATOR_OR)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_and_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_control_ptr control = BRAMA_MALLOC(sizeof(t_control));
        control->left         = *ast;
        control->opt          = opt;
        control->right        = right;
        *ast = new_control_ast(control);
    }

    return BRAMA_OK;
}


brama_status ast_bitwise_shift_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_addition_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 3, OPERATOR_BITWISE_LEFT_SHIFT, OPERATOR_BITWISE_RIGHT_SHIFT, OPERATOR_BITWISE_UNSIGNED_RIGHT_SHIFT)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_addition_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_binary_ptr binary = BRAMA_MALLOC(sizeof(t_binary));
        binary->left        = *ast;
        binary->opt         = opt;
        binary->right       = right;
        *ast = new_binary_ast(binary);
    }

    return BRAMA_OK;
}

brama_status ast_addition_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_multiplication_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 2, OPERATOR_ADDITION, OPERATOR_SUBTRACTION)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_multiplication_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_binary_ptr binary = BRAMA_MALLOC(sizeof(t_binary));
        binary->left        = *ast;
        binary->opt         = opt;
        binary->right       = right;
        *ast = new_binary_ast(binary);
    }

    return BRAMA_OK;
}

brama_status ast_multiplication_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_unary_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 2, OPERATOR_DIVISION, OPERATOR_MULTIPLICATION)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_unary_expr(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            return right_status;
        }

        t_binary_ptr binary = BRAMA_MALLOC(sizeof(t_binary));
        binary->left        = *ast;
        binary->opt         = opt;
        binary->right       = right;
        *ast = new_binary_ast(binary);
    }

    return BRAMA_OK;
}

brama_status ast_assignment_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX()

    bool new_def            = false;
    brama_keyword_type type = KEYWORD_VAR;
    if (ast_match_keyword(context, 3, KEYWORD_VAR, KEYWORD_LET, KEYWORD_CONST)) {
        type = get_keyword(ast_previous(context));
        new_def = true;
    }

    brama_status status = ast_accessor_stmt(context, ast, extra_data);

    if (status != BRAMA_OK && !is_symbol(ast_peek(context)))
        DESTROY_AST_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST, *ast);

    if (status != BRAMA_OK)
        *ast = new_symbol_ast(get_symbol(ast_consume(context)));

    if (ast_match_operator(context, 6, OPERATOR_ASSIGN, OPERATOR_ASSIGN_ADDITION, OPERATOR_ASSIGN_DIVISION, OPERATOR_ASSIGN_MODULUS, OPERATOR_ASSIGN_MULTIPLICATION, OPERATOR_ASSIGN_SUBTRACTION)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_assignable(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            CLEAR_AST(*ast);
            return right_status;
        }

        //ast_consume_operator(context, OPERATOR_SEMICOLON);

        t_assign_ptr assign = BRAMA_MALLOC(sizeof(t_assign));
        assign->object      = *ast;
        assign->def_type    = type;
        assign->opt         = opt;
        assign->assignment  = right;
        assign->new_def     = new_def;
        *ast = new_assign_ast(assign);
    }
    else
        DESTROY_AST_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST, *ast);

    return BRAMA_OK;
}

brama_status ast_continue(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    if (ast_match_keyword(context, 1, KEYWORD_CONTINUE)) {
        *ast = new_keyword_ast(KEYWORD_CONTINUE);
        return BRAMA_OK;
    }

    return BRAMA_DOES_NOT_MATCH_AST;
}

brama_status ast_break(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    if (ast_match_keyword(context, 1, KEYWORD_BREAK)) {
        *ast = new_keyword_ast(KEYWORD_BREAK);
        return BRAMA_OK;
    }

    return BRAMA_DOES_NOT_MATCH_AST;
}

brama_status ast_return_stmt(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    if (ast_match_keyword(context, 1, KEYWORD_RETURN)) {
        t_ast_ptr statement = NULL;
        if (check_end_of_line(context, END_LINE_CHECKER_SEMICOLON) == BRAMA_OK || ast_check_operator(context, OPERATOR_CURVE_BRACKET_END)) {
            *ast = new_return_ast(statement);
            return BRAMA_OK;
        }

        brama_status status = ast_assignable(context, &statement, extra_data);
        if (status != BRAMA_OK)
            DESTROY_AST_AND_RETURN(status, statement);

        *ast = new_return_ast(statement);
        return BRAMA_OK;
    }

    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}

brama_status ast_new_object(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    if (ast_match_keyword(context, 1, KEYWORD_NEW)) {
        if (!is_symbol(ast_peek(context)))
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_NEW_CLASS_CREATION_NOT_VALID);

        char_ptr object_name = get_symbol(ast_consume(context));
        if (!ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES))
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_NEW_CLASS_CREATION_NOT_VALID);

        vec_t_ast_ptr_t_ptr args = BRAMA_MALLOC(sizeof (vec_t_ast_ptr_t));
        vec_init(args);

        if (!ast_match_operator(context, 1, OPERATOR_RIGHT_PARENTHESES)) {
            do {
                t_ast_ptr arg = NULL;
                brama_status status = ast_assignable(context, &arg, extra_data);
                if (status != BRAMA_OK) {
                    destroy_ast_vector(args);
                    BRAMA_FREE(args);
                    RESTORE_PARSER_INDEX_AND_RETURN(status);
                }

                vec_push(args, arg);
            } while (ast_match_operator(context, 1, OPERATOR_COMMA));

            if (!ast_match_operator(context, 1, OPERATOR_RIGHT_PARENTHESES)) {
                vec_deinit(args);
                BRAMA_FREE(args);
                RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_CLOSE_OPERATOR_NOT_FOUND);
            }
        }

        t_object_creation_ptr object = (t_object_creation_ptr)BRAMA_MALLOC(sizeof(t_object_creation));
        object->object_name = object_name;
        object->args        = args;
        *ast = new_object_ast(object);
        return BRAMA_OK;
    }

    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}

brama_status ast_while_loop(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    if (ast_match_keyword(context, 1, KEYWORD_WHILE)) {
        if (!ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES))
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_OPEN_OPERATOR_NOT_FOUND);

        t_ast_ptr condition = NULL;
        brama_status condition_status = ast_assignable(context, &condition, extra_data | AST_IN_LOOP);
        if (condition_status != BRAMA_OK) {
            CLEAR_AST(condition);
            RESTORE_PARSER_INDEX_AND_RETURN(condition_status);
        }

        if (!ast_match_operator(context, 1, OPERATOR_RIGHT_PARENTHESES)){
            destroy_ast(condition);
            BRAMA_FREE(condition);
            condition = NULL;
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_OPEN_OPERATOR_NOT_FOUND);
        }

        t_ast_ptr body = NULL;
        brama_status body_status = ast_block_multiline_stmt(context, &body, extra_data | AST_IN_LOOP);
        if (body_status == BRAMA_DOES_NOT_MATCH_AST) {
            body_status = ast_block_body(context, &body, extra_data | AST_IN_LOOP);
            if (body_status != BRAMA_OK) {
                CLEAR_AST(condition);
                CLEAR_AST(body);
                RESTORE_PARSER_INDEX_AND_RETURN(body_status)
            }
        }
        else if (condition_status != BRAMA_OK) {
            CLEAR_AST(condition);
            CLEAR_AST(body);
            RESTORE_PARSER_INDEX_AND_RETURN(condition_status)
        }

        t_while_loop_ptr object = (t_while_loop_ptr)BRAMA_MALLOC(sizeof(t_while_loop));
        object->body            = body;
        object->condition       = condition;
        *ast = new_while_ast(object);
        return BRAMA_OK;
    }

    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}

brama_status ast_if_stmt(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    if (ast_match_keyword(context, 1, KEYWORD_IF)) {
        check_end_of_line(context, END_LINE_CHECKER_NEWLINE); /* Clear new line */

        if (!ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES))
            RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_OPEN_OPERATOR_NOT_FOUND);

        /* If statement parts */
        t_ast_ptr condition  = NULL;
        t_ast_ptr true_body  = NULL;
        t_ast_ptr false_body = NULL;

        brama_status condition_status = ast_expression(context, &condition, extra_data);
        if (condition_status != BRAMA_OK)
            DESTROY_AST_AND_RETURN(condition_status, condition);

        if (!ast_match_operator(context, 1, OPERATOR_RIGHT_PARENTHESES))
            DESTROY_AST_AND_RETURN(BRAMA_OPEN_OPERATOR_NOT_FOUND, condition);

        brama_status body_status = ast_block_multiline_stmt(context, &true_body, extra_data);
        if (body_status == BRAMA_DOES_NOT_MATCH_AST) {
            body_status = ast_declaration_stmt(context, &true_body, extra_data);
            if (body_status != BRAMA_OK) {
                CLEAR_AST(true_body);
                CLEAR_AST(condition);
                RESTORE_PARSER_INDEX_AND_RETURN(body_status)
            }
        }
        else if (condition_status != BRAMA_OK) {
            CLEAR_AST(true_body);
            CLEAR_AST(condition);
            RESTORE_PARSER_INDEX_AND_RETURN(condition_status)
        }

        check_end_of_line(context, END_LINE_CHECKER_NEWLINE | END_LINE_CHECKER_SEMICOLON);
        if (ast_match_keyword(context, 1, KEYWORD_ELSE)) {
            body_status = ast_block_multiline_stmt(context, &false_body, extra_data);
            if (body_status == BRAMA_DOES_NOT_MATCH_AST) {
                body_status = ast_declaration_stmt(context, &false_body, extra_data);
                if (body_status != BRAMA_OK) {
                    CLEAR_AST(true_body);
                    CLEAR_AST(false_body);
                    CLEAR_AST(condition);
                    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_BODY_NOT_FOUND)
                }
            }
            else if (condition_status != BRAMA_OK) {
                CLEAR_AST(true_body);
                CLEAR_AST(false_body);
                CLEAR_AST(condition);
                RESTORE_PARSER_INDEX_AND_RETURN(condition_status)
            }
        }

        t_if_stmt_ptr object = (t_if_stmt_ptr)BRAMA_MALLOC(sizeof(t_if_stmt));
        object->true_body  = true_body;
        object->false_body = false_body;
        object->condition  = condition;
        *ast = new_if_ast(object);
        return BRAMA_OK;
    }

    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}

brama_status ast_assignable(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status status = BRAMA_NOK;

    status = ast_new_object(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_or_expr(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    return BRAMA_DOES_NOT_MATCH_AST;
}

brama_status check_end_of_line(t_context_ptr context, int operators) {
    if (operators == 0 || context->tokinizer->tokens->length <= context->parser->index)
        return BRAMA_OK;

    t_token_ptr token = context->tokinizer->tokens->data[context->parser->index];
    int index = 0;
    brama_status status = BRAMA_SEMICOLON_REQUIRED;
    do {
        /* If there is no more token, it means we are end of tokens and we do not event need to validate */
        if (token == NULL)
            return BRAMA_OK;

        bool is_semicolon = operators & END_LINE_CHECKER_SEMICOLON && token->opt == OPERATOR_SEMICOLON;
        bool is_newline   = operators & END_LINE_CHECKER_NEWLINE   && token->opt == OPERATOR_NEW_LINE;

        /* If there is no token or it is not one of the required operator, respose error code */
        if (token->type != TOKEN_OPERATOR || (!is_semicolon && !is_newline))
            break;

        /* Is semi colon required, if yes, consume all semicolons */
        if (is_semicolon) {
            ++context->parser->index;
            status = BRAMA_OK;
        }

        /* Is end line required, if yes, consume all end lines */
        if (is_newline) {
            ++context->parser->index;
            status = BRAMA_OK;
        }
        ++index;

        if (context->tokinizer->tokens->length <= context->parser->index)
            return BRAMA_OK;

        token = context->tokinizer->tokens->data[context->parser->index];
    } while(true); /* Continue until next_token is null or operator not valid anymore */

    return status;
}

brama_status ast_block_body(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status status = BRAMA_NOK;

    status = ast_assignable(context, ast, extra_data);
    if (status == BRAMA_OK)
        goto check_semicolon_and_new_line;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_return_stmt(context, ast, extra_data);
    if (status == BRAMA_OK) {
        if (extra_data & AST_IN_FUNCTION)
            goto check_semicolon_and_new_line;
        return BRAMA_ILLEGAL_RETURN_STATEMENT;

    }
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_continue(context, ast, extra_data);
    if (status == BRAMA_OK) {
        if (extra_data & AST_IN_LOOP)
            goto check_semicolon_and_new_line;
        return BRAMA_ILLEGAL_CONTINUE_STATEMENT;

    }
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_break(context, ast, extra_data);
    if (status == BRAMA_OK) {
        if (extra_data & AST_IN_LOOP)
            goto check_semicolon_and_new_line;
        return BRAMA_ILLEGAL_BREAK_STATEMENT;

    }
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    return BRAMA_DOES_NOT_MATCH_AST;

    check_semicolon_and_new_line: // It looks not good, I know, I will fix it
    status = check_end_of_line(context, END_LINE_CHECKER_NEWLINE | END_LINE_CHECKER_SEMICOLON);
    if (status != BRAMA_OK)
        return status;

    return BRAMA_OK;
}

brama_status ast_expression(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    BACKUP_PARSER_INDEX();

    brama_status status = ast_or_expr(context, ast, extra_data);
    if (status == BRAMA_OK)
        return BRAMA_OK;
    else if (status != BRAMA_DOES_NOT_MATCH_AST)
        return status;

    status = ast_function_decleration(context, ast, (void*)FUNC_DEF_ASSIGNMENT);
    if (status == BRAMA_OK && (*ast)->type == AST_FUNCTION_CALL)
        return BRAMA_OK;

    RESTORE_PARSER_INDEX_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST);
}

t_token_ptr ast_consume(t_context_ptr context) {
    if (!ast_is_at_end(context)) {
        ++context->parser->index;
        return ast_previous(context);
    }
    return NULL;
}

bool ast_is_at_end(t_context_ptr context) {
    return ast_peek(context) == NULL;
}

t_token_ptr ast_previous(t_context_ptr context) {
    if (context->tokinizer->tokens->length <= (context->parser->index - 1))
        return NULL;

    return context->tokinizer->tokens->data[context->parser->index - 1];
}

bool is_next_new_line(t_context_ptr context) {
    t_token_ptr token = ast_peek(context);
    if (token != NULL && is_operator(token) && token->opt == OPERATOR_NEW_LINE)
        return true;

    token = context->tokinizer->tokens->data[context->parser->index - 1];
    if (token != NULL && is_operator(token) && token->opt == OPERATOR_NEW_LINE)
        return true;

    return false;
}

t_token_ptr ast_peek(t_context_ptr context) {
    check_end_of_line(context, END_LINE_CHECKER_NEWLINE);
    if (context->tokinizer->tokens->length <= context->parser->index)
        return NULL;
    return context->tokinizer->tokens->data[context->parser->index];
}

t_token_ptr ast_next(t_context_ptr context) {
    return context->tokinizer->tokens->data[context->parser->index + 1];
}

bool ast_check_token(t_context_ptr context, brama_token_type token_type) {
    t_token_ptr token = ast_peek(context);
    return token != NULL && token->type == token_type;
}

bool ast_check_operator(t_context_ptr context, brama_operator_type operator_type) {
    t_token_ptr token = ast_peek(context);
    return token != NULL && token->type == TOKEN_OPERATOR && token->int_ == operator_type;
}

bool ast_check_keyword(t_context_ptr context, brama_keyword_type keyword_type) {
    t_token_ptr token = ast_peek(context);
    return token != NULL && token->type == TOKEN_KEYWORD && token->int_ == keyword_type;
}

bool ast_match_token(t_context_ptr context, size_t count, ...) {
    va_list a_list;
    va_start(a_list, count);

    for (size_t i = 0; i < count; ++i) {
        int arg = va_arg (a_list, int);
        if (ast_check_token(context, arg)) {
            ast_consume(context);
            return true;
        }
    }
    return false;
}

bool ast_match_operator(t_context_ptr context, size_t count, ...) {
    va_list a_list;
    va_start(a_list, count);

    for (size_t i = 0; i < count; ++i) {
        int arg = va_arg (a_list, int);
        if (ast_check_operator(context, arg)) {
            ast_consume(context);
            return true;
        }
    }
    return false;
}

bool ast_match_keyword(t_context_ptr context, size_t count, ...) {
    va_list a_list;
    va_start(a_list, count);

    for (size_t i = 0; i < count; ++i) {
        int arg = va_arg (a_list, int);
        if (ast_check_keyword(context, arg)) {
            ast_consume(context);
            return true;
        }
    }
    return false;
}

t_token_ptr ast_consume_operator(t_context_ptr context, brama_operator_type operator_type) {
    if (ast_check_operator(context, operator_type)) return ast_consume(context);
    return NULL;
}

t_token_ptr ast_consume_token(t_context_ptr context, brama_token_type token_type) {
    if (ast_check_token(context, token_type)) return ast_consume(context);
    return NULL;
}

t_token_ptr ast_consume_keyword(t_context_ptr context, brama_keyword_type keyword_type) {
    if (ast_check_keyword(context, keyword_type)) return ast_consume(context);
    return NULL;
}

brama_status validate_ast(t_context_ptr context, t_ast_ptr_ptr ast) {
    if (ast == NULL || *ast == NULL)
        return BRAMA_NOK;

    if (((*ast)->type == AST_PRIMATIVE ||
        (*ast)->type == AST_FUNCTION_CALL ||
        (*ast)->type == AST_BINARY_OPERATION ||
        (*ast)->type == AST_CONTROL_OPERATION ||
        (*ast)->type == AST_UNARY ||
        (*ast)->type == AST_FUNCTION_DECLARATION) &&
        (!is_next_new_line(context) &&
         !ast_is_at_end(context) &&
         !ast_check_operator(context, OPERATOR_SEMICOLON)))
            return BRAMA_BLOCK_NOT_VALID;
    return BRAMA_OK;
}

brama_status ast_parser(t_context_ptr context) {
    context->parser->index = 0;
    while (!ast_is_at_end(context)) {

        /* In that situation, we don't need new line or semi colon. Clean all */
        check_end_of_line(context, END_LINE_CHECKER_NEWLINE | END_LINE_CHECKER_SEMICOLON);
        if (ast_peek(context) == NULL)
            return BRAMA_OK;

        t_ast_ptr ast       = NULL;
        brama_status status = ast_declaration_stmt(context, &ast, AST_IN_NONE);
        if (status == BRAMA_OK) {
            status = validate_ast(context, &ast);
            if (status == BRAMA_OK)
                vec_push(context->parser->asts, ast);
            else {
                CLEAR_AST(ast);
                return status;
            }
        }
        else {
            CLEAR_AST(ast);
            return status;
        }
    }

    return BRAMA_OK;
}


/* AST PARSER OPERATIONS END */

t_context_ptr brama_init() {
    t_context_ptr context      = (t_context_ptr)BRAMA_MALLOC(sizeof(t_context));
    context->error_message     = NULL;

    /* tokinizer */
    context->tokinizer         = (t_tokinizer_ptr)BRAMA_MALLOC(sizeof(t_tokinizer));
    context->tokinizer->column = 0;
    context->tokinizer->index  = 0;
    context->tokinizer->line   = 1;
    context->tokinizer->tokens = BRAMA_MALLOC(sizeof (vec_t_token_ptr_t));
    vec_init(context->tokinizer->tokens);

    /* parser */
    context->parser            = (t_parser_ptr)BRAMA_MALLOC(sizeof (t_parser));
    context->parser->index     = 0;
    context->parser->line      = 0;
    context->parser->asts      = BRAMA_MALLOC(sizeof (vec_t_ast_ptr_t));
    vec_init(context->parser->asts);

    /* keywords */
    map_init(&context->tokinizer->keywords);

    size_t keywordCount = sizeof (KEYWORDS_PAIR) / sizeof(KeywordPair);
    for (size_t i = 0; i < keywordCount; ++i)
        map_set(&context->tokinizer->keywords, KEYWORDS_PAIR[i].name,  KEYWORDS_PAIR[i].keyword);

    /* Compiler */
    context->compiler            = (t_compiler_ptr)BRAMA_MALLOC(sizeof(t_compiler));
    context->compiler->index     = 0;
    context->compiler->op_codes  = BRAMA_MALLOC(sizeof (vec_t_brama_opcode_t));
    context->compiler->constants = BRAMA_MALLOC(sizeof (vec_const_item));
    vec_init(context->compiler->op_codes);
    vec_init(context->compiler->constants);

    return context;
}

char_ptr brama_set_error(t_context_ptr context, int error) {
    if (error == BRAMA_MISSING_TEXT_DELIMITER) {
        char_ptr buffer = BRAMA_MALLOC(sizeof(char) * 128);
        sprintf(buffer, "Missing Delimiter at Line: %zu, Column: %zu", context->tokinizer->line, context->tokinizer->column);
        return buffer;
    } else if (error == BRAMA_MULTIPLE_DOT_ON_DOUBLE) {
        char_ptr buffer = BRAMA_MALLOC(sizeof(char) * 128);
        sprintf(buffer, "Multiple dot used for double: %zu, Column: %zu", context->tokinizer->line, context->tokinizer->column);
        return buffer;
    } else if (error == BRAMA_EXPRESSION_NOT_VALID) {
        char_ptr buffer = BRAMA_MALLOC(sizeof(char) * 128);
        sprintf(buffer, "Expression not valid\r\nLine:%zu, Column: %zu", context->tokinizer->line, context->tokinizer->column);
        return buffer;
    }

    return NULL;
}

void brama_execute(t_context_ptr context, char_ptr data) {
    brama_status tokinizer_status = brama_tokinize(context, data);
    if (tokinizer_status != BRAMA_OK) {
        context->status = tokinizer_status;
        if (context->error_message != NULL && strlen(context->error_message) > 0)
            BRAMA_FREE(context->error_message);

        context->error_message = brama_set_error(context, tokinizer_status);
        return;
    }

    brama_status ast_status = ast_parser(context);
    if (ast_status != BRAMA_OK) {
        context->status        = ast_status;
        context->error_message = brama_set_error(context, ast_status);
        return;
    }

    compile(context);
    run(context);
    context->status = BRAMA_OK;
}

void brama_dump(t_context_ptr context) {
    t_context_ptr _context = (t_context_ptr)context;
    int i;
    int totalToken = _context->tokinizer->tokens->length;
    for (i = 0; i < totalToken; ++i) {
        t_token_ptr token = _context->tokinizer->tokens->data[i];
        if (token->type == TOKEN_TEXT)
            printf("TEXT     = '%s'\r\n", token->char_ptr);
        else if (token->type == TOKEN_OPERATOR)
            printf("OPERATOR = '%s'\r\n", OPERATORS[token->int_].opt);
        else if (token->type == TOKEN_SYMBOL)
            printf("SYMBOL   = '%s'\r\n", token->char_ptr);
        else if (token->type == TOKEN_KEYWORD)
            printf("KEYWORD  = '%s'\r\n", KEYWORDS[token->int_]);
        else if (token->type == TOKEN_INTEGER)
            printf("INTEGER  = '%d'\r\n", token->int_);
        else if (token->type == TOKEN_DOUBLE)
            printf("DOUBLE   = '%f'\r\n", token->double_);
    }
}

#define LEVEL_PADDING 2
#define AST_DUMP_START()                           printf("{")
#define AST_DUMP_END()                             printf("\n%*s}", (level) * LEVEL_PADDING, "")
#define AST_DUMP_AST(AST)                          brama_dump_ast_internal( AST , level + 1)
#define AST_PRINT_PROPERTY(PROPERTY, DATA)         printf("\r\n%*s%s : '%s'", (level + 1) * LEVEL_PADDING, "", PROPERTY , DATA )
#define AST_PRINT_PROPERTY_DECIMAL(PROPERTY, DATA) printf("\r\n%*s%s : '%d'", (level + 1) * LEVEL_PADDING, "", PROPERTY , DATA )
#define AST_PRINT_PROPERTY_FLOAT(PROPERTY, DATA)   printf("\r\n%*s%s : '%f'", (level + 1) * LEVEL_PADDING, "", PROPERTY , DATA )
#define AST_PRINT_SECTION(PROPERTY)                printf("\r\n%*s%s : ", (level + 1) * LEVEL_PADDING, "", PROPERTY)
#define AST_PRINT_SIMPLE(PROPERTY)                 printf("%s", PROPERTY)

void brama_dump_vector_internal(vec_t_ast_ptr_t_ptr vector, size_t level) {
    int i     = 0;
    int total = vector->length;
    AST_PRINT_SIMPLE("[");
    for (i = 0; i < total; ++i) {
        t_ast_ptr ast = vector->data[i];
        AST_DUMP_AST(ast);
    }
    AST_PRINT_SIMPLE("]");
}

void brama_dump_ast_internal(t_ast_ptr ast, size_t level) {
    brama_ast_type type = ast->type;
    switch (type)
    {
    case AST_IF_STATEMENT:
        AST_DUMP_START();
        AST_PRINT_PROPERTY("type", "IF_STATEMENT");
        AST_PRINT_SECTION("condition");
        AST_DUMP_AST(ast->if_stmt_ptr->condition);
        AST_PRINT_SECTION("true body");
        AST_DUMP_AST(ast->if_stmt_ptr->true_body);

        if (ast->if_stmt_ptr->false_body != NULL) {
            AST_PRINT_SECTION("false body");
            AST_DUMP_AST(ast->if_stmt_ptr->false_body);
        }
        AST_DUMP_END();
        break;

    case AST_FUNCTION_CALL:
        AST_DUMP_START();
        AST_PRINT_PROPERTY("type", "FUNC_CALL");
        AST_PRINT_PROPERTY("call type", (ast->func_call_ptr->type == FUNC_CALL_NORMAL ? "NORMAL" : "ANONY"));
        AST_PRINT_SECTION("function");
        if (ast->func_call_ptr->type == FUNC_CALL_NORMAL)
            AST_DUMP_AST(ast->func_call_ptr->function);
        else {
            // todo: finish later
        }
        AST_PRINT_SECTION("args");
        brama_dump_vector_internal(ast->func_call_ptr->args, level + 1);
        AST_DUMP_END();
        break;

    case AST_ACCESSOR:
        AST_DUMP_START();
        AST_PRINT_PROPERTY("type", "ACCESSOR");
        AST_PRINT_SECTION("object");
        AST_DUMP_AST(ast->accessor_ptr->object);
        AST_PRINT_SECTION("property");
        AST_DUMP_AST(ast->accessor_ptr->property);
        AST_DUMP_END();
        break;

    case AST_SYMBOL:
        AST_DUMP_START();
        AST_PRINT_PROPERTY("type", "SYMBOL");
        AST_PRINT_PROPERTY("data", ast->char_ptr);
        AST_DUMP_END();
        break;

    case AST_PRIMATIVE:
        AST_DUMP_START();
        AST_PRINT_PROPERTY("type", "PRIMATIVE");

        switch (ast->primative_ptr->type)
        {
        case PRIMATIVE_BOOL:
            AST_PRINT_PROPERTY("bool", (ast->primative_ptr->bool_? "TRUE": "FALSE"));
            break;

        case PRIMATIVE_INTEGER:
            AST_PRINT_PROPERTY_DECIMAL("int", ast->primative_ptr->int_);
            break;

        case PRIMATIVE_STRING:
            AST_PRINT_PROPERTY_DECIMAL("string", ast->primative_ptr->char_ptr);
            break;

        case PRIMATIVE_NULL:
            AST_PRINT_PROPERTY_DECIMAL("null", "null");
            break;

        case PRIMATIVE_DOUBLE:
            AST_PRINT_PROPERTY_FLOAT("float", ast->primative_ptr->double_);
            break;
        }

        AST_DUMP_END();
        break;

    default:
        break;
    }
}


void brama_dump_ast(t_context_ptr context) {
    t_context_ptr _context = (t_context_ptr)context;
    int i                    = 0;
    int totalAst             = _context->parser->asts->length;
    vec_t_ast_ptr_t_ptr asts = _context->parser->asts;

    for (i = 0; i < totalAst; ++i) {
        t_ast_ptr ast = asts->data[i];
        brama_dump_ast_internal(ast, 0);
    }
}


bool destroy_ast(t_ast_ptr ast) {
   if (ast == NULL)
       return false;

   if (ast->type == AST_PRIMATIVE) {
       destroy_ast_primative(ast->primative_ptr);
       BRAMA_FREE(ast->primative_ptr);
       ast->primative_ptr = NULL;
   }

   else if (ast->type == AST_ASSIGNMENT) {
       destroy_ast_assignment(ast->assign_ptr);
       BRAMA_FREE(ast->assign_ptr);
       ast->assign_ptr = NULL;
   }

   else if (ast->type == AST_BINARY_OPERATION) {
       destroy_ast_binary(ast->binary_ptr);
       BRAMA_FREE(ast->binary_ptr);
       ast->binary_ptr = NULL;
   }

   else if (ast->type == AST_CONTROL_OPERATION) {
       destroy_ast_control(ast->control_ptr);
       BRAMA_FREE(ast->control_ptr);
       ast->control_ptr = NULL;
   }

   else if (ast->type == AST_WHILE) {
       destroy_ast_while_loop(ast->while_ptr);
       BRAMA_FREE(ast->while_ptr);
       ast->control_ptr = NULL;
   }

   else if (ast->type == AST_BLOCK) {
       destroy_ast_vector(ast->vector_ptr);
       BRAMA_FREE(ast->vector_ptr);
       ast->vector_ptr = NULL;
   }

   else if (ast->type == AST_UNARY) {
       destroy_ast_unary(ast->unary_ptr);
       BRAMA_FREE(ast->unary_ptr);
       ast->unary_ptr = NULL;
   }

   else if (ast->type == AST_RETURN) {
       destroy_ast(ast->ast_ptr);
       BRAMA_FREE(ast->ast_ptr);
       ast->ast_ptr = NULL;
   }

   else if (ast->type == AST_FUNCTION_CALL) {
       destroy_ast_func_call(ast->func_call_ptr);
       BRAMA_FREE(ast->func_call_ptr);
       ast->vector_ptr = NULL;
   }

   else if (ast->type == AST_FUNCTION_DECLARATION) {
       destroy_ast_func_decl(ast->func_decl_ptr);
       BRAMA_FREE(ast->func_decl_ptr);
       ast->func_decl_ptr = NULL;
   }

   else if (ast->type == AST_OBJECT_CREATION) {
       destroy_ast_object_creation(ast->object_creation_ptr);
       BRAMA_FREE(ast->object_creation_ptr);
       ast->object_creation_ptr = NULL;
   }

   else if (ast->type == AST_IF_STATEMENT) {
       destroy_ast_if_stmt(ast->if_stmt_ptr);
       BRAMA_FREE(ast->if_stmt_ptr);
       ast->if_stmt_ptr = NULL;
   }

   else if (ast->type == AST_ACCESSOR) {
       destroy_ast_accessor(ast->accessor_ptr);
       BRAMA_FREE(ast->accessor_ptr);
       ast->accessor_ptr = NULL;
   }

   else if (ast->type == AST_BREAK)
       return true;

   else if (ast->type == AST_CONTINUE)
       return true;

   else if (ast->type == AST_SYMBOL)
       return true;

   else
       return false;

   return true;
}

bool destroy_ast_unary(t_unary_ptr unary_ptr) {
    if (unary_ptr->content != NULL) {
        destroy_ast(unary_ptr->content);
        BRAMA_FREE(unary_ptr->content);
    }

    return true;
}

bool destroy_ast_if_stmt(t_if_stmt_ptr if_stmt_ptr) {
    if (if_stmt_ptr->condition != NULL) {
        destroy_ast(if_stmt_ptr->condition);
        BRAMA_FREE(if_stmt_ptr->condition);
    }

    if (if_stmt_ptr->true_body != NULL) {
        destroy_ast(if_stmt_ptr->true_body);
        BRAMA_FREE(if_stmt_ptr->true_body);
    }

    if (if_stmt_ptr->false_body != NULL) {
        destroy_ast(if_stmt_ptr->false_body);
        BRAMA_FREE(if_stmt_ptr->false_body);
    }

    return true;
}

bool destroy_ast_object_creation(t_object_creation_ptr object_creation_ptr) {
    if (object_creation_ptr->args != NULL) {
        destroy_ast_vector(object_creation_ptr->args);
        BRAMA_FREE(object_creation_ptr->args);
    }

    return true;
}

bool destroy_ast_func_call(t_func_call_ptr func_call_ptr) {
    if (func_call_ptr->args != NULL) {
        destroy_ast_vector(func_call_ptr->args);
        BRAMA_FREE(func_call_ptr->args);
    }

    if (func_call_ptr->type == FUNC_CALL_NORMAL) {
        if (func_call_ptr->function != NULL) {
            destroy_ast(func_call_ptr->function);
            BRAMA_FREE(func_call_ptr->function);
        }
    } else {
        if (func_call_ptr->func_decl_ptr != NULL) {
            destroy_ast_func_decl(func_call_ptr->func_decl_ptr);
            BRAMA_FREE(func_call_ptr->func_decl_ptr);
        }
    }

    return true;
}

bool destroy_ast_func_decl(t_func_decl_ptr func_decl_ptr) {
    if (func_decl_ptr->args != NULL) {
        destroy_ast_vector(func_decl_ptr->args);
        BRAMA_FREE(func_decl_ptr->args);
    }
    if (func_decl_ptr->body != NULL) {
        destroy_ast(func_decl_ptr->body);
        BRAMA_FREE(func_decl_ptr->body);
    }

    return true;
}

bool destroy_ast_while_loop(t_while_loop_ptr while_ptr) {
    if (while_ptr->body != NULL) {
        if (destroy_ast(while_ptr->body)) {
            BRAMA_FREE(while_ptr->body);
        }
    }

    if (while_ptr->condition != NULL) {
        if (destroy_ast(while_ptr->condition)) {
            BRAMA_FREE(while_ptr->condition);
        }
    }
    return true;
}

bool destroy_ast_accessor(t_accessor_ptr accessor_ptr) {
    if (accessor_ptr->object != NULL) {
        if (destroy_ast(accessor_ptr->object)) {
            BRAMA_FREE(accessor_ptr->object);
        }
    }

    if (accessor_ptr->property != NULL) {
        if (destroy_ast(accessor_ptr->property)) {
            BRAMA_FREE(accessor_ptr->property);
        }
    }
}

bool destroy_ast_binary(t_binary_ptr binary) {
    if (binary->left != NULL) {
        if (destroy_ast(binary->left)) {
            BRAMA_FREE(binary->left);
        }
    }

    if (binary->right != NULL) {
        if (destroy_ast(binary->right)) {
            BRAMA_FREE(binary->right);
        }
    }
    return true;
}

bool destroy_ast_control(t_control_ptr control) {
    if (control->left != NULL) {
        if (destroy_ast(control->left)) {
            BRAMA_FREE(control->left);
        }
    }

    if (control->right != NULL) {
        if (destroy_ast(control->right)) {
            BRAMA_FREE(control->right);
        }
    }
    return true;
}

bool destroy_ast_assignment(t_assign_ptr assignment) {
    if (assignment->assignment != NULL) {
        if (destroy_ast(assignment->assignment)) {
            BRAMA_FREE(assignment->assignment);
        }
    }
    if (assignment->object != NULL) {
        if (destroy_ast(assignment->object)) {
            BRAMA_FREE(assignment->object);
        }
    }
    return true;
}

bool destroy_ast_vector(vec_t_ast_ptr_t_ptr vector) {
    size_t i;
    size_t total = vector->length;
    for (i = 0; i < total; ++i) {
        t_ast_ptr item = vector->data[i];
        destroy_ast(item);
        BRAMA_FREE(item);
        item = NULL;
    }
    //destroy_ast_vector(vector);
    return true;
}

bool destroy_token_vector(vec_t_token_ptr_t_ptr vector) {
    size_t i;
    size_t total = vector->length;
    for (i = 0; i < total; ++i) {
        t_token_ptr item = vector->data[i];
        BRAMA_FREE(item);
        item = NULL;
    }
    destroy_ast_vector(vector);
    return true;
}

bool destroy_ast_primative(t_primative_ptr primative) {
    switch (primative->type) {
        case PRIMATIVE_STRING:
        case PRIMATIVE_BOOL:
        case PRIMATIVE_DOUBLE:
        case PRIMATIVE_INTEGER:
        case PRIMATIVE_NONE:
            /* We do not need to free any of those types. String will be freed at token destroy operation */
            break;

        case PRIMATIVE_ARRAY:
            destroy_ast_vector(primative->array);
            BRAMA_FREE(primative->array);
            primative->array = NULL;
            break;

        case PRIMATIVE_DICTIONARY: {
            if (primative->dict != NULL) {
                map_iter_t iter = map_iter(primative->dict);
                const char* key = map_next(primative->dict, &iter);
                while(key != NULL) {
                    t_ast_ptr ast = *map_get(primative->dict, key);
                    destroy_ast(ast);
                    BRAMA_FREE(ast);
                    key = map_next(primative->dict, &iter);
                }
                map_deinit(primative->dict);
                BRAMA_FREE(primative->dict);
                primative->dict = NULL;
            }

            break;
        }

        default:
            break;
    }

    return true;
}


/* Compile Begin */

/* Binary Operation
 * Example : 10 + 20 - 10 * 5.5 */
void compile_binary(t_context_ptr context, t_ast_ptr const ast) {
    compile_internal(context, ast->binary_ptr->left);
    size_t left_index = context->compiler->constants->length - 1;

    compile_internal(context, ast->binary_ptr->right);
    size_t right_index = context->compiler->constants->length - 1;

    t_brama_byte opcode;
    t_brama_vmdata code;
    code.op   = 0;
    code.reg1 = 0;
    code.reg2 = 0;
    code.reg3 = 0;

    code.reg1 = left_index;
    code.reg2 = right_index;

    switch (ast->binary_ptr->opt) {
        case OPERATOR_ADDITION:
            code.op = VM_OPT_ADDITION;
            break;

        case OPERATOR_SUBTRACTION:
            code.op = VM_OPT_SUBTRACTION;
            break;

        case OPERATOR_BITWISE_AND:
            code.op = VM_OPT_SUBTRACTION;
            break;

        case OPERATOR_BITWISE_OR:
            code.op = VM_OPT_BITWISE_OR;
            break;

        case OPERATOR_BITWISE_XOR:
            code.op = VM_OPT_BITWISE_XOR;
            break;

        case OPERATOR_DIVISION:
            code.op = VM_OPT_DIVISION;
            break;

        case OPERATOR_MULTIPLICATION:
            code.op = VM_OPT_MULTIPLICATION;
            break;

        case OPERATOR_BITWISE_LEFT_SHIFT:
            code.op = VM_OPT_BITWISE_LEFT_SHIFT;
            break;

        case OPERATOR_BITWISE_RIGHT_SHIFT:
            code.op = VM_OPT_BITWISE_RIGHT_SHIFT;
            break;

        case OPERATOR_BITWISE_UNSIGNED_RIGHT_SHIFT:
            code.op = VM_OPT_BITWISE_UNSIGNED_RIGHT_SHIFT;
            break;
    }

    vec_push(context->compiler->op_codes, vm_encode(&code));
}

void compile_primative(t_context_ptr context, t_ast_ptr const ast) {
    switch (ast->primative_ptr->type) {
        case PRIMATIVE_INTEGER: {
            vec_push(context->compiler->constants, numberToValue(ast->primative_ptr->int_));
        }
            break;

        case PRIMATIVE_DOUBLE: {
            t_vm_const_item_ptr item = BRAMA_MALLOC(sizeof(t_vm_const_item));
            item->double_            = ast->primative_ptr->double_;
            item->type               = CONST_DOUBLE;
            vec_push(context->compiler->constants, item);
        }
            break;

        case PRIMATIVE_BOOL: {
            t_vm_const_item_ptr item = BRAMA_MALLOC(sizeof(t_vm_const_item));
            item->bool_              = ast->primative_ptr->bool_;
            item->type               = CONST_BOOL;
            vec_push(context->compiler->constants, item);
        }
            break;
    }
}


void compile_internal(t_context_ptr context, t_ast_ptr const ast) {
    switch (ast->type) {
        case AST_PRIMATIVE:
            compile_primative(context, ast);
            break;

        case AST_BINARY_OPERATION:
            compile_binary(context, ast);
            break;
    }
}

void compile(t_context_ptr context) {
    vec_t_ast_ptr_t_ptr asts = context->parser->asts;
    size_t total_ast  = asts->length;
    for (size_t i = 0; i < total_ast; ++i) {
        t_ast_ptr ast = asts->data[i];
        compile_internal(context, ast);
    }
}

bool isBool(t_brama_data value) {
    return value == TRUE_VAL || value == FALSE_VAL;
}

t_brama_data numberToValue(double num) {
    DoubleBits data;
    data.num = num;
    return data.bits64;
}

double valueToNumber(t_brama_data num) {
    DoubleBits data;
    data.bits64 = num;
    return data.num;
}

void run(t_context_ptr context) {
    vec_t_byte_ptr bytes         = context->compiler->op_codes;
    vec_const_item_ptr constants = context->compiler->constants;

    size_t total_bytes  = bytes->length;
    for (size_t i = 0; i < total_bytes; ++i) {
        t_brama_vmdata vmdata;
        vm_decode(bytes->data[i], &vmdata);

        switch (vmdata.op) {
            case VM_OPT_ADDITION: {
                t_brama_data left  = constants->data[vmdata.reg1];
                t_brama_data right = constants->data[vmdata.reg2];

                if (IS_NUM(left) && IS_NUM(right)) {
                    printf("%f\r\n", valueToNumber(left) + valueToNumber(right));
                }

                //printf("%d\r\n", constants->data[vmdata.reg2]->int_ + constants->data[vmdata.reg3]->int_);
            }
        }
    }
}

/* Compile End */

/* VM Begin */

/* FOR FUTURE USAGE */
void vm_decode(t_brama_byte instr, t_brama_vmdata_ptr t) {
    t->op   = (instr & 0xF000) >> 12;
    t->reg1 = (instr & 0x0F00) >> 8;
    t->reg2 = (instr & 0x00F0) >> 4;
    t->reg3 = (instr & 0x000F);
    t->scal = (instr & 0x00FF);
}

t_brama_byte vm_encode(t_brama_vmdata_ptr t) {
    t_brama_byte instr = 0;
    instr += t->op   << 12;
    instr += t->reg1 << 8;
    if (t->scal != 0)
        instr += t->scal;
    else {
        instr += t->reg2 << 4;
        instr += t->reg3;
    }
    return instr;
}

/* VM End */

void brama_destroy(t_context_ptr context) {
    t_context_ptr _context = (t_context_ptr)context;
    size_t i;
    size_t totalToken = _context->tokinizer->tokens->length;
    for (i = 0; i < totalToken; i++) {
        t_token_ptr token = _context->tokinizer->tokens->data[i];
        if (token->type == TOKEN_TEXT ||
            token->type == TOKEN_SYMBOL)
            BRAMA_FREE((char_ptr)token->char_ptr);
        BRAMA_FREE(token);
    }

    size_t totalAst = _context->parser->asts->length;
    for (i = 0; i < totalAst; ++i) {
        t_ast_ptr ast = _context->parser->asts->data[i];
        destroy_ast(ast);
        BRAMA_FREE(ast);
    }

    if (context->error_message != NULL)
        BRAMA_FREE(context->error_message);

    vec_deinit(_context->tokinizer->tokens);
    vec_deinit(_context->parser->asts);
    vec_deinit(_context->compiler->op_codes);

    map_deinit(&_context->tokinizer->keywords);
    BRAMA_FREE(_context->tokinizer->tokens);
    BRAMA_FREE(_context->tokinizer);

    BRAMA_FREE(_context->parser->asts);
    BRAMA_FREE(_context->parser);

    BRAMA_FREE(_context->compiler);
    BRAMA_FREE(_context);
}
