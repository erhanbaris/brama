#ifndef STATIC_PY_H
#define STATIC_PY_H

#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "vector.h"


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
#define OPERATOR_NONE                    0
#define OPERATOR_ADDITION                1
#define OPERATOR_SUBTRACTION             2
#define OPERATOR_MULTIPLICATION          3
#define OPERATOR_DIVISION                4
#define OPERATOR_MODULES                 5
#define OPERATOR_INCREMENT               6
#define OPERATOR_DECCREMENT              7
#define OPERATOR_ASSIGN                  8
#define OPERATOR_ASSIGN_ADDITION         9
#define OPERATOR_ASSIGN_SUBTRACTION      10
#define OPERATOR_ASSIGN_MULTIPLICATION   11
#define OPERATOR_ASSIGN_DIVISION         12
#define OPERATOR_ASSIGN_MODULUS          13
#define OPERATOR_EQUAL                   14
#define OPERATOR_EQUAL_VALUE             15
#define OPERATOR_NOT_EQUAL               16
#define OPERATOR_NOT_EQUAL_VALUE         17
#define OPERATOR_NOT                     18
#define OPERATOR_AND                     19
#define OPERATOR_OR                      20
#define OPERATOR_BITWISE_AND             21
#define OPERATOR_BITWISE_OR              22
#define OPERATOR_BITWISE_NOT             23
#define OPERATOR_BITWISE_XOR             24
#define OPERATOR_BITWISE_LEFT_SHIFT      25
#define OPERATOR_BITWISE_RIGHT_SHIFT     26
#define OPERATOR_GREATER_THAN            27
#define OPERATOR_LESS_THAN               28
#define OPERATOR_GREATER_EQUAL_THAN      29
#define OPERATOR_LESS_EQUAL_THAN         30
#define OPERATOR_QUESTION_MARK           31
#define OPERATOR_COLON_MARK              32
#define OPERATOR_BITWISE_AND_ASSIGN      33
#define OPERATOR_BITWISE_OR_ASSIGN       34
#define OPERATOR_BITWISE_XOR_ASSIGN      35
#define OPERATOR_LEFT_PARENTHESES        36
#define OPERATOR_RIGHT_PARENTHESES       37
#define OPERATOR_SQUARE_BRACKET_START    38
#define OPERATOR_SQUARE_BRACKET_END      39
#define OPERATOR_COMMA                   40
#define OPERATOR_SEMICOLON               41
#define OPERATOR_DOT                     42
#define OPERATOR_COMMENT_LINE            43
#define OPERATOR_COMMENT_MULTILINE_START 44
#define OPERATOR_COMMENT_MULTILINE_END   45

typedef struct {
  char* name;
  int   keyword;
} KeywordPair;

