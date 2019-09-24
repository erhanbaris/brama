#include "static_py.h"
#include "string_stream.h"

#include <string.h>
#include <stdbool.h>
#include <math.h>

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

void getSymbol(t_tokinizer* tokinizer) {
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
        //printf("%s = %d\r\n", data, *keywordInfo);

        t_token* token = (t_token*)malloc(sizeof (t_token));
        token->type    = TOKEN_KEYWORD;
        token->current = tokinizer->column;
        token->line    = tokinizer->line;
        token->int_    = *keywordInfo;

        increase(tokinizer);
        vector_add(&tokinizer->tokens, token);
        free(data);
    } else {

        t_token* token  = (t_token*)malloc(sizeof (t_token));
        token->type     = TOKEN_SYMBOL;
        token->current  = tokinizer->column;
        token->line     = tokinizer->line;
        token->char_ptr = data;

        increase(tokinizer);
        vector_add(&tokinizer->tokens, token);
    }

    string_stream_destroy(stream);
}

int getText(t_tokinizer* tokinizer, char symbol) {
    t_string_stream* stream = string_stream_init();

    increase(tokinizer);
    char ch = getChar(tokinizer);
    char chNext;

    while (!isEnd(tokinizer) && ch != symbol) {
        ch = getChar(tokinizer);
        chNext = getNextChar(tokinizer);

        if (ch == '$')
            getSymbol(tokinizer);
        else if (ch == '\\' && chNext == symbol) {
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
        return STATIC_PY_NOK;

    t_token* token  = (t_token*)malloc(sizeof (t_token));
    token->type     = TOKEN_TEXT;
    token->current  = tokinizer->column;
    token->line     = tokinizer->line;
    token->char_ptr = string_stream_get(stream);

    increase(tokinizer);
    vector_add(&tokinizer->tokens, token);

    string_stream_destroy(stream);
    return STATIC_PY_OK;
}

void getNumber(t_tokinizer* tokinizer) {
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
            if (chNext == '.')
                break;

            if (isDouble) {
                //error(_T("Number problem"));
                break;
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

        vector_add(&tokinizer->tokens, token);
    }

    vector_add(&tokinizer->tokens, token);
}

void getOperator(t_tokinizer* tokinizer) {
    char ch      = getChar(tokinizer);
    char chNext  = getNextChar(tokinizer);
    char chThird = getThirdChar(tokinizer);
    increase(tokinizer);

    if (ch == '-' && isInteger(getNextChar(tokinizer)))
        getNumber(tokinizer);
    else {
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

        vector_add(&tokinizer->tokens, token);
    }
}

t_context* static_py_init() {
    t_context* context         = (t_context*)malloc(sizeof(t_context));
    context->tokinizer         = (t_tokinizer*)malloc(sizeof(t_tokinizer));
    context->tokinizer->column = 0;
    context->tokinizer->index  = 0;
    context->tokinizer->line   = 1;
    map_init(&context->tokinizer->keywords);

    size_t keywordCount = sizeof (KEYWORDS_PAIR) / sizeof(KeywordPair);
    for (size_t i = 0; i < keywordCount; ++i)
        map_set(&context->tokinizer->keywords, KEYWORDS_PAIR[i].name,  KEYWORDS_PAIR[i].keyword);

    return context;
}

void static_py_execute(t_context* context, char* data) {
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
            getSymbol(tokinizer);
            continue;
        } else if (ch == '"') {
            getText(tokinizer, '"');
            continue;
        } else if (ch == '\'') {
            getText(tokinizer, '\'');
            continue;
        } else if (ch >= '0' && ch <= '9') {
            getNumber(tokinizer);
            continue;
        } else {
            getOperator(tokinizer);
            continue;
        }
    }
}

void static_py_destroy(t_context* context) {
    t_context* _context = (t_context*)context;
    int i;
    int totalToken = vector_count(&_context->tokinizer->tokens);
    for (i = 0; i < totalToken; i++) {
        t_token* token = (t_token*)vector_get(&_context->tokinizer->tokens, i);
        if (token->type == TOKEN_TEXT ||
            token->type == TOKEN_SYMBOL)
            free((char*)token->char_ptr);
    }

    map_deinit(&_context->tokinizer->keywords);
    free(_context->tokinizer);
    free(_context);
}
