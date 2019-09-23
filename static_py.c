#include "static_py.h"
#include "map.h"
#include "vector.h"
#include "string_stream.h"

#include <string.h>
#include <stdbool.h>

/* CODES */
#define STATIC_PY_OK      0
#define STATIC_PY_NOK     1


/* TOKEN TYPES */
#define TOKEN_NONE        1
#define TOKEN_INTEGER     2
#define TOKEN_DOUBLE      3
#define TOKEN_SYMBOL      4
#define TOKEN_OPERATOR    5
#define TOKEN_TEXT        6
#define TOKEN_VARIABLE    7
#define TOKEN_KEYWORD     8
#define TOKEN_END_OF_FILE 9

/* KEYWORD TYPES */
#define KEYWORD_DO         1
#define KEYWORD_IF         2
#define KEYWORD_IN         3
#define KEYWORD_FOR        4
#define KEYWORD_LET        5
#define KEYWORD_NEW        6
#define KEYWORD_TRY        7
#define KEYWORD_VAR        8
#define KEYWORD_CASE       9
#define KEYWORD_ELSE       10
#define KEYWORD_ENUM       11
#define KEYWORD_EVAL       12
#define KEYWORD_NULL       13
#define KEYWORD_THIS       14
#define KEYWORD_TRUE       15
#define KEYWORD_VOID       16
#define KEYWORD_WITH       17
#define KEYWORD_BREAK      18
#define KEYWORD_CATCH      19
#define KEYWORD_CLASS      20
#define KEYWORD_CONST      21
#define KEYWORD_FALSE      22
#define KEYWORD_SUPER      23
#define KEYWORD_THROW      24
#define KEYWORD_WHILE      25
#define KEYWORD_YIELD      26
#define KEYWORD_DELETE     27
#define KEYWORD_EXPORT     28
#define KEYWORD_IMPORT     29
#define KEYWORD_PUBLIC     30
#define KEYWORD_RETURN     31
#define KEYWORD_STATIC     32
#define KEYWORD_SWITCH     33
#define KEYWORD_TYPEOF     34
#define KEYWORD_DEFAULT    35
#define KEYWORD_EXTENDS    36
#define KEYWORD_FINALLY    37
#define KEYWORD_PACKAGE    38
#define KEYWORD_PRIVATE    39
#define KEYWORD_CONTINUE   40
#define KEYWORD_DEBUGGER   41
#define KEYWORD_FUNCTION   42
#define KEYWORD_ARGUMENTS  43
#define KEYWORD_INTERFACE  44
#define KEYWORD_PROTECTED  45
#define KEYWORD_IMPLEMENTS 46
#define KEYWORD_INSTANCEOF 47

/* OPERATOR TYPES */
#define OPERATOR_NONE                 1
#define OPERATOR_OPERATION            2
#define OPERATOR_PLUS                 3
#define OPERATOR_MINUS                4
#define OPERATOR_MULTIPLICATION       5
#define OPERATOR_DIVISION             6
#define OPERATOR_EQUAL                7
#define OPERATOR_NOT_EQUAL            8
#define OPERATOR_GREATOR              9
#define OPERATOR_LOWER                10
#define OPERATOR_GREATOR_EQUAL        11
#define OPERATOR_LOWER_EQUAL          12
#define OPERATOR_SINGLE_QUOTES        13
#define OPERATOR_DOUBLE_QUOTES        14
#define OPERATOR_LEFT_PARENTHESES     15
#define OPERATOR_RIGHT_PARENTHESES    16
#define OPERATOR_SQUARE_BRACKET_START 17
#define OPERATOR_SQUARE_BRACKET_END   18
#define OPERATOR_BLOCK_START          19
#define OPERATOR_BLOCK_END            20
#define OPERATOR_OR                   21
#define OPERATOR_AND                  22
#define OPERATOR_COMMA                23
#define OPERATOR_ASSIGN               24
#define OPERATOR_SINGLE_COLON         25
#define OPERATOR_DOUBLE_COLON         26
#define OPERATOR_UNDERLINE            27
#define OPERATOR_APPEND               28
#define OPERATOR_INDEXER              29

typedef struct {
    size_t line;
    size_t current;
    size_t type;
    void*  data;
} t_token;

typedef struct {
    size_t line;
    size_t column;
    size_t index;
    size_t contentLength;
    char* content;
    vector tokens;
    map_int_t keywords;
} t_tokinizer;

typedef struct {
    t_tokinizer* tokinizer;
} t_context;

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
    tokinizer->keywords;
    int* keywordInfo = map_get(&tokinizer->keywords, data);

    if (keywordInfo){
        printf("%s = %d\r\n", data, *keywordInfo);

        t_token* token = (t_token*)malloc(sizeof (t_token));
        token->type    = TOKEN_KEYWORD;
        token->current = tokinizer->column;
        token->line    = tokinizer->line;
        token->data    = (void*)*keywordInfo;

        increase(tokinizer);
        vector_add(&tokinizer->tokens, token);
        free(data);
    } else {

        t_token* token = (t_token*)malloc(sizeof (t_token));
        token->type    = TOKEN_SYMBOL;
        token->current = tokinizer->column;
        token->line    = tokinizer->line;
        token->data    = (void*)data;

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

    t_token* token = (t_token*)malloc(sizeof (t_token));
    token->type    = TOKEN_TEXT;
    token->current = tokinizer->column;
    token->line    = tokinizer->line;
    token->data    = (void*)string_stream_get(stream);

    increase(tokinizer);
    vector_add(&tokinizer->tokens, token);

    string_stream_destroy(stream);
    return STATIC_PY_OK;
}

void* static_py_init() {
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

    printf("context created");
    return context;
}

static size_t djb_hash(const char* cp)
{
    size_t hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char) *cp++;
    return hash;
}

/* Fowler/Noll/Vo (FNV) hash function, variant 1a */
static size_t fnv1a_hash(const char* cp)
{
    size_t hash = 0x811c9dc5;
    while (*cp) {
        hash ^= (unsigned char) *cp++;
        hash *= 0x01000193;
    }
    return hash;
}

void static_py_execute(void* context, char* data) {
    t_tokinizer* tokinizer   = ((t_context*)context)->tokinizer;
    tokinizer->content       = data;
    tokinizer->contentLength = strlen(data);

    while (!isEnd(tokinizer)) {
        char ch = getChar(tokinizer);
        char chNext = getNextChar(tokinizer);

        if (isWhitespace(ch))
        {
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
            token->data    = (void*)OPERATOR_UNDERLINE;

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
        }
    }
}

void static_py_destroy(void* context) {
    t_context* _context = (t_context*)context;
    int i;
    for (i = 0; i < vector_count(&_context->tokinizer->tokens); i++) {
        t_token* token = (t_token*)vector_get(&_context->tokinizer->tokens, i);
        if (token->type == TOKEN_TEXT ||
            token->type == TOKEN_SYMBOL)
            free((char*)token->data);
    }

    map_deinit(&_context->tokinizer->keywords);
    free(_context->tokinizer);
    free(_context);

    printf("context destroyed");
}
