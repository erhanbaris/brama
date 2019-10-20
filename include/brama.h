#ifndef BRAMA_H
#define BRAMA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "map.h"
#include "vec.h"
#include "macros.h"
#include "tools.h"

//#if (__STDC_VERSION__ >= 201112L)
//#endif


/* CODES */
typedef enum brama_status  {
    BRAMA_OK                             = 0,
    BRAMA_NOK                            = 1,
    BRAMA_MISSING_TEXT_DELIMITER         = 2,
    BRAMA_MULTIPLE_DOT_ON_DOUBLE         = 3,
    BRAMA_PARSE_ERROR                    = 4,
    BRAMA_PARSE_ARRAY_INIT_NOT_PRIMATIVE = 5,
    BRAMA_EXPRESSION_NOT_VALID           = 6,
    BRAMA_DOES_NOT_MATCH_AST             = 7,
    BRAMA_OPEN_OPERATOR_NOT_FOUND        = 8,
    BRAMA_CLOSE_OPERATOR_NOT_FOUND       = 9,
    BRAMA_DICTIONARY_NOT_VALID           = 10,
    BRAMA_FUNCTION_NAME_REQUIRED         = 11,
    BRAMA_NEW_CLASS_CREATION_NOT_VALID   = 12,
    BRAMA_BODY_NOT_FOUND                 = 13,
    BRAMA_INVALID_UNARY_EXPRESSION       = 14,
    BRAMA_FUNCTION_CALL_NOT_VALID        = 15,
    BRAMA_ARRAY_NOT_VALID                = 16,
    BRAMA_BLOCK_NOT_VALID                = 17,
    BRAMA_SEMICOLON_REQUIRED             = 18,
    BRAMA_ILLEGAL_RETURN_STATEMENT       = 19,
    BRAMA_ILLEGAL_BREAK_STATEMENT        = 20,
    BRAMA_ILLEGAL_CONTINUE_STATEMENT     = 21,
    BRAMA_ILLEGAL_ACCESSOR_STATEMENT     = 22,
    BRAMA_ILLEGAL_FUNCTION_ARGUMENT      = 23

} brama_status;

/* PRIMATIVE TYPES */
typedef enum brama_primative_type {
    PRIMATIVE_NONE     = 0,
    PRIMATIVE_INTEGER    ,
    PRIMATIVE_DOUBLE     ,
    PRIMATIVE_STRING     ,
    PRIMATIVE_BOOL       ,
    PRIMATIVE_ARRAY      ,
    PRIMATIVE_DICTIONARY ,
    PRIMATIVE_NULL
} brama_primative_type;

/* TOKEN TYPES */
typedef enum brama_token_type {
    TOKEN_NONE    = 0,
    TOKEN_INTEGER    ,
    TOKEN_DOUBLE     ,
    TOKEN_SYMBOL     ,
    TOKEN_OPERATOR   ,
    TOKEN_TEXT       ,
    TOKEN_KEYWORD    ,
    TOKEN_END_OF_FILE
} brama_token_type;

