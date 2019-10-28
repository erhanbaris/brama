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
        token->keyword    = *keywordInfo;

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
    double beforeTheComma = 0;
    double afterTheComma  = 0;
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
        token->double_ = beforeTheComma;
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
        token->opt     = OPERATOR_SUBTRACTION;

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
    token->opt        = OPERATOR_NONE;

    switch (ch) {
        
        /* 
         * This is special case that is why coded like that.
         * '>>>' is requre all tree token could be same but others just looking first two char 
         */
        case '>' : {
            if (chNext == '>') {
                if (chThird == '>') {
                    token->opt = OPERATOR_BITWISE_UNSIGNED_RIGHT_SHIFT;
                    increase(tokinizer);
                }
                else 
                    token->opt = OPERATOR_BITWISE_RIGHT_SHIFT;
                increase(tokinizer);

            } else if (chNext == '=' ) {
                token->opt = OPERATOR_GREATER_EQUAL_THAN;
                increase(tokinizer);
            } else token->opt = OPERATOR_GREATER_THAN;
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

        OPERATOR_CASE_DOUBLE('%', '=', OPERATOR_MODULO,         OPERATOR_ASSIGN_MODULUS);
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

    if (token->opt == OPERATOR_NONE)
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
            token->opt     = OPERATOR_NEW_LINE;

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

GET_ITEM(keyword,  keyword,  brama_keyword_type)
GET_ITEM(integer,  double_,  int)
GET_ITEM(double,   double_,  double)
GET_ITEM(text,     char_ptr, char_ptr)
GET_ITEM(symbol,   char_ptr, char_ptr)
GET_ITEM(operator, opt,      brama_operator_type)

NEW_PRIMATIVE_DEF(int,    double,              PRIMATIVE_INTEGER,    double_)
NEW_PRIMATIVE_DEF(double, double,              PRIMATIVE_DOUBLE,     double_)
NEW_PRIMATIVE_DEF(text,   char_ptr,            PRIMATIVE_STRING,     char_ptr)
NEW_PRIMATIVE_DEF(bool,   bool,                PRIMATIVE_BOOL,       bool_)
NEW_PRIMATIVE_DEF(empty,  int,                 PRIMATIVE_NULL,       double_)
NEW_PRIMATIVE_DEF(array,  vec_ast_ptr,         PRIMATIVE_ARRAY,      array)
NEW_PRIMATIVE_DEF(dict,   map_ast_t_ptr,       PRIMATIVE_DICTIONARY, dict)

NEW_AST_DEF(symbol,    char_ptr,              AST_SYMBOL,               char_ptr, false)
NEW_AST_DEF(unary,     t_unary_ptr,           AST_UNARY,                unary_ptr, false)
NEW_AST_DEF(binary,    t_binary_ptr,          AST_BINARY_OPERATION,     binary_ptr, false)
NEW_AST_DEF(control,   t_control_ptr,         AST_CONTROL_OPERATION,    control_ptr, false)
NEW_AST_DEF(assign,    t_assign_ptr,          AST_ASSIGNMENT,           assign_ptr, false)
NEW_AST_DEF(func_call, t_func_call_ptr,       AST_FUNCTION_CALL,        func_call_ptr, false)
NEW_AST_DEF(func_decl, t_func_decl_ptr,       AST_FUNCTION_DECLARATION, func_decl_ptr, true)
NEW_AST_DEF(block,     vec_ast_ptr ,          AST_BLOCK,                vector_ptr, false)
NEW_AST_DEF(object,    t_object_creation_ptr, AST_OBJECT_CREATION,      object_creation_ptr, false)
NEW_AST_DEF(while,     t_while_loop_ptr ,     AST_WHILE,                while_ptr, false)
NEW_AST_DEF(if,        t_if_stmt_ptr,         AST_IF_STATEMENT,         if_stmt_ptr, false)
NEW_AST_DEF(return,    t_ast_ptr,             AST_RETURN,               ast_ptr, false)
NEW_AST_DEF(accessor,  t_accessor_ptr,        AST_ACCESSOR,             accessor_ptr, false)
NEW_AST_DEF(keyword,   brama_keyword_type,    AST_KEYWORD,              keyword, false)

brama_status as_primative(t_token_ptr token, t_ast_ptr_ptr ast) {
    switch (token->type) {
    case TOKEN_INTEGER:
        *ast = new_primative_ast_int(get_double(token));
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
        else
            return BRAMA_PARSE_ERROR;
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
        vec_ast_ptr args = BRAMA_MALLOC(sizeof (vec_ast));
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

    status = ast_function_decleration(context, ast, FUNC_DEF_ASSIGNMENT);
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
        status = ast_symbol_expr(context, ast, extra_data);

    /* We are parsing function parameters */
    if (ast_match_operator(context, 1, OPERATOR_LEFT_PARENTHESES)) {
        vec_ast_ptr args = BRAMA_MALLOC(sizeof (vec_ast));
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
        vec_ast_ptr blocks = BRAMA_MALLOC(sizeof (vec_ast));
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

        vec_ast_ptr args = BRAMA_MALLOC(sizeof(vec_ast));
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
            vec_ast_ptr call_args = BRAMA_MALLOC(sizeof(vec_ast));
            vec_init(call_args);

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
    brama_status left_status = ast_modulo_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 2, OPERATOR_ADDITION, OPERATOR_SUBTRACTION)) {
        brama_operator_type opt = get_operator(ast_previous(context));
        t_ast_ptr right         = NULL;
        brama_status right_status = ast_modulo_expr(context, &right, extra_data);
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

brama_status ast_modulo_expr(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data) {
    brama_status left_status = ast_multiplication_expr(context, ast, extra_data);
    if (left_status != BRAMA_OK)
        return left_status;

    while (ast_match_operator(context, 1, OPERATOR_MODULO)) {
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

    t_ast_ptr right         = NULL;
    brama_operator_type opt = OPERATOR_NONE;

    if (ast_match_operator(context, 6, OPERATOR_ASSIGN, OPERATOR_ASSIGN_ADDITION, OPERATOR_ASSIGN_DIVISION, OPERATOR_ASSIGN_MODULUS, OPERATOR_ASSIGN_MULTIPLICATION, OPERATOR_ASSIGN_SUBTRACTION)) {
        opt = get_operator(ast_previous(context));
        brama_status right_status = ast_assignable(context, &right, extra_data);
        if (right_status != BRAMA_OK) {
            CLEAR_AST(right);
            CLEAR_AST(*ast);
            return right_status;
        }
    } else if (!new_def)
        DESTROY_AST_AND_RETURN(BRAMA_DOES_NOT_MATCH_AST, *ast);

    t_assign_ptr assign = BRAMA_MALLOC(sizeof(t_assign));
    assign->object      = *ast;
    assign->def_type    = type;
    assign->opt         = opt;
    assign->assignment  = right;
    assign->new_def     = new_def;
    *ast = new_assign_ast(assign);

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

        vec_ast_ptr args = BRAMA_MALLOC(sizeof (vec_ast));
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

    status = ast_function_decleration(context, ast, FUNC_DEF_ASSIGNMENT);
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
    return token != NULL && token->type == TOKEN_OPERATOR && token->opt == operator_type;
}

bool ast_check_keyword(t_context_ptr context, brama_keyword_type keyword_type) {
    t_token_ptr token = ast_peek(context);
    return token != NULL && token->type == TOKEN_KEYWORD && token->keyword == keyword_type;
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
    context->tokinizer->tokens = BRAMA_MALLOC(sizeof (vec_token));
    vec_init(context->tokinizer->tokens);

    /* parser */
    context->parser            = (t_parser_ptr)BRAMA_MALLOC(sizeof (t_parser));
    context->parser->index     = 0;
    context->parser->line      = 0;
    context->parser->asts      = BRAMA_MALLOC(sizeof (vec_ast));
    vec_init(context->parser->asts);

    /* keywords */
    map_init(&context->tokinizer->keywords);

    size_t keywordCount = sizeof (KEYWORDS_PAIR) / sizeof(KeywordPair);
    for (size_t i = 0; i < keywordCount; ++i)
        map_set(&context->tokinizer->keywords, KEYWORDS_PAIR[i].name,  KEYWORDS_PAIR[i].keyword);

    /* Compiler */
    context->compiler                 = (t_compiler_ptr)BRAMA_MALLOC(sizeof(t_compiler));
    context->compiler->index          = 0;
    context->compiler->head           = NULL;
    context->compiler->total_object   = 0;
    context->compiler->op_codes       = BRAMA_MALLOC(sizeof (vec_opcode));
    context->compiler->global_storage = BRAMA_MALLOC(sizeof (t_storage));
    context->compiler->global_storage->id           = 0;
    context->compiler->global_storage->loop_counter = 0;
    vec_init(&context->compiler->global_storage->constants);
    vec_init(&context->compiler->global_storage->variables);
    map_init(&context->compiler->global_storage->variable_names);

    vec_init(&context->compiler->storages);
    vec_init(context->compiler->op_codes);

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

void brama_compile(t_context_ptr context, char_ptr data) {
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

    context->status = BRAMA_OK;
}

void brama_run(t_context_ptr context) {
    if (context->status != BRAMA_OK)
        return;

    compile(context);
    run(context);
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
            printf("OPERATOR = '%s'\r\n", OPERATORS[(int)token->opt].opt);
        else if (token->type == TOKEN_SYMBOL)
            printf("SYMBOL   = '%s'\r\n", token->char_ptr);
        else if (token->type == TOKEN_KEYWORD)
            printf("KEYWORD  = '%s'\r\n", KEYWORDS[(int)token->keyword]);
        else if (token->type == TOKEN_INTEGER)
            printf("INTEGER  = '%d'\r\n", token->double_);
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

void brama_dump_vector_internal(vec_ast_ptr vector, size_t level) {
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

    case AST_BINARY_OPERATION:
        AST_DUMP_START();
        AST_PRINT_PROPERTY("type", "BINARY");
        AST_PRINT_PROPERTY("opt", OPERATORS[(int)ast->binary_ptr->opt]);
        AST_PRINT_SECTION("left");
        AST_DUMP_AST(ast->binary_ptr->left);
        AST_PRINT_SECTION("right");
        AST_DUMP_AST(ast->binary_ptr->right);
        AST_DUMP_END();
        break;

    case AST_ASSIGNMENT:
        AST_DUMP_START();
        AST_PRINT_PROPERTY("type", "ASSIGN");
        AST_PRINT_PROPERTY("opt", OPERATORS[(int)ast->assign_ptr->opt]);
        AST_PRINT_SECTION("object");
        AST_DUMP_AST(ast->assign_ptr->object);
        AST_PRINT_SECTION("assignment");
        AST_DUMP_AST(ast->assign_ptr->assignment);
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
    vec_ast_ptr asts = _context->parser->asts;

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

   else if (ast->type == AST_KEYWORD)
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

bool destroy_ast_vector(vec_ast_ptr vector) {
    size_t i;
    size_t total = vector->length;
    for (i = 0; i < total; ++i) {
        t_ast_ptr item = vector->data[i];
        destroy_ast(item);
        BRAMA_FREE(item);
        item = NULL;
    }
    vec_deinit(vector);
    //destroy_ast_vector(vector);
    return true;
}

bool destroy_token_vector(vec_token_ptr vector) {
    size_t i;
    size_t total = vector->length;
    for (i = 0; i < total; ++i) {
        t_token_ptr item = vector->data[i];
        BRAMA_FREE(item);
        item = NULL;
    }
    vec_deinit(vector);
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
void compile_binary(t_context_ptr context, t_binary_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    size_t dest_id = 0;
    if (upper_ast == AST_ASSIGNMENT)
        dest_id = compile_info->index;
    else {
        vec_push(&storage->variables,  NULL);
        dest_id = storage->variables.length;
    }

    compile_internal(context, ast->left, storage, compile_info, AST_NONE);
    int left_index = compile_info->index;

    compile_internal(context, ast->right, storage, compile_info, AST_NONE);
    int right_index = compile_info->index;

    t_brama_vmdata code;
    code.op   = 0;
    code.reg1 = dest_id;
    code.reg2 = 0;
    code.reg3 = 0;
    code.scal = 0;

    code.reg2 = left_index;
    code.reg3 = right_index;

    switch (ast->opt) {
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

    compile_info->index       = dest_id;
    vec_push(context->compiler->op_codes, vm_encode(&code));
}

void compile_control(t_context_ptr context, t_control_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    size_t dest_id = 0;
    if (upper_ast == AST_ASSIGNMENT)
        dest_id = compile_info->index;
    else {
        vec_push(&storage->variables,  NULL);
        dest_id = storage->variables.length;
    }

    compile_internal(context, ast->left, storage, compile_info, AST_NONE);
    int left_index = compile_info->index;

    compile_internal(context, ast->right, storage, compile_info, AST_NONE);
    int right_index = compile_info->index;

    t_brama_vmdata code;
    code.op   = 0;
    code.reg1 = dest_id;
    code.reg2 = 0;
    code.reg3 = 0;
    code.scal = 0;

    code.reg2 = left_index;
    code.reg3 = right_index;

    switch (ast->opt) {
        case OPERATOR_LESS_THAN:
            code.op = VM_OPT_LT;
            break;

        case OPERATOR_LESS_EQUAL_THAN:
            code.op = VM_OPT_LTE;
            break;

        case OPERATOR_GREATER_THAN:
            code.op = VM_OPT_GT;
            break;

        case OPERATOR_GREATER_EQUAL_THAN:
            code.op = VM_OPT_GTE;
            break;

        case OPERATOR_EQUAL:
            code.op = VM_OPT_GT;
            break;

        case OPERATOR_EQUAL_VALUE:
            code.op = VM_OPT_GTE;
            break;
    }

    compile_info->index       = dest_id;
    vec_push(context->compiler->op_codes, vm_encode(&code));
}



void compile_symbol(t_context_ptr context, char_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    /* We need to check variable that used on scope before */
    size_t* index = map_get(&storage->variable_names, ast);

    if (index == NULL) {
        /* We did not found variable */
        compile_info->index = 0;
    } else
        /* Return back variable index */
        compile_info->index = (*index) + 1;
}

void compile_assignment(t_context_ptr context, t_assign_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    /* We need to check variable that used on scope before */
    size_t* index = map_get(&storage->variable_names, ast->object->char_ptr);

    if (index == NULL) {
        /* Variable not defined before, create new variable slot */
        vec_push(&storage->variables, UNDEFINED_VAL);

        /* Variable name linked to variable index */
        map_set(&storage->variable_names, ast->object->char_ptr, storage->variables.length - 1);

        /* Sub operation need to know which slot it need to use */
        compile_info->index = storage->variables.length;
    } else
        /* Assign previous variable index */
        compile_info->index = (*index) + 1;

    //vec_push(&storage->variable_names, assign->object->char_ptr);

    /* Has assignment code */
    if (ast->assignment != NULL) {
        size_t variable_index = compile_info->index;

        /* Opcode generation for assignment */
        compile_internal(context, ast->assignment, storage, compile_info, AST_ASSIGNMENT);

        /* If it is primative, set value to variable */
        if (ast->assignment->type == AST_PRIMATIVE) {
            t_brama_vmdata code;
            code.op = VM_OPT_INIT_VAR;
            code.reg1 = variable_index;
            code.reg2 = storage->constants.length * -1;
            code.reg3 = 0;
            code.scal = 0;
            vec_push(context->compiler->op_codes, vm_encode(&code));
        }
        else if (ast->assignment->type == AST_SYMBOL) {
            t_brama_vmdata code;
            code.op = VM_OPT_COPY;
            code.reg1 = variable_index;
            code.reg2 = compile_info->index;
            code.reg3 = 0;
            code.scal = 0;
            vec_push(context->compiler->op_codes, vm_encode(&code));
        }
    }
}

void compile_if(t_context_ptr context, t_if_stmt_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    compile_internal(context, ast->condition, storage, compile_info, AST_IF_STATEMENT);
    size_t condition = compile_info->index;


    /* Add IF opcode */
    t_brama_vmdata code;
    code.op = VM_OPT_IF_EQ;
    code.reg1 = condition;
    code.reg2 = 0;
    code.reg3 = 0;
    code.scal = 0;
    vec_push(context->compiler->op_codes, vm_encode(&code));

    size_t jmp_true_location  = 0;
    size_t jmp_false_location = 0;

    if (ast->false_body != NULL) {

        /*                           *
         * Else block declared so we *
         * need to define flow       *
         * control like below        *
         * ------------------------- *
         * 1. IF CONDITION           *
         * ------------------------- *
         * 2. JMP (FALSE ) ->  TO 5. *
         * ------------------------- *
         * 3. TRUE BLOCK             *
         * ------------------------- *
         * 4. JMP (TRUE) ->  TO 6.   *
         * ------------------------- *
         * 5. FALSE BLOCK            *
         * ------------------------- *
         * 6. NORMAL OPCODE          *
         * ------------------------- */

        vec_push(context->compiler->op_codes, NULL); // Put 'false' jmp code
        jmp_false_location = context->compiler->op_codes->length - 1;

        /* Build 'true' block */
        compile_internal(context, ast->true_body, storage, compile_info, AST_IF_STATEMENT);
        vec_push(context->compiler->op_codes, NULL); // Put 'true' jmp code

        jmp_true_location = context->compiler->op_codes->length - 1;

        /* Configure 'false' block jmp opcode */
        code.op = VM_OPT_JMP;
        code.reg1 = 0;
        code.reg2 = 0;
        code.reg3 = 0;
        code.scal = context->compiler->op_codes->length - jmp_false_location - 1;
        context->compiler->op_codes->data[jmp_false_location] = vm_encode(&code);

        /* Build 'false' block */
        compile_internal(context, ast->false_body, storage, compile_info, AST_IF_STATEMENT);

        /* Configure 'true' block jmp opcode */
        code.op = VM_OPT_JMP;
        code.reg1 = 0;
        code.reg2 = 0;
        code.reg3 = 0;
        code.scal = context->compiler->op_codes->length - jmp_true_location - 1;
        context->compiler->op_codes->data[jmp_true_location] = vm_encode(&code);

    } else {
         /*                           *
          * We do not have else block *
          * ------------------------- *
          * 1. IF CONDITION           *
          * ------------------------- *
          * 2. JMP (FALSE ) ->  TO 4. *
          * ------------------------- *
          * 3. TRUE BLOCK             *
          * ------------------------- *
          * 4. NORMAL OPCODE          *
          * ------------------------- */

        vec_push(context->compiler->op_codes, NULL); // Put 'false' jmp code
        jmp_false_location = context->compiler->op_codes->length - 1;

        /* Build 'true' block */
        compile_internal(context, ast->true_body, storage, compile_info, AST_IF_STATEMENT);

        /* Configure 'false' block jmp opcode */
        code.op = VM_OPT_JMP;
        code.reg1 = 0;
        code.reg2 = 0;
        code.reg3 = 0;
        code.scal = context->compiler->op_codes->length - jmp_false_location - 1;

        /* Part of the while condition */
        if (upper_ast == AST_WHILE) {
            code.scal += 1;
        }

        context->compiler->op_codes->data[jmp_false_location] = vm_encode(&code);
    }
}

void compile_block(t_context_ptr context, vec_ast_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    int index;
    t_ast_ptr ast_item = NULL;

    vec_foreach(ast, ast_item, index) {
        compile_internal(context, ast_item, storage, compile_info, upper_ast);
    }
}

void compile_unary(t_context_ptr context, t_unary_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    int dest_id = compile_info->index;
    compile_internal(context, ast->content, storage, compile_info, AST_UNARY);

    t_brama_vmdata code;
    code.reg1 = compile_info->index;
    code.reg2 = 0;
    code.reg3 = 0;
    code.scal = 0;

    switch (ast->opt) {
        case OPERATOR_INCREMENT:
            code.op = VM_OPT_INC;
            break;

        case OPERATOR_DECCREMENT:
            code.op = VM_OPT_DINC;
            break;

        case OPERATOR_SUBTRACTION: {
            /* Add mul operator */
            code.op = VM_OPT_MULTIPLICATION;
            code.reg1 = dest_id;
            code.reg2 = compile_info->index;

            int index;
            vec_find(&storage->constants, numberToValue(-1), index);
            if (index == -1) {
                vec_push(&storage->constants, numberToValue(-1));
                code.reg3 = (storage->constants.length) * -1;
            }
            else
                code.reg3 = ++index * -1;
            break;
        }

        case OPERATOR_NOT:
            break;

        case OPERATOR_BITWISE_NOT:
            break;
    }

    vec_push(context->compiler->op_codes, vm_encode(&code));
}

void compile_while(t_context_ptr context, t_while_loop_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    /*                           *
     * Else block declared so we *
     * need to define flow       *
     * control like below        *
     * ------------------------- *
     * 1. CONTROL STATEMENT      *
     * ------------------------- *
     * N+1. IF CONDITION (1. VAR)*
     * ------------------------- *
     * N+2. JMP (FALSE)->TO N+5. *
     * ------------------------- *
     * N+3. WHILE BLOCK          *
     * ------------------------- *
     * N+4. JMP -> TO 1.         *
     * ------------------------- *
     * N+5. NORMAL OPCODE        *
     * ------------------------- */

    /* Define new variable to store condition information */
    t_assign_ptr assign      = BRAMA_MALLOC(sizeof(t_assign));
    assign->opt              = OPERATOR_ASSIGN;
    assign->def_type         = KEYWORD_VAR;
    assign->new_def          = true;
    assign->object           = BRAMA_MALLOC(sizeof(t_ast));
    assign->object->type     = AST_SYMBOL;
    assign->object->char_ptr = BRAMA_MALLOC(sizeof(char) * 20);
    assign->object->create_new_storage = false;

    /* Store loop status */
    sprintf(assign->object->char_ptr, "(loop #%d)", ++storage->loop_counter);
    assign->object->char_ptr[19] = '\0';
    assign->assignment = ast->condition;

    compile_assignment(context, assign, storage, compile_info, AST_WHILE);
    size_t condition = compile_info->index;

    /* Build if statement */
    t_if_stmt_ptr if_stmt = BRAMA_MALLOC(sizeof(t_if_stmt));
    if_stmt->condition    = assign->object;
    if_stmt->true_body    = ast->body;
    if_stmt->false_body   = NULL;

    size_t jmp_compare_location = context->compiler->op_codes->length - 1;
    compile_if(context, if_stmt, storage, compile_info, AST_WHILE);

    t_brama_vmdata code;
    code.op = VM_OPT_JMP;
    code.reg1 = 0;
    code.reg2 = 0;
    code.reg3 = 0;
    code.scal = jmp_compare_location - context->compiler->op_codes->length - 1;

    vec_push(context->compiler->op_codes, vm_encode(&code));

    /* Remove all references */
    BRAMA_FREE(assign->object->char_ptr);
    assign->assignment = NULL;
    if_stmt->condition = NULL;
    if_stmt->true_body = NULL;
    destroy_ast_assignment(assign);
    BRAMA_FREE(assign);

    destroy_ast_if_stmt(if_stmt);
    BRAMA_FREE(if_stmt);
}

void compile_primative(t_context_ptr context, t_primative_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {
    int index = -1;
    compile_info->status = BRAMA_OK;

    switch (ast->type) {
        case PRIMATIVE_INTEGER:
        case PRIMATIVE_DOUBLE:
            vec_find(&storage->constants, numberToValue(ast->double_), index);
            if (index == -1) {
                vec_push(&storage->constants, numberToValue(ast->double_));
                compile_info->index = (storage->constants.length) * -1;
            }
            else
                compile_info->index = ++index * -1;

            break;

        case PRIMATIVE_BOOL:
            vec_find(&storage->constants, ast->bool_ ? (TRUE_VAL) : (FALSE_VAL), index);
            if (index == -1) {
                vec_push(&storage->constants, ast->bool_ ? (TRUE_VAL) : (FALSE_VAL));
                compile_info->index = (storage->constants.length) * -1;
            }
            else
                compile_info->index = ++index * -1;
            break;

        case PRIMATIVE_STRING: {
            t_brama_value value;
            compile_info->index = 0;
            vec_foreach(&storage->constants, value, index) {
                if (IS_STRING(value) && strcmp(ast->char_ptr, AS_OBJ(value)->char_ptr) == 0)
                    compile_info->index = (index + 1) * -1;
            }

            if (compile_info->index == 0) {
                t_vm_object_ptr object = new_vm_object(context);
                object->type           = CONST_STRING;
                object->char_ptr       = strdup(ast->char_ptr);
                vec_push(&storage->constants, GET_VALUE_FROM_OBJ(object));
                compile_info->index = (storage->constants.length) * -1;
            }
        }
            break;

        default:
            compile_info->status = BRAMA_NOK;
            break;
    }
}


void compile_internal(t_context_ptr context, t_ast_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast) {

    if (ast->create_new_storage) {
        t_storage_ptr new_storage = BRAMA_MALLOC(sizeof(t_storage));
        vec_push(&context->compiler->storages, new_storage);

        new_storage->id               = context->compiler->storages.length;
        new_storage->previous_storage = storage;
        new_storage->loop_counter     = 0;
        vec_init(&new_storage->constants);
        vec_init(&new_storage->variables);
        map_init(&new_storage->variable_names);

        storage = new_storage;
    }

    switch (ast->type) {
        case AST_PRIMATIVE:
            compile_primative(context, ast->primative_ptr, storage, compile_info, upper_ast);
            break;

        case AST_BINARY_OPERATION:
            compile_binary(context, ast->binary_ptr, storage, compile_info, upper_ast);
            break;

        case AST_ASSIGNMENT:
            compile_assignment(context, ast->assign_ptr, storage, compile_info, upper_ast);
            break;

        case AST_CONTROL_OPERATION:
            compile_control(context, ast->control_ptr, storage, compile_info, upper_ast);
            break;

        case AST_SYMBOL:
            compile_symbol(context, ast->char_ptr, storage, compile_info, upper_ast);
            break;

        case AST_IF_STATEMENT:
            compile_if(context, ast->if_stmt_ptr, storage, compile_info, upper_ast);
            break;

        case AST_WHILE:
            compile_while(context, ast->while_ptr, storage, compile_info, upper_ast);
            break;

        case AST_BLOCK:
            compile_block(context, ast->vector_ptr, storage, compile_info, upper_ast);
            break;

        case AST_UNARY:
            compile_unary(context, ast->vector_ptr, storage, compile_info, upper_ast);
            break;
    }
}

void compile(t_context_ptr context) {
    t_compile_info_ptr compile_info = BRAMA_MALLOC(sizeof(t_compile_info));

    vec_ast_ptr asts             = context->parser->asts;
    size_t total_ast             = asts->length;
    for (size_t i = 0; i < total_ast; ++i) {
        t_ast_ptr ast = asts->data[i];
        compile_internal(context, ast, context->compiler->global_storage, compile_info, AST_NONE);
    }

    BRAMA_FREE(compile_info);

    vec_push(context->compiler->op_codes, NULL);
}

bool isBool(t_brama_value value) {
    return value == TRUE_VAL || value == FALSE_VAL;
}

t_brama_value numberToValue(double num) {
    DoubleBits data;
    data.num = num;
    return data.bits64;
}

double valueToNumber(t_brama_value num) {
    DoubleBits data;
    data.bits64 = num;
    return data.num;
}

int8_t sabs8(int8_t i)
{
    int8_t res;

    if (INT8_MIN == i)
    {
        res = INT8_MAX;
    }
    else
    {
        res = i < 0 ? -i : i;
    }

    return res;
}

brama_status brama_get_var(t_context_ptr context, char_ptr var_name, t_get_var_info** var_info) {
    size_t* index = map_get(&context->compiler->global_storage->variable_names, var_name);
    if (index == NULL)
        return BRAMA_NOK;

    *var_info          = BRAMA_MALLOC(sizeof(t_get_var_info));
    (*var_info)->bool_ = false;

    t_brama_value value = context->compiler->global_storage->variables.data[*index];
    if (IS_BOOL(value)) {
        (*var_info)->type  = CONST_BOOL;
        (*var_info)->bool_ = IS_TRUE(value) ? true : false;

    } else if (IS_NUM(value)) {
        double num = valueToNumber(value);
        double intpart;
        double fractpart = modf(num, &intpart);
 
        if (fractpart == 0.0) {
            (*var_info)->type  = CONST_INTEGER;
            (*var_info)->double_  = num;
        } else {
            (*var_info)->type     = CONST_DOUBLE;
            (*var_info)->double_  = num;
        }

    } else if (IS_UNDEFINED(value)) {
        (*var_info)->type  = CONST_UNDEFINED;

    } else if (IS_NULL(value)) {
        (*var_info)->type  = CONST_NULL;

    } else if (IS_STRING(value)) {
        (*var_info)->type  = CONST_STRING;

        (*var_info)->char_ptr = BRAMA_MALLOC(sizeof(char) * (strlen(AS_STRING(value)) + 1));
        strcpy((*var_info)->char_ptr, AS_STRING(value));
        (*var_info)->char_ptr[strlen(AS_STRING(value))] = '\0';
    }

    return  BRAMA_OK;
}

brama_status brama_destroy_get_var(t_context_ptr context, t_get_var_info** var_info) {
    if (var_info == NULL || (*var_info) == NULL)
        return BRAMA_OK;

    if ((*var_info)->type  == CONST_STRING) {
        BRAMA_FREE((*var_info)->char_ptr);
    }

    BRAMA_FREE(*var_info);
    *var_info = NULL;
    return BRAMA_OK;
}

void brama_compile_dump(t_context_ptr context) {
    int            index          = 0;
    vec_byte_ptr   bytes          = context->compiler->op_codes;
    vec_value_ptr  constants      = &context->compiler->global_storage->constants;
    vec_value_ptr  variables      = &context->compiler->global_storage->variables;
    map_size_t_ptr variable_names = &context->compiler->global_storage->variable_names;
    char_ptr       tmp_var        = NULL;
    t_brama_value  tmp_val;

    printf ("[CONSTANTS] [%d]\r\n", constants->length);
    vec_foreach(constants, tmp_val, index) {
        if (IS_BOOL(tmp_val))
            printf ("    [-%d]    %s\r\n", index + 1, IS_FALSE(tmp_val) ? "false" : "true");
        else if (IS_NUM(tmp_val))
            printf ("    [-%d]    %f\r\n", index + 1, valueToNumber(tmp_val));
        else if (IS_STRING(tmp_val))
            printf ("    [-%d]    '%s'\r\n", index + 1, AS_STRING(tmp_val));
        else if (IS_UNDEFINED(tmp_val))
            printf ("    [-%d]    undefined\r\n", index + 1);
        else if (IS_NULL(tmp_val))
            printf ("    [-%d]    null\r\n", index + 1);
    }

    printf ("\r\n[VARIABLES] [%d]\r\n", variables->length);
    char_ptr* variable_infos = BRAMA_CALLOC(variables->length, sizeof(char_ptr));

    map_iter_t iter = map_iter(variable_names);
    index           = 0;
    while ((tmp_var = map_next(variable_names, &iter)))
        variable_infos[(*map_get(variable_names, tmp_var))] = tmp_var;

    index           = 0;
    vec_foreach(variables, tmp_val, index) {
        if (IS_BOOL(tmp_val)) {
            printf ("    [%d]  %-15s %s\r\n", index + 1, variable_infos[index], IS_FALSE(tmp_val) ? "false" : "true");
        } else if (IS_NUM(tmp_val)) {
            printf ("    [%d]  %-15s %f\r\n", index + 1, variable_infos[index], valueToNumber(tmp_val));
        } else if (IS_UNDEFINED(tmp_val)) {
            printf ("    [%d]  %-15s undefined\r\n", index + 1, variable_infos[index]);
        } else if (IS_NULL(tmp_val)) {
            printf ("    [%d]  %-15s null\r\n", index + 1, variable_infos[index]);
        } else if (IS_STRING(tmp_val)) {
            printf ("    [%d]  %-15s '%s'\r\n", index + 1, variable_infos[index], AS_STRING(tmp_val));
        }
    }

    index = 0;
    t_brama_byte val;

    printf ("\r\n[OPCODES] [%d]\r\n", bytes->length - 1);
    vec_foreach(bytes, val, index) {
        t_brama_vmdata vmdata;
        vm_decode(val, &vmdata);

        switch (vmdata.op) {
            /* Print just operator name */
            case VM_OPT_HALT:
                printf ("    [%d]    %-10s\r\n", index, VM_OPCODES[(int)vmdata.op].name);
                break;

            /* Print reg1*/
            case VM_OPT_INC:
            case VM_OPT_DINC:
            case VM_OPT_IF_EQ:
                printf ("    [%d]    %-10s %2d\r\n", index, VM_OPCODES[(int)vmdata.op].name, vmdata.reg1);
                break;

            /* Print reg1 and reg2 */
            case VM_OPT_COPY:
            case VM_OPT_INIT_VAR:
                printf ("    [%d]    %-10s %2d %2d\r\n", index, VM_OPCODES[(int)vmdata.op].name, vmdata.reg1, vmdata.reg2);
                break;

            /* Print reg1 and scal */
            case VM_OPT_JMP:
                printf ("    [%d]    %-10s %2d %2d\r\n", index, VM_OPCODES[(int)vmdata.op].name, vmdata.reg1, vmdata.scal);
                break;

            /* Print reg1, reg2 and  reg3*/
            default:
                printf ("    [%d]    %-10s %2d %2d %2d\r\n", index, VM_OPCODES[(int)vmdata.op].name, vmdata.reg1, vmdata.reg2, vmdata.reg3);
                break;
        }
    }
            
    BRAMA_FREE(variable_infos);
}

void run(t_context_ptr context) {
    vec_byte_ptr bytes      = context->compiler->op_codes;
    vec_value_ptr constants = &context->compiler->global_storage->constants;
    vec_value_ptr variables = &context->compiler->global_storage->variables;

    size_t total_bytes = bytes->length;
    t_brama_byte* ipc  = &bytes->data[0];

    while (*ipc != NULL) {
        t_brama_vmdata vmdata;
        vm_decode(*ipc, &vmdata);

        switch (vmdata.op) {

            case VM_OPT_INIT_VAR: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                variables->data[vmdata.reg1 - 1] = left;
                break;
            }

            case VM_OPT_IF_EQ: {
                t_brama_value variable = vmdata.reg1 < 0 ? constants->data[sabs8(vmdata.reg1) - 1] : variables->data[vmdata.reg1 - 1];
                if (IS_TRUE(variable))
                    ++ipc;
                break;
            }

            case VM_OPT_JMP: {
                ipc += vmdata.scal;
                break;
            }

            /* Actually this code copy variable from reg2 to reg1*/
            case VM_OPT_COPY:
                variables->data[vmdata.reg1 - 1] = variables->data[vmdata.reg2 - 1];
            break;

            /* Increment variable (++) */
            case VM_OPT_INC: {
                t_brama_value variable = variables->data[vmdata.reg1 - 1];
                if (IS_NUM(variable)) {
                    variables->data[vmdata.reg1 - 1] = numberToValue(valueToNumber(variable) + 1);
                }
                break;
            }

            /* Decrement variable (--) */
            case VM_OPT_DINC: {
                t_brama_value variable = variables->data[vmdata.reg1 - 1];
                if (IS_NUM(variable)) {
                    variables->data[vmdata.reg1 - 1] = numberToValue(valueToNumber(variable) - 1);
                }
                break;
            }

            /* CONTROLS */
            /* '<' operator */
            case VM_OPT_LT: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right))  {
                    variables->data[vmdata.reg1 - 1] = valueToNumber(left) < valueToNumber(right) ? TRUE_VAL : FALSE_VAL;
                }
                break;
            }

                /* '<=' operator */
            case VM_OPT_LTE: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right))  {
                    variables->data[vmdata.reg1 - 1] = valueToNumber(left) <= valueToNumber(right) ? TRUE_VAL : FALSE_VAL;
                }
                break;
            }
                /* '>' operator */
            case VM_OPT_GT: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right))  {
                    variables->data[vmdata.reg1 - 1] = valueToNumber(left) > valueToNumber(right) ? TRUE_VAL : FALSE_VAL;
                }
                break;
            }

                /* '>=' operator */
            case VM_OPT_GTE: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right))  {
                    variables->data[vmdata.reg1 - 1] = numberToValue(valueToNumber(left) >= valueToNumber(right)) ? TRUE_VAL : FALSE_VAL;
                }
                break;
            }

            /* CONTROLS */

            case VM_OPT_ADDITION: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right))  {
                    variables->data[vmdata.reg1 - 1] = numberToValue(valueToNumber(left) + valueToNumber(right));
                } else if (IS_UNDEFINED(left) && IS_UNDEFINED(right)) {
                    variables->data[vmdata.reg1 - 1] = NULL_VAL;
                } else if (IS_STRING(left) || IS_STRING(right)) {
                    if (IS_STRING(left) && IS_STRING(right)) {
                        char_ptr tmp = BRAMA_MALLOC(sizeof(char) * ((strlen(AS_STRING(left))) + (strlen(AS_STRING(right))) + 1));
                        strcpy(tmp, AS_STRING(left));
                        strcpy(tmp + strlen(AS_STRING(left)), AS_STRING(right));
                        tmp[(strlen(AS_STRING(left))) + (strlen(AS_STRING(right)))] = '\0';

                        t_vm_object_ptr object = new_vm_object(context);
                        object->type           = CONST_STRING;
                        object->char_ptr       = tmp;
                        variables->data[vmdata.reg1 - 1] = GET_VALUE_FROM_OBJ(object);
                        //compile_info->index = (storage->constants.length) * -1;
                    }
                }
                break;
            }

            case VM_OPT_SUBTRACTION: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right)) {
                    variables->data[vmdata.reg1 - 1] = numberToValue(valueToNumber(left) - valueToNumber(right));
                }
                else if (IS_UNDEFINED(left) || IS_UNDEFINED(right)) {
                    variables->data[vmdata.reg1 - 1] = UNDEFINED_VAL;
                }
                break;
            }

            case VM_OPT_DIVISION: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right)) {
                    variables->data[vmdata.reg1 - 1] = numberToValue(valueToNumber(left) / valueToNumber(right));
                }
                else if (IS_UNDEFINED(left) || IS_UNDEFINED(right)) {
                    variables->data[vmdata.reg1 - 1] = UNDEFINED_VAL;
                }
            }
                break;

            case VM_OPT_MULTIPLICATION: {
                t_brama_value left  = vmdata.reg2 < 0 ? constants->data[sabs8(vmdata.reg2) - 1] : variables->data[vmdata.reg2 - 1];
                t_brama_value right = vmdata.reg3 < 0 ? constants->data[sabs8(vmdata.reg3) - 1] : variables->data[vmdata.reg3 - 1];

                if (IS_NUM(left) && IS_NUM(right)) {
                    variables->data[vmdata.reg1 - 1] = numberToValue(valueToNumber(left) * valueToNumber(right));
                }
                else if (IS_UNDEFINED(left) || IS_UNDEFINED(right)) {
                    variables->data[vmdata.reg1 - 1] = UNDEFINED_VAL;
                }
                break;
            }
        }

        ++ipc;
    }
}

