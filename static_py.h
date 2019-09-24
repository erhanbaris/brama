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


t_context* static_py_init();
void       static_py_execute(t_context* context, char* data);
void       static_py_destroy(t_context*);


#endif // STATIC_PY_H