/* KEYWORD TYPES */
typedef enum brama_keyword_type {
    KEYWORD_NONE     = 0,
    KEYWORD_DO         ,
    KEYWORD_IF         ,
    KEYWORD_IN         ,
    KEYWORD_FOR        ,
    KEYWORD_LET        ,
    KEYWORD_NEW        ,
    KEYWORD_TRY        ,
    KEYWORD_VAR        ,
    KEYWORD_CASE       ,
    KEYWORD_ELSE       ,
    KEYWORD_ENUM       ,
    KEYWORD_EVAL       ,
    KEYWORD_NULL       ,
    KEYWORD_THIS       ,
    KEYWORD_TRUE       ,
    KEYWORD_VOID       ,
    KEYWORD_WITH       ,
    KEYWORD_BREAK      ,
    KEYWORD_CATCH      ,
    KEYWORD_CLASS      ,
    KEYWORD_CONST      ,
    KEYWORD_FALSE      ,
    KEYWORD_SUPER      ,
    KEYWORD_THROW      ,
    KEYWORD_WHILE      ,
    KEYWORD_YIELD      ,
    KEYWORD_DELETE     ,
    KEYWORD_EXPORT     ,
    KEYWORD_IMPORT     ,
    KEYWORD_PUBLIC     ,
    KEYWORD_RETURN     ,
    KEYWORD_STATIC     ,
    KEYWORD_SWITCH     ,
    KEYWORD_TYPEOF     ,
    KEYWORD_DEFAULT    ,
    KEYWORD_EXTENDS    ,
    KEYWORD_FINALLY    ,
    KEYWORD_PACKAGE    ,
    KEYWORD_PRIVATE    ,
    KEYWORD_CONTINUE   ,
    KEYWORD_DEBUGGER   ,
    KEYWORD_FUNCTION   ,
    KEYWORD_ARGUMENTS  ,
    KEYWORD_INTERFACE  ,
    KEYWORD_PROTECTED  ,
    KEYWORD_IMPLEMENTS ,
    KEYWORD_INSTANCEOF
} brama_keyword_type;

/* OPERATOR TYPES */
typedef enum brama_operator_type {
    OPERATOR_NONE                  = 0,
    OPERATOR_ADDITION                ,
    OPERATOR_SUBTRACTION             ,
    OPERATOR_MULTIPLICATION          ,
    OPERATOR_DIVISION                ,
    OPERATOR_MODULES                 ,
    OPERATOR_INCREMENT               ,
    OPERATOR_DECCREMENT              ,
    OPERATOR_ASSIGN                  ,
    OPERATOR_ASSIGN_ADDITION         ,
    OPERATOR_ASSIGN_SUBTRACTION      ,
    OPERATOR_ASSIGN_MULTIPLICATION   ,
    OPERATOR_ASSIGN_DIVISION         ,
    OPERATOR_ASSIGN_MODULUS          ,
    OPERATOR_EQUAL                   ,
    OPERATOR_EQUAL_VALUE             ,
    OPERATOR_NOT_EQUAL               ,
    OPERATOR_NOT_EQUAL_VALUE         ,
    OPERATOR_NOT                     ,
    OPERATOR_AND                     ,
    OPERATOR_OR                      ,
    OPERATOR_BITWISE_AND             ,
    OPERATOR_BITWISE_OR              ,
    OPERATOR_BITWISE_NOT             ,
    OPERATOR_BITWISE_XOR             ,
    OPERATOR_BITWISE_LEFT_SHIFT      ,
    OPERATOR_BITWISE_RIGHT_SHIFT     ,
    OPERATOR_BITWISE_UNSIGNED_RIGHT_SHIFT,
    OPERATOR_GREATER_THAN            ,
    OPERATOR_LESS_THAN               ,
    OPERATOR_GREATER_EQUAL_THAN      ,
    OPERATOR_LESS_EQUAL_THAN         ,
    OPERATOR_QUESTION_MARK           ,
    OPERATOR_COLON_MARK              ,
    OPERATOR_BITWISE_AND_ASSIGN      ,
    OPERATOR_BITWISE_OR_ASSIGN       ,
    OPERATOR_BITWISE_XOR_ASSIGN      ,
    OPERATOR_LEFT_PARENTHESES        ,
    OPERATOR_RIGHT_PARENTHESES       ,
    OPERATOR_SQUARE_BRACKET_START    ,
    OPERATOR_SQUARE_BRACKET_END      ,
    OPERATOR_COMMA                   ,
    OPERATOR_SEMICOLON               ,
    OPERATOR_DOT                     ,
    OPERATOR_COMMENT_LINE            ,
    OPERATOR_COMMENT_MULTILINE_START ,
    OPERATOR_COMMENT_MULTILINE_END   ,
    OPERATOR_CURVE_BRACKET_START     ,
    OPERATOR_CURVE_BRACKET_END       ,
    OPERATOR_NEW_LINE
} brama_operator_type;

