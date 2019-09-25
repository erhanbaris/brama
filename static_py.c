#include "static_py.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>


/* TOKINIZER OPERATIONS START */
bool isEnd(t_tokinizer* tokinizer) {
    return tokinizer->contentLength <= tokinizer->index;
}

char getChar(t_tokinizer* tokinizer) {
    if (!isEnd(tokinizer))
        return tokinizer->content[tokinizer->index];

    return '\0';
}

char getNextChar(t_tokinizer* tokinizer) {
    if (tokinizer->contentLength > (tokinizer->index + 1))
        return tokinizer->content[tokinizer->index + 1];

    return '\0';
}

char getThirdChar(t_tokinizer* tokinizer) {
    if (tokinizer->contentLength > (tokinizer->index + 2))
        return tokinizer->content[tokinizer->index + 2];

    return '\0';
}


void increase(t_tokinizer* tokinizer) {
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
            (ch >= 'A' && ch <= 'Z'));
}

int getSymbol(t_tokinizer* tokinizer) {
    t_string_stream* stream = string_stream_init();

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

    char* data = string_stream_get(stream);
    int* keywordInfo = (int*)map_get(&tokinizer->keywords, data);

    if (keywordInfo) {
        t_token* token = (t_token*)malloc(sizeof (t_token));
        token->type    = TOKEN_KEYWORD;
        token->current = tokinizer->column;
        token->line    = tokinizer->line;
        token->int_    = *keywordInfo;

        vector_add(tokinizer->tokens, token);
        free(data);
    } else {
        t_token* token  = (t_token*)malloc(sizeof (t_token));
        token->type     = TOKEN_SYMBOL;
        token->current  = tokinizer->column;
        token->line     = tokinizer->line;
        token->char_ptr = data;

        vector_add(tokinizer->tokens, token);
    }

    string_stream_destroy(stream);
    return STATIC_PY_OK;
}

int getText(t_tokinizer* tokinizer, char symbol) {
    t_string_stream* stream = string_stream_init();

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

    t_token* token  = (t_token*)malloc(sizeof (t_token));
    token->type     = TOKEN_TEXT;
    token->current  = tokinizer->column;
    token->line     = tokinizer->line;
    token->char_ptr = string_stream_get(stream);
    vector_add(tokinizer->tokens, token);

    if (stream->text_length == 0)
        increase(tokinizer);

    string_stream_destroy(stream);
    return STATIC_PY_OK;
}

int getNumber(t_tokinizer* tokinizer) {
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

    t_token* token = (t_token*)malloc(sizeof (t_token));

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
        t_token* token = (t_token*)malloc(sizeof (t_token));
        token->type    = TOKEN_OPERATOR;
        token->current = tokinizer->column;
        token->line    = tokinizer->line;
        token->int_    = OPERATOR_SUBTRACTION;

        vector_add(tokinizer->tokens, token);
    }

    vector_add(tokinizer->tokens, token);
    return STATIC_PY_OK;
}

int getOperator(t_tokinizer* tokinizer) {
    char ch      = getChar(tokinizer);
    char chNext  = getNextChar(tokinizer);
    char chThird = getThirdChar(tokinizer);
    increase(tokinizer);

    if (ch == '-' && isInteger(getNextChar(tokinizer)))
        return getNumber(tokinizer);

    t_token* token = (t_token*)malloc(sizeof (t_token));
    token->type    = TOKEN_OPERATOR;
    token->current = tokinizer->column;
    token->line    = tokinizer->line;
    token->int_    = OPERATOR_NONE;

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
        OPERATOR_CASE_SINGLE(',', OPERATOR_COMMA);
        OPERATOR_CASE_SINGLE(';', OPERATOR_SEMICOLON);
        OPERATOR_CASE_SINGLE('.', OPERATOR_DOT);
    }
    if (token->int_ == OPERATOR_NONE)
        return STATIC_PY_NOK;

    vector_add(tokinizer->tokens, token);
    return STATIC_PY_OK;
}

int static_py_tokinize(t_context* context, char* data) {
    t_tokinizer* tokinizer   = ((t_context*)context)->tokinizer;
    tokinizer->content       = data;
    tokinizer->contentLength = strlen(data);

    while (!isEnd(tokinizer)) {
        char ch     = getChar(tokinizer);
        char chNext = getNextChar(tokinizer);

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



t_context* static_py_init() {
    t_context* context                = (t_context*)malloc(sizeof(t_context));
    context->tokinizer                = (t_tokinizer*)malloc(sizeof(t_tokinizer));
    context->tokinizer->column        = 0;
    context->tokinizer->index         = 0;
    context->tokinizer->line          = 1;
    context->tokinizer->error_message = NULL;
    context->tokinizer->tokens        = vector_init();
    map_init(&context->tokinizer->keywords);

    size_t keywordCount = sizeof (KEYWORDS_PAIR) / sizeof(KeywordPair);
    for (size_t i = 0; i < keywordCount; ++i)
        map_set(&context->tokinizer->keywords, KEYWORDS_PAIR[i].name,  KEYWORDS_PAIR[i].keyword);

    return context;
}

char* static_py_set_error(t_context* context, int error) {
    if (error == STATIC_PY_MISSING_TEXT_DELIMITER) {
        char* buffer = malloc(sizeof(char) * 128);
        sprintf(buffer, "Missing Delimiter at Line: %d, Column: %d", context->tokinizer->line, context->tokinizer->column);
        return buffer;
    } else if (error == STATIC_PY_MULTIPLE_DOT_ON_DOUBLE) {
        char* buffer = malloc(sizeof(char) * 128);
        sprintf(buffer, "Multiple dot used for double: %d, Column: %d", context->tokinizer->line, context->tokinizer->column);
        return buffer;
    }

    return NULL;
}

void static_py_execute(t_context* context, char* data) {
    int tokinizer_status = static_py_tokinize(context, data);
    if (tokinizer_status != STATIC_PY_OK) {
        if (context->tokinizer->error_message != NULL && strlen(context->tokinizer->error_message) > 0)
            free(context->tokinizer->error_message);

        context->tokinizer->error_message = static_py_set_error(context, tokinizer_status);
    }
}

void static_py_dump(t_context* context) {
    t_context* _context = (t_context*)context;
    int i;
    int totalToken = _context->tokinizer->tokens->count;
    for (i = 0; i < totalToken; i++) {
        t_token* token = (t_token*)vector_get(_context->tokinizer->tokens, i);
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

void static_py_destroy(t_context* context) {
    t_context* _context = (t_context*)context;
    size_t i;
    size_t totalToken = _context->tokinizer->tokens->count;
    for (i = 0; i < totalToken; i++) {
        t_token* token = (t_token*)vector_get(_context->tokinizer->tokens, i);
        if (token->type == TOKEN_TEXT ||
            token->type == TOKEN_SYMBOL)
            free((char*)token->char_ptr);
    }

    map_deinit(&_context->tokinizer->keywords);
    free(_context->tokinizer);
    free(_context);
}