/* Compile End
 *
 *
function d(a, b)
    local erhan;
    if a == 1 then
        return a + 1
    else
        return b + 4
    end
    return d
end
 */

/* VM Begin */

t_vm_object_ptr new_vm_object(t_context_ptr context) {
    t_vm_object_ptr obj = BRAMA_MALLOC(sizeof(t_vm_object));
    obj->next   = context->compiler->head;
    obj->marked = false;
    context->compiler->head = obj;
    ++context->compiler->total_object;
    return obj;
}


void vm_decode(t_brama_byte instr, t_brama_vmdata_ptr t) {
    t->op   = (instr & OP_MASK  ) >> 24;
    t->reg1 = (instr & REG1_MASK) >> 16;
    t->reg2 = (instr & REG2_MASK) >> 8;
    t->reg3 = (instr & REG3_MASK);
    t->scal = (instr & SCAL_MASK);
}

t_brama_byte vm_encode(t_brama_vmdata_ptr t) {
    t_brama_byte instr = ((t->op   << 24) & OP_MASK  ) |
                         ((t->reg1 << 16) & REG1_MASK) |
                         ((t->reg2 << 8 ) & REG2_MASK) |
                         ((t->reg3      ) & REG3_MASK) | 
                         ((t->scal      ) & SCAL_MASK);
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

    /* Remove all objects 
     * Simple GC         */
    t_vm_object_ptr _object = context->compiler->head;
    while (_object != NULL) {
        t_vm_object_ptr next_object = _object->next;

        if (_object->type == CONST_STRING) 
            BRAMA_FREE(_object->char_ptr);

        BRAMA_FREE(_object);
        _object = next_object;
    }

    context->compiler->total_object = 0;

    if (context->error_message != NULL)
        BRAMA_FREE(context->error_message);

    vec_deinit(context->compiler->op_codes);
    BRAMA_FREE(context->compiler->op_codes);

    vec_deinit(&context->compiler->storages);
    vec_deinit(&context->compiler->global_storage->constants);
    vec_deinit(&context->compiler->global_storage->variables);
    map_deinit(&context->compiler->global_storage->variable_names);
    BRAMA_FREE(context->compiler->global_storage);

    vec_deinit(_context->tokinizer->tokens);
    BRAMA_FREE(_context->tokinizer->tokens);

    vec_deinit(_context->parser->asts);
    BRAMA_FREE(_context->parser->asts);

    map_deinit(&_context->tokinizer->keywords);

    BRAMA_FREE(_context->tokinizer);
    BRAMA_FREE(_context->parser);
    BRAMA_FREE(_context->compiler);
    BRAMA_FREE(_context);
}