typedef struct {
    size_t line;
    size_t current;
    size_t type;
    union {
        void* data_ptr;
        char* char_ptr;
        char char_;
        int   int_;
        double double_;
    };
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

#define OPERATOR_CASE_DOUBLE_START_WITH(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_3_SYMBOL, OPERATOR_1, OPERATOR_2, OPERATOR_3) \
    case OPERATOR_1_SYMBOL :                       \
        if (chNext == OPERATOR_2_SYMBOL ) {        \
            token->int_ = OPERATOR_2 ;             \
            increase(tokinizer);                   \
        } else if (chNext == OPERATOR_3_SYMBOL ) { \
            token->int_ = OPERATOR_3 ;             \
            increase(tokinizer);                   \
        } else token->int_ = OPERATOR_1 ;          \
        break;

#define OPERATOR_CASE_DOUBLE_START_WITH_FOUR(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_3_SYMBOL, OPERATOR_4_SYMBOL, OPERATOR_1, OPERATOR_2, OPERATOR_3, OPERATOR_4) \
    case OPERATOR_1_SYMBOL :                       \
        if (chNext == OPERATOR_2_SYMBOL ) {        \
            token->int_ = OPERATOR_2 ;             \
            increase(tokinizer);                   \
        } else if (chNext == OPERATOR_3_SYMBOL ) { \
            token->int_ = OPERATOR_3 ;             \
            increase(tokinizer);                   \
        } else if (chNext == OPERATOR_4_SYMBOL ) { \
            token->int_ = OPERATOR_4 ;             \
            increase(tokinizer);                   \
        } else token->int_ = OPERATOR_1 ;          \
        break;

#define OPERATOR_CASE_DOUBLE(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_1, OPERATOR_2)  \
    case OPERATOR_1_SYMBOL :                \
        if (chNext == OPERATOR_2_SYMBOL ) { \
            token->int_ = OPERATOR_2 ;      \
            increase(tokinizer);            \
        } else token->int_ = OPERATOR_1 ;   \
        break;

#define OPERATOR_CASE_TRIBLE(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_3_SYMBOL, OPERATOR_1, OPERATOR_2, OPERATOR_3) \
case OPERATOR_1_SYMBOL :                     \
    if (chNext == OPERATOR_2_SYMBOL ) {      \
        increase(tokinizer);                 \
        if (chThird == OPERATOR_3_SYMBOL ) { \
            token->int_ = OPERATOR_3 ;       \
            increase(tokinizer);             \
        } else token->int_ = OPERATOR_2 ;    \
    } else token->int_ = OPERATOR_1 ;        \
    break;

#define OPERATOR_CASE_SINGLE(OPERATOR_SYMBOL, OPERATOR)  case OPERATOR_SYMBOL :\
                                                                token->int_ = OPERATOR ;\
                                                            break;

static KeywordPair KEYWORDS_PAIR[] = {
   { "do",  KEYWORD_DO },
   { "if",  KEYWORD_IF },
   { "in",  KEYWORD_IN },
   { "for",  KEYWORD_FOR },
   { "let",  KEYWORD_LET },
   { "new",  KEYWORD_NEW },
   { "try",  KEYWORD_TRY },
   { "var",  KEYWORD_VAR },
   { "case",  KEYWORD_CASE },
   { "else",  KEYWORD_ELSE },
   { "enum",  KEYWORD_ENUM },
   { "eval",  KEYWORD_EVAL },
   { "null",  KEYWORD_NULL },
   { "this",  KEYWORD_THIS },
   { "true",  KEYWORD_TRUE },
   { "void",  KEYWORD_VOID },
   { "with",  KEYWORD_WITH },
   { "break",  KEYWORD_BREAK },
   { "catch",  KEYWORD_CATCH },
   { "class",  KEYWORD_CLASS },
   { "const",  KEYWORD_CONST },
   { "false",  KEYWORD_FALSE },
   { "super",  KEYWORD_SUPER },
   { "throw",  KEYWORD_THROW },
   { "while",  KEYWORD_WHILE },
   { "yield",  KEYWORD_YIELD },
   { "delete",  KEYWORD_DELETE },
   { "export",  KEYWORD_EXPORT },
   { "import",  KEYWORD_IMPORT },
   { "public",  KEYWORD_PUBLIC },
   { "return",  KEYWORD_RETURN },
   { "static",  KEYWORD_STATIC },
   { "switch",  KEYWORD_SWITCH },
   { "typeof",  KEYWORD_TYPEOF },
   { "default",  KEYWORD_DEFAULT },
   { "extends",  KEYWORD_EXTENDS },
   { "finally",  KEYWORD_FINALLY },
   { "package",  KEYWORD_PACKAGE },
   { "private",  KEYWORD_PRIVATE },
   { "continue",  KEYWORD_CONTINUE },
   { "debugger",  KEYWORD_DEBUGGER },
   { "function",  KEYWORD_FUNCTION },
   { "arguments",  KEYWORD_ARGUMENTS },
   { "interface",  KEYWORD_INTERFACE },
   { "protected",  KEYWORD_PROTECTED },
   { "implements",  KEYWORD_IMPLEMENTS },
   { "instanceof",  KEYWORD_INSTANCEOF }
 };


t_context* static_py_init();
void       static_py_execute(t_context* context, char* data);
void       static_py_destroy(t_context*);


#endif // STATIC_PY_H
