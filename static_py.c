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

void getSymbol(t_tokinizer* tokinizer)
{
    t_string_stream* stream = string_stream_init();

    char ch;
    while (!isEnd(tokinizer))
    {
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

    if (keywordInfo){
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

int getText(t_tokinizer* tokinizer, char symbol)
{
    t_string_stream* stream = string_stream_init();

    increase(tokinizer);
    char ch = getChar(tokinizer);
    char chNext;

    while (!isEnd(tokinizer) && ch != symbol)
    {
        ch = getChar(tokinizer);
        chNext = getNextChar(tokinizer);

        if (ch == '$')
            getSymbol(tokinizer);
        else if (ch == '\\' && chNext == symbol)
        {
            string_stream_add_char(stream, symbol);
            increase(tokinizer);
        }
        else if (ch == symbol)
        {
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

void getNumber(t_tokinizer* tokinizer)
{
    bool isMinus       = false;
    int dotPlace       = 0;
    int beforeTheComma = 0;
    int afterTheComma  = 0;
    size_t start       = tokinizer->column;
    bool isDouble      = false;
    char ch            = getChar(tokinizer);
    char chNext        = getNextChar(tokinizer);

    while (!isEnd(tokinizer))
    {
        if (ch == '-')
        {
            if (isMinus || (beforeTheComma > 0 || afterTheComma > 0))
                break;

            isMinus = true;
        }
        else if (ch == '.')
        {
            if (chNext == '.')
                break;

            if (isDouble)
            {
                //error(_T("Number problem"));
                break;
            }

            isDouble = true;
        }
        else if ((ch >= '0' && ch <= '9'))
        {
            if (isDouble)
            {
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
        token->int_    = OPERATOR_MINUS;

        vector_add(&tokinizer->tokens, token);
    }

    vector_add(&tokinizer->tokens, token);
}

void getOperator(t_tokinizer* tokinizer)
{

}

t_context* static_py_init() {
    t_context* context         = (t_context*)malloc(sizeof(t_context));
    context->tokinizer         = (t_tokinizer*)malloc(sizeof(t_tokinizer));
    context->tokinizer->column = 0;
    context->tokinizer->index  = 0;
    context->tokinizer->line   = 1;
    map_init(&context->tokinizer->keywords);

    map_set(&context->tokinizer->keywords, "do",  KEYWORD_DO);
    map_set(&context->tokinizer->keywords, "if",  KEYWORD_IF);
    map_set(&context->tokinizer->keywords, "in",  KEYWORD_IN);
    map_set(&context->tokinizer->keywords, "for",  KEYWORD_FOR);
    map_set(&context->tokinizer->keywords, "let",  KEYWORD_LET);
    map_set(&context->tokinizer->keywords, "new",  KEYWORD_NEW);
    map_set(&context->tokinizer->keywords, "try",  KEYWORD_TRY);
    map_set(&context->tokinizer->keywords, "var",  KEYWORD_VAR);
    map_set(&context->tokinizer->keywords, "case",  KEYWORD_CASE);
    map_set(&context->tokinizer->keywords, "else",  KEYWORD_ELSE);
    map_set(&context->tokinizer->keywords, "enum",  KEYWORD_ENUM);
    map_set(&context->tokinizer->keywords, "eval",  KEYWORD_EVAL);
    map_set(&context->tokinizer->keywords, "null",  KEYWORD_NULL);
    map_set(&context->tokinizer->keywords, "this",  KEYWORD_THIS);
    map_set(&context->tokinizer->keywords, "true",  KEYWORD_TRUE);
    map_set(&context->tokinizer->keywords, "void",  KEYWORD_VOID);
    map_set(&context->tokinizer->keywords, "with",  KEYWORD_WITH);
    map_set(&context->tokinizer->keywords, "break",  KEYWORD_BREAK);
    map_set(&context->tokinizer->keywords, "catch",  KEYWORD_CATCH);
    map_set(&context->tokinizer->keywords, "class",  KEYWORD_CLASS);
    map_set(&context->tokinizer->keywords, "const",  KEYWORD_CONST);
    map_set(&context->tokinizer->keywords, "false",  KEYWORD_FALSE);
    map_set(&context->tokinizer->keywords, "super",  KEYWORD_SUPER);
    map_set(&context->tokinizer->keywords, "throw",  KEYWORD_THROW);
    map_set(&context->tokinizer->keywords, "while",  KEYWORD_WHILE);
    map_set(&context->tokinizer->keywords, "yield",  KEYWORD_YIELD);
    map_set(&context->tokinizer->keywords, "delete",  KEYWORD_DELETE);
    map_set(&context->tokinizer->keywords, "export",  KEYWORD_EXPORT);
    map_set(&context->tokinizer->keywords, "import",  KEYWORD_IMPORT);
    map_set(&context->tokinizer->keywords, "public",  KEYWORD_PUBLIC);
    map_set(&context->tokinizer->keywords, "return",  KEYWORD_RETURN);
    map_set(&context->tokinizer->keywords, "static",  KEYWORD_STATIC);
    map_set(&context->tokinizer->keywords, "switch",  KEYWORD_SWITCH);
    map_set(&context->tokinizer->keywords, "typeof",  KEYWORD_TYPEOF);
    map_set(&context->tokinizer->keywords, "default",  KEYWORD_DEFAULT);
    map_set(&context->tokinizer->keywords, "extends",  KEYWORD_EXTENDS);
    map_set(&context->tokinizer->keywords, "finally",  KEYWORD_FINALLY);
    map_set(&context->tokinizer->keywords, "package",  KEYWORD_PACKAGE);
    map_set(&context->tokinizer->keywords, "private",  KEYWORD_PRIVATE);
    map_set(&context->tokinizer->keywords, "continue",  KEYWORD_CONTINUE);
    map_set(&context->tokinizer->keywords, "debugger",  KEYWORD_DEBUGGER);
    map_set(&context->tokinizer->keywords, "function",  KEYWORD_FUNCTION);
    map_set(&context->tokinizer->keywords, "arguments",  KEYWORD_ARGUMENTS);
    map_set(&context->tokinizer->keywords, "interface",  KEYWORD_INTERFACE);
    map_set(&context->tokinizer->keywords, "protected",  KEYWORD_PROTECTED);
    map_set(&context->tokinizer->keywords, "implements",  KEYWORD_IMPLEMENTS);
    map_set(&context->tokinizer->keywords, "instanceof",  KEYWORD_INSTANCEOF);
    return context;
}

void static_py_execute(t_context* context, char* data) {
    t_tokinizer* tokinizer   = ((t_context*)context)->tokinizer;
    tokinizer->content       = data;
    tokinizer->contentLength = strlen(data);

    while (!isEnd(tokinizer)) {
        char ch = getChar(tokinizer);
        char chNext = getNextChar(tokinizer);

        if (isWhitespace(ch)) {
            while (!isEnd(tokinizer) && isWhitespace(ch))
            {
                increase(tokinizer);

                if (isNewLine(ch))
                {
                    tokinizer->column = 0;
                    ++tokinizer->line;
                }

                ch = getChar(tokinizer);
            }

            continue;
        } else if (ch == '/' && chNext == '/') {
            while (!isEnd(tokinizer) && !isNewLine(ch))
            {
                increase(tokinizer);
                ch = getChar(tokinizer);
            }

            tokinizer->column = 0;
            ++tokinizer->line;
            continue;
        } else if (ch == '_' && (chNext == '\0' || (isSymbol(ch) == false && ch >= '0' && ch <= '9'))) {
            t_token* token = (t_token*)malloc(sizeof (t_token));
            token->type    = TOKEN_OPERATOR;
            token->current = tokinizer->column;
            token->line    = tokinizer->line;
            token->int_    = OPERATOR_UNDERLINE;

            increase(tokinizer);
            vector_add(&tokinizer->tokens, token);
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
        } else if ((ch >= '0' && ch <= '9') || ch == '.') {
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
    for (i = 0; i < vector_count(&_context->tokinizer->tokens); i++) {
        t_token* token = (t_token*)vector_get(&_context->tokinizer->tokens, i);
        if (token->type == TOKEN_TEXT ||
            token->type == TOKEN_SYMBOL)
            free((char*)token->char_ptr);
    }

    map_deinit(&_context->tokinizer->keywords);
    free(_context->tokinizer);
    free(_context);
}