/* AST TYPES */
typedef enum brama_ast_type {
    AST_NONE                 = 0,
    AST_IF_STATEMENT         = 1,
    AST_ASSIGNMENT           = 2,
    AST_SYMBOL               = 3,
    AST_PRIMATIVE            = 4,
    AST_BINARY_OPERATION     = 5,
    AST_STRUCT_OPERATION     = 6,
    AST_CONTROL_OPERATION    = 7,
    AST_FUNCTION_CALL        = 8,
    AST_BLOCK                = 9,
    AST_PARENTHESES_BLOCK    = 10,
    AST_FOR                  = 11,
    AST_WHILE                = 12,
    AST_FUNCTION_DECLARATION = 13,
    AST_RETURN               = 14,
    AST_UNARY                = 15,
    AST_EXPR_STATEMENT       = 16,
    AST_OBJECT_CREATION      = 17,
    AST_SWITCH               = 18,
    AST_BREAK                = 19,
    AST_CONTINUE             = 20,
    AST_ACCESSOR             = 21,
    AST_KEYWORD              = 22
} brama_ast_type;

/* VM Operators */
enum brama_vm_operator {
    VM_OPT_HALT,
    VM_OPT_ADDITION                ,
    VM_OPT_SUBTRACTION             ,
    VM_OPT_MULTIPLICATION          ,
    VM_OPT_DIVISION                ,
    VM_OPT_MODULES                 ,
    VM_OPT_BITWISE_AND             ,
    VM_OPT_BITWISE_OR              ,
    VM_OPT_BITWISE_NOT             ,
    VM_OPT_BITWISE_XOR             ,
    VM_OPT_BITWISE_LEFT_SHIFT      ,
    VM_OPT_BITWISE_RIGHT_SHIFT     ,
    VM_OPT_BITWISE_UNSIGNED_RIGHT_SHIFT,
    VM_OPT_EQ,
    VM_OPT_LT,
    VM_OPT_LTE,
    VM_OPT_GT,
    VM_OPT_GTE,
    VM_OPT_AND,
    VM_OPT_OR,
    VM_OPT_DUP,
    VM_OPT_POP,
    VM_OPT_CONST_STR,
    VM_OPT_CONST_INT,
    VM_OPT_CONST_BOOL,
    VM_OPT_CONST_DOUBLE,
    VM_OPT_NULL,
    VM_OPT_UNDEFINED,
    VM_OPT_DELETE,
    VM_OPT_JMP,
    VM_OPT_IF_EQ,
    VM_OPT_JIF,
    VM_OPT_JNIF,
    VM_OPT_INC,
    VM_OPT_DINC,
    VM_OPT_LOAD,
    VM_OPT_LOAD_0,
    VM_OPT_LOAD_1,
    VM_OPT_LOAD_2,
    VM_OPT_LOAD_3,
    VM_OPT_STORE,
    VM_OPT_STORE_0,
    VM_OPT_STORE_1,
    VM_OPT_STORE_2,
    VM_OPT_STORE_3,
    VM_OPT_GLOAD,
    VM_OPT_GLOAD_0,
    VM_OPT_GLOAD_1,
    VM_OPT_GLOAD_2,
    VM_OPT_GLOAD_3,
    VM_OPT_GSTORE,
    VM_OPT_GSTORE_0,
    VM_OPT_GSTORE_1,
    VM_OPT_GSTORE_2,
    VM_OPT_GSTORE_3,
    VM_OPT_CALL,
    VM_OPT_RETURN,
    VM_OPT_PUSH,
    VM_OPT_PRINT,
    VM_OPT_NEG,
    VM_OPT_CALL_NATIVE,
    VM_OPT_METHOD_DEF,
    VM_OPT_INITARRAY,
    VM_OPT_INITDICT,
    VM_OPT_NOT_EQ,
    VM_OPT_APPEND
};

typedef enum _brama_ast_extra_data_type {
    AST_IN_NONE     = 0,
    AST_IN_FUNCTION = 1 << 0,
    AST_IN_LOOP     = 1 << 1,
    AST_IN_SWITCH   = 1 << 2
} brama_ast_extra_data_type;

