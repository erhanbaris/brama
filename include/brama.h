#ifndef BRAMA_H
#define BRAMA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

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
    BRAMA_ILLEGAL_FUNCTION_ARGUMENT      = 23,
    BRAMA_CONSTANT_NOT_FOUND             = 24,
    BRAMA_CASE_KEYWORD_NOT_FOUND         = 25,
    BRAMA_SWITCH_NOT_VALID               = 26,
    BRAMA_DEFAULT_CASE_USED              = 27,
    BRAMA_AST_NOT_COMPILED               = 28,
    BRAMA_FUNCTION_NOT_FOUND             = 29

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
    PRIMATIVE_NULL       ,
    PRIMATIVE_UNDEFINED
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
    KEYWORD_INSTANCEOF ,
    KEYWORD_UNDEFINED
} brama_keyword_type;

/* OPERATOR TYPES */
typedef enum brama_operator_type {
    OPERATOR_NONE                  = 0,
    OPERATOR_ADDITION                ,
    OPERATOR_SUBTRACTION             ,
    OPERATOR_MULTIPLICATION          ,
    OPERATOR_DIVISION                ,
    OPERATOR_MODULO                  ,
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

/* COMPILE BLOCK TYPES */
typedef enum brama_compile_block_type {
    COMPILE_BLOCK_NONE        = 0,
    COMPILE_BLOCK_WHILE       = 1,
    COMPILE_BLOCK_SWITCH      = 2,
    COMPILE_BLOCK_SWITCH_CASE = 3,
    COMPILE_BLOCK_FUNC_DECL   = 4,
} brama_compile_block_type;

/* VM Operators */
enum brama_vm_operator {
    VM_OPT_HALT = 0,
    VM_OPT_ADDITION  = 1,
    VM_OPT_SUBTRACTION  = 2,
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
    VM_OPT_EQV,
    VM_OPT_NEQ,
    VM_OPT_NEQV,
    VM_OPT_LT,
    VM_OPT_LTE,
    VM_OPT_GT,
    VM_OPT_GTE,
    VM_OPT_AND,
    VM_OPT_OR,
    VM_OPT_NULL,
    VM_OPT_UNDEFINED,
    VM_OPT_DELETE,
    VM_OPT_JMP,
    VM_OPT_IF,
    VM_OPT_INC,
    VM_OPT_DINC,
    VM_OPT_INIT_VAR,
    VM_OPT_CALL,
    VM_OPT_RETURN,
    VM_OPT_PRINT,
    VM_OPT_CALL_NATIVE,
    VM_OPT_METHOD_DEF,
    VM_OPT_INITARRAY,
    VM_OPT_INITDICT,
    VM_OPT_NOT_EQ,
    VM_OPT_APPEND,
    VM_OPT_LOOP,
    VM_OPT_COPY,
    VM_OPT_NOT,
    VM_OPT_CASE,
    VM_OPT_FUNC,
    VM_OPT_STORAGE_ID,
};

/* VM CONST TYPE */
typedef enum _brama_vm_const_type {
    CONST_NULL      = 0,
    CONST_UNDEFINED = 1,
    CONST_INTEGER   = 2,
    CONST_DOUBLE    = 3,
    CONST_STRING    = 4,
    CONST_BOOL      = 5,
    CONST_FUNCTION  = 6
} brama_vm_const_type;

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

typedef enum _memory_prototype_item_type {
    MEMORY_PROTOTYPE_CONST,
    MEMORY_PROTOTYPE_VARIABLE,
    MEMORY_PROTOTYPE_TEMPORARY,
    MEMORY_PROTOTYPE_RETURN_VAR,
    MEMORY_PROTOTYPE_TOTAL_ARGS,
    MEMORY_PROTOTYPE_FUNCTION_ARG,
} memory_prototype_item_type;

static memory_prototype_item_type MEMORY_SORT_INFO[] = {
    MEMORY_PROTOTYPE_CONST,
    MEMORY_PROTOTYPE_RETURN_VAR,
    MEMORY_PROTOTYPE_TOTAL_ARGS,
    MEMORY_PROTOTYPE_VARIABLE,
    MEMORY_PROTOTYPE_FUNCTION_ARG,
    MEMORY_PROTOTYPE_TEMPORARY
};

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

static OperatorPair VM_OPCODES[] =  {
        { "HALT", ""},
        { "ADD", "+"},
        { "SUB", "-"},
        { "MUL", "*"},
        { "DIV", "/"},
        { "MOD", "%"},
        { "BAND", "&"},
        { "BOR", "|"},
        { "BNOT", "!"},
        { "BXOR", "^"},
        { "SHIFTL", "<<"},
        { "SHIFTR", ">>"},
        { "SHIFTUR", ">>>"},
        { "EQ", "=="},
        { "EQV", "==="},
        { "NEQ", "!="},
        { "NEQV", "!=="},
        { "LT", "<"},
        { "LTE", "<="},
        { "GT", ">"},
        { "GTE", ">="},
        { "AND", "&&"},
        { "OR", "||"},
        { "NULL", "null"},
        { "UNDEFINED", "undefined"},
        { "DELETE", ""},
        { "JMP", ""},
        { "IF", ""},
        { "INC", "++"},
        { "DINC", "--"},
        { "INIT_VAR", ""},
        { "CALL", ""},
        { "RETURN", ""},
        { "PRINT", ""},
        { "CALL_NATIVE", ""},
        { "METHOD_DEF", ""},
        { "INITARRAY", ""},
        { "INITDICT", ""},
        { "NOT_EQ", "!="},
        { "APPEND", ""},
        { "LOOP", ""},
        { "COPY", ""},
        { "NOT", "!"},
        { "CASE", ""},
        { "FUNC", ""},
        { "STORAGE_ID", ""}
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
    "INSTANCEOF",
    "UNDEFINED"
};

/* STRUCTS */
typedef struct _t_token           t_token;
typedef struct _t_tokinizer       t_tokinizer;
typedef struct _t_parser          t_parser;
typedef struct _t_primative       t_primative;
typedef struct _t_unary           t_unary;
typedef struct _t_func_call       t_func_call;
typedef struct _t_ast             t_ast;
typedef struct _t_context         t_context;
typedef struct _t_binary          t_binary;
typedef struct _t_func_decl       t_func_decl;
typedef struct _t_object_creation t_object_creation;
typedef struct _t_while_loop      t_while_loop;
typedef struct _t_if_stmt         t_if_stmt;
typedef struct _t_switch_stmt     t_switch_stmt;
typedef struct _t_case_item       t_case_item;
typedef struct _t_accessor        t_accessor;
typedef struct _t_compiler        t_compiler;
typedef struct _t_brama_vmdata    t_brama_vmdata;
typedef struct _t_tokinizer       t_tokinizer;
typedef struct _t_assign          t_assign;
typedef struct _t_control         t_control;
typedef struct _t_vm_object       t_vm_object;
typedef struct _t_storage         t_storage;
typedef struct _t_compile_info    t_compile_info;
typedef struct _t_get_var_info    t_get_var_info;
typedef struct _t_compile_stack   t_compile_stack;
typedef struct _t_compile_while   t_compile_while;
typedef struct _t_compile_switch  t_compile_switch;
typedef struct _t_compile_switch_case  t_compile_switch_case;
typedef struct _t_compile_func_decl  t_compile_func_decl;
typedef struct _t_function_referance t_function_referance;
typedef struct _t_brama_link         t_brama_link;
typedef struct _t_memory_prototype_item t_memory_prototype_item;

typedef t_vm_object*       t_vm_object_ptr;
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
typedef t_switch_stmt*     t_switch_stmt_ptr;
typedef t_case_item*       t_case_item_ptr;
typedef t_accessor*        t_accessor_ptr;
typedef t_compiler*        t_compiler_ptr;
typedef t_storage*         t_storage_ptr;
typedef t_compile_info*    t_compile_info_ptr;
typedef t_get_var_info*    t_get_var_info_ptr;
typedef t_compile_while*   t_compile_while_ptr;
typedef t_compile_switch*  t_compile_switch_ptr;
typedef t_compile_switch_case*  t_compile_switch_case_ptr;
typedef t_compile_func_decl*    t_compile_func_decl_ptr;
typedef t_function_referance*   t_function_referance_ptr;
typedef t_brama_link*           t_brama_link_ptr;
typedef t_memory_prototype_item* t_memory_prototype_item_ptr;
typedef char*              char_ptr;
typedef void*              void_ptr;
typedef int*               int_ptr;
typedef double*            double_ptr;
typedef t_compile_stack*   t_compile_stack_ptr;

enum brama_vm_operator;

typedef uint64_t           t_brama_value;
typedef uint32_t           t_brama_opcode;
typedef int32_t            t_brama_byte;
typedef int8_t             t_brama_char;
typedef union              _t_brama_double { t_brama_byte bytes[8];  double double_; } t_brama_double;
typedef union              _t_brama_int    { t_brama_byte bytes[4];  int int_; } t_brama_int;
typedef bool               t_brama_bool;
typedef void(*brama_function_callback)(t_context_ptr context, size_t param_size, t_brama_value* params);

typedef map_t(struct _t_ast *)  map_ast_t;
typedef map_ast_t*              map_ast_t_ptr;
typedef map_t(t_brama_value)    map_value;
typedef map_value*              map_value_ptr;
typedef vec_t(t_brama_value)    vec_value;
typedef vec_value*              vec_value_ptr;
typedef map_t(size_t)           map_size_t;
typedef map_size_t*             map_size_t_ptr;
typedef map_t(t_function_referance_ptr) map_function_referance;
typedef map_function_referance* map_function_referance_ptr;
typedef vec_t(t_storage*)       vec_storage;
typedef vec_storage*            vec_storage_ptr;
typedef t_brama_vmdata*         t_brama_vmdata_ptr;
typedef vec_t(t_token_ptr)      vec_token;
typedef vec_t(t_token_ptr)*     vec_token_ptr;
typedef vec_t(t_case_item_ptr)  vec_case_item;
typedef vec_t(t_case_item_ptr)* vec_case_item_ptr;
typedef vec_t(t_ast_ptr)        vec_ast;
typedef vec_t(t_ast_ptr)*       vec_ast_ptr;
typedef vec_t(t_brama_opcode)   vec_opcode;
typedef vec_t(t_brama_opcode)*  vec_opcode_ptr;
typedef vec_t(t_brama_byte)     vec_byte;
typedef vec_t(t_brama_byte)*    vec_byte_ptr;
typedef vec_t(char_ptr)         vec_string;
typedef vec_t(char_ptr)*        vec_string_ptr;
typedef vec_t(t_compile_stack*) vec_compile_stack;
typedef vec_t(t_brama_link_ptr) vec_link;
typedef vec_compile_stack*      vec_compile_stack_ptr;

typedef struct _t_token {
    size_t           line;
    size_t           current;
    brama_token_type type;
    union {
        char_ptr            char_ptr;
        double              double_;
        brama_keyword_type  keyword;
        brama_operator_type opt;
    };
} t_token;

typedef struct _t_tokinizer {
    size_t    line;
    size_t    column;
    size_t    index;
    size_t    contentLength;
    char_ptr  content;
    vec_t(t_token*)* tokens;
    map_int_t keywords;
} t_tokinizer;

typedef struct _t_parser {
    size_t              index;
    size_t              line;
    vec_ast_ptr asts;
} t_parser;

typedef struct _t_compiler {
    vec_compile_stack compile_stack;
    vec_byte_ptr      op_codes;
    t_storage_ptr     global_storage;
    vec_storage       storages;
    t_vm_object_ptr   object_head;
    vec_link          links;
    size_t            total_object;
    size_t            storage_index;
} t_compiler;

typedef struct _t_storage {
    size_t        id;

    /* Totals */
    size_t        temp_count;
    size_t        constant_count;
    size_t        variable_count;

    /* Counters */
    size_t        loop_counter;
    size_t        temp_counter;    

    map_function_referance      functions;
    vec_value                   variables;
    map_size_t                  variable_names;
    t_storage_ptr               previous_storage;
    t_memory_prototype_item_ptr memory_prototype_head;
} t_storage;

typedef struct _t_primative {
    brama_primative_type type;
    union {
        int         int_;
        double      double_;
        bool        bool_;
        char*       char_ptr;
        vec_ast_ptr array;
        map_ast_t*  dict;
    };
} t_primative;

typedef struct _t_unary {
    brama_operator_type      opt;
    brama_unary_operant_type operand_type;
    t_ast_ptr                content;
} t_unary;

typedef struct _t_binary {
    brama_operator_type opt;
    t_ast_ptr           right;
    t_ast_ptr           left;
} t_binary;

typedef struct _t_assign {
    brama_operator_type opt;
    brama_keyword_type  def_type;
    bool                new_def;
    t_ast_ptr           assignment;
    t_ast_ptr           object;
} t_assign;

typedef struct _t_control {
    brama_operator_type opt;
    t_ast_ptr           right;
    t_ast_ptr           left;
} t_control;

typedef struct _t_func_call {
    union {
        t_ast_ptr       function;
        t_func_decl_ptr func_decl_ptr;
    };
    brama_func_call_type type;
    vec_ast_ptr          args;
} t_func_call;

typedef struct _t_func_decl {
    char_ptr    name;
    vec_ast_ptr args;
    t_ast_ptr   body;
} t_func_decl;

typedef struct _t_object_creation {
    char_ptr    object_name;
    vec_ast_ptr args;
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

typedef struct _t_case_item {
    t_ast* key;
    t_ast* body;
} t_case_item;

typedef struct _t_switch_stmt {
    t_ast*            condition;
    vec_case_item_ptr cases;
} t_switch_stmt;

typedef struct _t_ast {
    brama_ast_type type;
    bool           ends_with_newline;
    bool           ends_with_semicolon;
    bool create_new_storage;

    union {
        t_func_call*           func_call_ptr;
        t_func_decl*           func_decl_ptr;
        t_unary*               unary_ptr;
        t_binary*              binary_ptr;
        t_control*             control_ptr;
        t_primative*           primative_ptr;
        t_assign*              assign_ptr;
        vec_ast_ptr            vector_ptr;
        t_object_creation*     object_creation_ptr;
        t_while_loop*          while_ptr;
        t_if_stmt*             if_stmt_ptr;
        t_accessor*            accessor_ptr;
        t_switch_stmt*         switch_stmt_ptr;
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
    size_t       error_location_on_code;
} t_context;

typedef struct _t_brama_vmdata {
    enum brama_vm_operator op;
    uint8_t  reg1;
    uint8_t  reg2;
    uint8_t  reg3;
    int16_t scal;
} t_brama_vmdata;


typedef struct _t_vm_object {
    brama_vm_const_type type;
    bool                marked;
    t_vm_object_ptr     next;
    union {
        char_ptr                 char_ptr;
        t_function_referance_ptr function;
    };
} t_vm_object;

typedef struct _t_compile_info {
    int                index;
    t_brama_vmdata_ptr post_opcode;
} t_compile_info;

typedef struct _t_get_var_info {
    brama_vm_const_type type;
    union {
        double     double_;
        bool       bool_;
        char_ptr   char_ptr;
    };
} t_get_var_info;

typedef struct _t_compile_stack {
    size_t         start_address;
    size_t         end_address;
    brama_compile_block_type compile_stack_type;
    void*          ast;
    union
    {
        void*                     compile_obj;
        t_compile_while_ptr       while_ptr;
        t_compile_switch_ptr      switch_ptr;
        t_compile_switch_case_ptr switch_case_ptr;
        t_compile_func_decl_ptr   func_decl_ptr;
    };
} t_compile_stack;

typedef struct _t_compile_block {
    vec_int_t breaks;
    vec_int_t continues;
} t_compile_block;

typedef struct _t_compile_while {
    vec_int_t breaks;
    vec_int_t continues;
} t_compile_while;

typedef struct _t_compile_switch {
    vec_int_t breaks;
} t_compile_switch;

typedef struct _t_compile_switch_case {
    size_t    index;
    vec_int_t breaks;
} t_compile_switch_case;

typedef struct _t_compile_func_decl {
    vec_int_t returns;
} t_compile_func_decl;

typedef struct _t_function_referance {
    size_t    location;
    char_ptr  name;
    uint64_t  hash;
    char_ptr* args;
    size_t    args_length;
    size_t    storage_id;
    bool      is_native;
} t_function_referance;

typedef struct _t_brama_native_function {
    char_ptr                _class;
    char_ptr                function;
    brama_function_callback callback;
} t_brama_native_function;

typedef struct _t_brama_link {
    t_brama_value* source;
    t_brama_value* destination;
} t_brama_link;

typedef struct _t_memory_prototype_item {
    t_brama_value               value;
    char_ptr                    name;
    memory_prototype_item_type  type;
    t_memory_prototype_item_ptr next;
} t_memory_prototype_item;

/* VM Defs */

// A mask that selects the sign bit.
#define SIGN_BIT ((uint64_t)1 << 63)

// The bits that must be set to indicate a quiet NaN.
#define QNAN ((uint64_t)0x7ffc000000000000)

// If the NaN bits are set, it's not a number.
#define IS_NUM(value) (((value) & QNAN) != QNAN)

// An object pointer is a NaN with a set sign bit.
#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define IS_STRING(value)   (IS_OBJ(value) && AS_OBJ(value)->type == CONST_STRING)
#define IS_FUNCTION(value) (IS_OBJ(value) && AS_OBJ(value)->type == CONST_FUNCTION)

#define IS_FALSE(value)     ((value) == FALSE_VAL)
#define IS_TRUE(value)      ((value) == TRUE_VAL)
#define IS_BOOL(value)      (value == TRUE_VAL || value == FALSE_VAL)
#define IS_NULL(value)      ((value) == NULL_VAL)
#define IS_UNDEFINED(value) ((value) == UNDEFINED_VAL)

// Masks out the tag bits used to identify the singleton value.
#define MASK_TAG (7)

// Tag values for the different singleton values.
#define TAG_NAN       (0)
#define TAG_NULL      (1)
#define TAG_FALSE     (2)
#define TAG_TRUE      (3)
#define TAG_UNDEFINED (4)
#define TAG_UNUSED2   (5)
#define TAG_UNUSED3   (6)
#define TAG_HALT      (7)

// Value -> 0 or 1.
#define AS_BOOL(value) ((value) == TRUE_VAL)

// Value -> Obj*.
#define AS_OBJ(value) ((t_vm_object*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))
#define AS_STRING(value) ((t_vm_object*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))->char_ptr
#define AS_FUNCTION(value) ((t_vm_object*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))->function

// Singleton values.
#define NULL_VAL      ((t_brama_value)(uint64_t)(QNAN | TAG_NULL))
#define FALSE_VAL     ((t_brama_value)(uint64_t)(QNAN | TAG_FALSE))
#define TRUE_VAL      ((t_brama_value)(uint64_t)(QNAN | TAG_TRUE))
#define UNDEFINED_VAL ((t_brama_value)(uint64_t)(QNAN | TAG_UNDEFINED))
#define HALT_VAL      ((t_brama_value)(uint64_t)(QNAN | TAG_HALT))

// Gets the singleton type tag for a Value (which must be a singleton).
#define GET_TAG(value) ((int)((t_brama_value) & MASK_TAG))

#define GET_VALUE_FROM_OBJ(obj) ((t_brama_value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj)))


/*

         STORAGE STRUCTURE
 *--------------------------------*
 * RETURN BACK ADDRESS (OPCODE)   *
 *--------------------------------*
 * RETURN BACK VARIABLES          *
 *--------------------------------* --------
 *     CONSTANT 1                 *         |
 *--------------------------------*         |
 *     CONSTANT 2                 *    CONSTANT DEFINITIONS
 *--------------------------------*         |
 *     CONSTANT 3                 *         |
 *--------------------------------*---------
 *     TEMPORARY 1                *         |
 *--------------------------------*         |
 *     TEMPORARY 2                *    TEMPORARY VARIABLES
 *--------------------------------*         |
 *     TEMPORARY 3                *         |
 *--------------------------------*---------
 *     VARIABLE 1                 *         |
 *--------------------------------*         |
 *     VARIABLE 2                 *    NORMAL VARIABLES
 *--------------------------------*         |
 *     VARIABLE 3                 *         |
 *--------------------------------*---------



 OPCODE STRUCTURE
-------------------------------------------------------------------------------
 1 1 1 1   1 1 1 1   1 1 1 1   1 1 1 1   1 1 1 1   1 1 1 1   1 1 1 1   1 1 1 1
-------------------------------------------------------------------------------
|   OP CODE        |   REG1            |    REG2           |      REG3        |
-------------------------------------------------------------------------------
|   OP CODE        |   REG1            |                SCAL                  |
-------------------------------------------------------------------------------
*/

#define OP_MASK   0xFF000000
#define REG1_MASK 0x00FF0000
#define REG2_MASK 0x0000FF00
#define REG3_MASK 0x000000FF
#define SCAL_MASK 0x0000FFFF

typedef union
{
    uint64_t bits64;
    double num;
} DoubleBits;

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
   { "instanceof",  KEYWORD_INSTANCEOF },
   { "undefined",   KEYWORD_UNDEFINED}
 };

t_context_ptr brama_init       ();
void          brama_compile    (t_context_ptr context, char_ptr data);
void          brama_run        (t_context_ptr context);
void_ptr      brama_last_error (t_context_ptr context);
void          brama_dump       (t_context_ptr context);
void          brama_dump_ast   (t_context_ptr context);
void          brama_destroy    (t_context_ptr context);


#endif // BRAMA_H
