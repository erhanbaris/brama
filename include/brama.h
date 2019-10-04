#ifndef BRAMA_H
#define BRAMA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "map.h"
#include "macros.h"
#include "tools.h"

//#if (__STDC_VERSION__ >= 201112L)
//#endif


/* CODES */
#define BRAMA_STATUS                         int
#define BRAMA_OK                             0
#define BRAMA_NOK                            1
#define BRAMA_MISSING_TEXT_DELIMITER         2
#define BRAMA_MULTIPLE_DOT_ON_DOUBLE         3
#define BRAMA_PARSE_ERROR                    4
#define BRAMA_PARSE_ARRAY_INIT_NOT_PRIMATIVE 5
#define BRAMA_EXPRESSION_NOT_VALID           6
#define BRAMA_DOES_NOT_MATCH_AST             7
#define BRAMA_OPEN_OPERATOR_NOT_FOUND        8
#define BRAMA_CLOSE_OPERATOR_NOT_FOUND       9
#define BRAMA_DICTIONARY_NOT_VALID           10
#define BRAMA_FUNCTION_NAME_REQUIRED         11

/* PRIMATIVE TYPES */
#define PRIMATIVE_NONE       0
#define PRIMATIVE_INTEGER    1
#define PRIMATIVE_DOUBLE     2
#define PRIMATIVE_STRING     3
#define PRIMATIVE_BOOL       4
#define PRIMATIVE_ARRAY      5
#define PRIMATIVE_DICTIONARY 6
#define PRIMATIVE_NULL       7

/* TOKEN TYPES */
#define TOKEN_NONE        1
#define TOKEN_INTEGER     2
#define TOKEN_DOUBLE      3
#define TOKEN_SYMBOL      4
#define TOKEN_OPERATOR    5
#define TOKEN_TEXT        6
#define TOKEN_KEYWORD     7
#define TOKEN_END_OF_FILE 8

/* KEYWORD TYPES */
#define KEYWORD_NONE       0
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
#define OPERATOR_CURVE_BRACKET_START     46
#define OPERATOR_CURVE_BRACKET_END       47

/* AST TYPES */
#define AST_NONE                 0
#define AST_IF_STATEMENT         1
#define AST_ASSIGNMENT           2
#define AST_SYMBOL               3
#define AST_PRIMATIVE            4
#define AST_BINARY_OPERATION     5
#define AST_STRUCT_OPERATION     6
#define AST_CONTROL_OPERATION    7
#define AST_FUNCTION_CALL        8
#define AST_BLOCK                9
#define AST_PARENTHESES_BLOCK    10
#define AST_FOR                  11
#define AST_WHILE                12
#define AST_FUNCTION_DECLARATION 13
#define AST_RETURN               14
#define AST_UNARY                15
#define AST_EXPR_STATEMENT       16

typedef struct {
    int   status;
    char* description;
} StatusPair;

typedef struct {
    char* name;
    int   keyword;
} KeywordPair;

typedef struct {
    char* name;
    char* opt;
} OperatorPair;

static StatusPair STATUS_DESCRIPTIONS[] = {
    { BRAMA_OK                             , "Ok" },
    { BRAMA_NOK                            , "Not Ok" },
    { BRAMA_MISSING_TEXT_DELIMITER         , "Text delimiter missing" },
    { BRAMA_MULTIPLE_DOT_ON_DOUBLE         , "Multiple dot used on double" },
    { BRAMA_PARSE_ERROR                    , "Parse error" },
    { BRAMA_PARSE_ARRAY_INIT_NOT_PRIMATIVE , "Array init not primative" },
    { BRAMA_EXPRESSION_NOT_VALID           , "Expression not valid" }
};

static OperatorPair OPERATORS[] =  {
    { "NONE",                    "-"   },
    { "ADDITION",                "+"   },
    { "SUBTRACTION",             "-"   },
    { "MULTIPLICATION",          "*"   },
    { "DIVISION",                "/"   },
    { "MODULES",                 "%"   },
    { "INCREMENT",               "++"  },
    { "DECCREMENT",              "--"  },
    { "ASSIGN",                  "=="  },
    { "ASSIGN_ADDITION",         "+="  },
    { "ASSIGN_SUBTRACTION",      "-="  },
    { "ASSIGN_MULTIPLICATION",   "*="  },
    { "ASSIGN_DIVISION",         "/="  },
    { "ASSIGN_MODULUS",          "%="  },
    { "EQUAL",                   "=="  },
    { "EQUAL_VALUE",             "===" },
    { "NOT_EQUAL",               "!="  },
    { "NOT_EQUAL_VALUE",         "!==" },
    { "NOT",                     "!"   },
    { "AND",                     "&&"  },
    { "OR",                      "||"  },
    { "BITWISE_AND",             "&"   },
    { "BITWISE_OR",              "|"   },
    { "BITWISE_NOT",             "~"   },
    { "BITWISE_XOR",             "^"   },
    { "BITWISE_LEFT_SHIFT",      ">>"  },
    { "BITWISE_RIGHT_SHIFT",     "<<"  },
    { "GREATER_THAN",            ">"   },
    { "LESS_THAN",               "<"   },
    { "GREATER_EQUAL_THAN",      ">="  },
    { "LESS_EQUAL_THAN",         "<="  },
    { "QUESTION_MARK",           "?"   },
    { "COLON_MARK",              ":"   },
    { "BITWISE_AND_ASSIGN",      "&="  },
    { "BITWISE_OR_ASSIGN",       "|="  },
    { "BITWISE_XOR_ASSIGN",      "^="  },
    { "LEFT_PARENTHESES",        "("   },
    { "RIGHT_PARENTHESES",       ")"   },
    { "SQUARE_BRACKET_START",    "["   },
    { "SQUARE_BRACKET_END",      "]"   },
    { "COMMA",                   ","   },
    { "SEMICOLON",               ";"   },
    { "DOT",                     "."   },
    { "COMMENT_LINE",            "//"  },
    { "COMMENT_MULTILINE_START", "/*"  },
    { "COMMENT_MULTILINE_END",   "*/"  },
    { "CURVE_BRACKET_START",     "{"   },
    { "CURVE_BRACKET_END",       "}"   }
};