/* Unary Operand Type */
typedef enum _brama_unary_operant_type {
    UNARY_OPERAND_BEFORE = 0,
    UNARY_OPERAND_AFTER  = 1
} brama_unary_operant_type;

/* Function Call Type */
typedef enum _brama_func_call_type {
    FUNC_CALL_NORMAL = 0,
    FUNC_CALL_ANONY  = 1
} brama_func_call_type;

/* End of line checker */
typedef enum _brama_end_line_checker_type {
    END_LINE_CHECKER_SEMICOLON = 1 << 0,
    END_LINE_CHECKER_NEWLINE   = 1 << 1
} brama_end_line_checker_type;

/* Function Definition Type */

typedef enum func_def_type {
    FUNC_DEF_NORMAL     = (1<<0),
    FUNC_DEF_ASSIGNMENT = (1<<1)
} func_def_type;

typedef struct {
    brama_status status;
    char*        description;
} StatusPair;

typedef struct {
    char*              name;
    brama_keyword_type keyword;
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
    { "NEW_LINE",                "\n"  },
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
typedef struct _t_token t_token;
typedef struct _t_tokinizer t_tokinizer;
typedef struct _t_parser t_parser;
typedef struct _t_primative t_primative;
typedef struct _t_unary t_unary;
typedef struct _t_func_call t_func_call;
typedef struct _t_ast t_ast;
typedef struct _t_context t_context;
typedef struct _t_binary t_binary;
typedef struct _t_func_decl t_func_decl;
typedef struct _t_object_creation t_object_creation;
typedef struct _t_while_loop t_while_loop;
typedef struct _t_if_stmt t_if_stmt;
typedef struct _t_accessor t_accessor;
typedef struct _t_compiler t_compiler;
typedef struct _t_brama_vmdata t_brama_vmdata;
typedef struct _t_tokinizer t_tokinizer;
typedef struct _t_assign t_assign;
typedef struct _t_control t_control;


typedef t_tokinizer*       t_tokinizer_ptr;
typedef t_token*           t_token_ptr;
typedef t_parser*          t_parser_ptr;
typedef t_primative*       t_primative_ptr;
typedef t_unary*           t_unary_ptr;
typedef t_binary*          t_binary_ptr;
typedef t_assign*          t_assign_ptr;
typedef t_control*         t_control_ptr;
typedef t_func_call*       t_func_call_ptr;
typedef t_func_decl*       t_func_decl_ptr;
typedef t_ast*             t_ast_ptr;
typedef t_ast**            t_ast_ptr_ptr;
typedef t_context*         t_context_ptr;
typedef t_string_stream*   t_string_stream_ptr;
typedef t_object_creation* t_object_creation_ptr;
typedef t_while_loop*      t_while_loop_ptr;
typedef t_if_stmt*         t_if_stmt_ptr;
typedef t_accessor*        t_accessor_ptr;
typedef t_compiler*        t_compiler_ptr;
typedef char*              char_ptr;
typedef void*              void_ptr;
typedef int*               int_ptr;

typedef uint8_t            t_brama_opcode;
typedef int8_t             t_brama_byte;
typedef int8_t             t_brama_char;
typedef union              _t_brama_double { t_brama_byte bytes[8];  double double_; } t_brama_double;
typedef union              _t_brama_int    { t_brama_byte bytes[4];  int int_; } t_brama_int;
typedef bool               t_brama_bool;

typedef map_t(struct _t_ast *) map_ast_t;
typedef map_ast_t*             map_ast_t_ptr;
typedef t_brama_vmdata*        t_brama_vmdata_ptr;
typedef vec_t(t_token_ptr)     vec_t_token_ptr_t;
typedef vec_t(t_token_ptr)*    vec_t_token_ptr_t_ptr;
typedef vec_t(t_ast_ptr)       vec_t_ast_ptr_t;
typedef vec_t(t_ast_ptr)*      vec_t_ast_ptr_t_ptr;
typedef vec_t(t_brama_opcode)  vec_t_brama_opcode_t;
typedef vec_t(t_brama_opcode)* vec_t_brama_opcode_t_ptr;

typedef struct _t_token {
    size_t           line;
    size_t           current;
    brama_token_type type;
    union {
        char*  char_ptr;
        char   char_;
        int    int_;
        double double_;
        brama_keyword_type  keyword;
        brama_operator_type opt;
    };
} t_token;

typedef struct _t_tokinizer {
    size_t    line;
    size_t    column;
    size_t    index;
    size_t    contentLength;
    char*     content;
    vec_t(t_token*)* tokens;
    map_int_t keywords;
} t_tokinizer;

typedef struct _t_parser {
    size_t              index;
    size_t              line;
    vec_t_ast_ptr_t_ptr asts;
} t_parser;

typedef struct _t_compiler {
    size_t          index;
    vec_t(uint8_t)* op_codes;
} t_compiler;

typedef struct _t_primative {
    brama_primative_type type;
    union {
        int        int_;
        double     double_;
        bool       bool_;
        char*      char_ptr;
        vec_t(struct _t_ast*)*  array;
        map_ast_t* dict;
    };
} t_primative;

typedef struct _t_unary {
    brama_operator_type      opt;
    brama_unary_operant_type operand_type;
    struct _t_ast*           content;
} t_unary;

typedef struct _t_binary {
    brama_operator_type opt;
    struct _t_ast*      right;
    struct _t_ast*      left;
} t_binary;

typedef struct _t_assign {
    brama_operator_type opt;
    brama_keyword_type  def_type;
    bool                new_def;
    struct _t_ast*      assignment;
    struct _t_ast*      object;
} t_assign;

typedef struct _t_control {
    brama_operator_type opt;
    struct _t_ast*      right;
    struct _t_ast*      left;
} t_control;

typedef struct _t_func_call {
    union {
        struct _t_ast*       function;
        struct _t_func_decl* func_decl_ptr;
    };
    brama_func_call_type     type;
    vec_t(struct _t_ast*)*   args;
} t_func_call;

typedef struct _t_func_decl {
    char*          name;
    vec_t(struct _t_ast*)* args;
    struct _t_ast* body;
} t_func_decl;

typedef struct _t_object_creation {
    char*     object_name;
    vec_t(struct _t_ast*)* args;
} t_object_creation;

typedef struct _t_accessor{
    struct _t_ast* object;
    struct _t_ast* property;
} t_accessor;

typedef struct _t_while_loop {
    t_ast* condition;
    t_ast* body;
} t_while_loop;

typedef struct _t_if_stmt {
    t_ast* condition;
    t_ast* true_body;
    t_ast* false_body;
} t_if_stmt;

typedef struct _t_ast {
    brama_ast_type type;
    union {
        t_func_call*           func_call_ptr;
        t_func_decl*           func_decl_ptr;
        t_unary*               unary_ptr;
        t_binary*              binary_ptr;
        t_control*             control_ptr;
        t_primative*           primative_ptr;
        t_assign*              assign_ptr;
        vec_t(struct _t_ast*)* vector_ptr;
        t_object_creation*     object_creation_ptr;
        t_while_loop*          while_ptr;
        t_if_stmt*             if_stmt_ptr;
        t_accessor*            accessor_ptr;
        struct _t_ast*         ast_ptr;
        char*                  char_ptr;
        int                    int_;
        brama_keyword_type     keyword;
    };
} t_ast;

typedef struct _t_context {
    t_tokinizer* tokinizer;
    t_parser*    parser;
    t_compiler*  compiler;

    char*        error_message;
    brama_status status;
} t_context;

typedef struct _t_brama_vmdata {
    union {
        int reg1;
        int op;
    };
    int reg2;
    int reg3;
    int scal;
} t_brama_vmdata;

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

t_context_ptr brama_init       ();
void          brama_execute    (t_context_ptr context, char_ptr data);
void_ptr      brama_last_error (t_context_ptr context);
void          brama_dump       (t_context_ptr context);
void          brama_dump_ast   (t_context_ptr context);
void          brama_destroy    (t_context_ptr context);


#endif // BRAMA_H