static char* KEYWORDS[] = {
    "NONE",
    "DO",
    "IF",
    "IN",
    "FOR",
    "LET",
    "NEW",
    "TRY",
    "VAR",
    "CASE",
    "ELSE",
    "ENUM",
    "EVAL",
    "NULL",
    "THIS",
    "TRUE",
    "VOID",
    "WITH",
    "BREAK",
    "CATCH",
    "CLASS",
    "CONST",
    "FALSE",
    "SUPER",
    "THROW",
    "WHILE",
    "YIELD",
    "DELETE",
    "EXPORT",
    "IMPORT",
    "PUBLIC",
    "RETURN",
    "STATIC",
    "SWITCH",
    "TYPEOF",
    "DEFAULT",
    "EXTENDS",
    "FINALLY",
    "PACKAGE",
    "PRIVATE",
    "CONTINUE",
    "DEBUGGER",
    "FUNCTION",
    "ARGUMENTS",
    "INTERFACE",
    "PROTECTED",
    "IMPLEMENTS",
    "INSTANCEOF"
};

/* STRUCTS */
struct _t_token;
struct _t_tokinizer;
struct _t_parser;
struct _t_primative;
struct _t_unary;
struct _t_func_call;
struct _t_ast;
struct _t_context;
struct _t_binary;
struct _t_func_decl;

typedef map_t(struct _t_ast *) map_ast_t;

typedef struct _t_token {
    size_t line;
    size_t current;
    int    type;
    union {
        void*  data_ptr;
        char*  char_ptr;
        char   char_;
        int    int_;
        double double_;
    };
} t_token;

typedef struct _t_tokinizer {
    size_t    line;
    size_t    column;
    size_t    index;
    size_t    contentLength;
    char*     content;
    t_vector* tokens;
    map_int_t keywords;
} t_tokinizer;

typedef struct _t_parser {
    size_t    index;
    t_vector* asts;
} t_parser;

typedef struct _t_primative {
    int type;
    union {
        int        int_;
        double     double_;
        bool       bool_;
        char*      char_ptr;
        t_vector*  array;
        map_ast_t* dict;
    };
} t_primative;

typedef struct _t_unary {
    int            opt;
    struct _t_ast* right;
} t_unary;

typedef struct _t_binary {
    int            opt;
    struct _t_ast* right;
    struct _t_ast* left;
} t_binary;

typedef struct _t_assign {
    int            opt;
    int            def_type; /* let, var, const */
    struct _t_ast* assignment;
    char*          symbol;
} t_assign;

typedef struct _t_control {
    int            opt;
    struct _t_ast* right;
    struct _t_ast* left;
} t_control;

typedef struct _t_func_call {
    t_vector* function;
    t_vector* args;
} t_func_call;

typedef struct _t_func_decl {
    char*          name;
    t_vector*      args;
    struct _t_ast* body;
} t_func_decl;

typedef struct _t_ast {
    int type;
    union {
        t_func_call*     func_call_ptr;
        t_func_decl*     func_decl_ptr;
        t_unary*         unary_ptr;
        t_binary*        binary_ptr;
        t_control*       control_ptr;
        t_primative*     primative_ptr;
        t_assign*        assign_ptr;
        t_vector*        vector_ptr;
        struct _t_ast*   ast_ptr;
        char*            char_ptr;
        int              int_;
    };
} t_ast;

typedef struct _t_context {
    t_tokinizer*     tokinizer;
    t_parser*        parser;

    char*            error_message;
    BRAMA_STATUS status;
} t_context;


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

typedef t_tokinizer*     t_tokinizer_ptr;
typedef t_token*         t_token_ptr;
typedef t_parser*        t_parser_ptr;
typedef t_primative*     t_primative_ptr;
typedef t_unary*         t_unary_ptr;
typedef t_binary*        t_binary_ptr;
typedef t_assign*        t_assign_ptr;
typedef t_control*       t_control_ptr;
typedef t_func_call*     t_func_call_ptr;
typedef t_func_decl*     t_func_decl_ptr;
typedef t_ast*           t_ast_ptr;
typedef t_ast**          t_ast_ptr_ptr;
typedef t_context*       t_context_ptr;
typedef t_string_stream* t_string_stream_ptr;
typedef t_vector*        t_vector_ptr;
typedef char*            char_ptr;
typedef void*            void_ptr;
typedef int*             int_ptr;
typedef map_ast_t*       map_ast_t_ptr;

t_context_ptr brama_init       ();
void          brama_execute    (t_context_ptr context, char_ptr data);
void_ptr      brama_last_error (t_context_ptr context);
void          brama_dump       (t_context_ptr context);
void          brama_destroy    (t_context_ptr context);


#endif // BRAMA_H
