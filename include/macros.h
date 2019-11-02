#ifndef MACROS_H
#define MACROS_H

#include <limits.h>

/* Calculating absolute value with branches (if) very slow. Using bitwise operation much for faster than if.
   http://graphics.stanford.edu/~seander/bithacks.html#IntegerAbs */
#define FAST_ABS(SOURCE_VAR) do {\
    abs_mask = SOURCE_VAR >> sizeof(int) * CHAR_BIT - 1;\
    abs_value = ( SOURCE_VAR + abs_mask) ^ abs_mask; \
}while(false)

#define FAST_MAX(X, Y) X ^ (( X ^ Y ) & -( X < Y ));
#define FAST_MIN(X, Y) Y ^ (( X ^ Y ) & -( X < Y ));

#define OPERATOR_CASE_DOUBLE_START_WITH(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_3_SYMBOL, OPERATOR_1, OPERATOR_2, OPERATOR_3) \
    case OPERATOR_1_SYMBOL :                       \
        if (chNext == OPERATOR_2_SYMBOL ) {        \
            token->opt = OPERATOR_2 ;             \
            increase(tokinizer);                   \
        } else if (chNext == OPERATOR_3_SYMBOL ) { \
            token->opt = OPERATOR_3 ;             \
            increase(tokinizer);                   \
        } else token->opt = OPERATOR_1 ;          \
        break;

#define OPERATOR_CASE_DOUBLE_START_WITH_FOUR(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_3_SYMBOL, OPERATOR_4_SYMBOL, OPERATOR_1, OPERATOR_2, OPERATOR_3, OPERATOR_4) \
    case OPERATOR_1_SYMBOL :                       \
        if (chNext == OPERATOR_2_SYMBOL ) {        \
            token->opt = OPERATOR_2 ;             \
            increase(tokinizer);                   \
        } else if (chNext == OPERATOR_3_SYMBOL ) { \
            token->opt = OPERATOR_3 ;             \
            increase(tokinizer);                   \
        } else if (chNext == OPERATOR_4_SYMBOL ) { \
            token->opt = OPERATOR_4 ;             \
            increase(tokinizer);                   \
        } else token->opt = OPERATOR_1 ;          \
        break;

#define OPERATOR_CASE_DOUBLE(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_1, OPERATOR_2)  \
    case OPERATOR_1_SYMBOL :                \
        if (chNext == OPERATOR_2_SYMBOL ) { \
            token->opt = OPERATOR_2 ;      \
            increase(tokinizer);            \
        } else token->opt = OPERATOR_1 ;   \
        break;

#define OPERATOR_CASE_TRIBLE(OPERATOR_1_SYMBOL, OPERATOR_2_SYMBOL, OPERATOR_3_SYMBOL, OPERATOR_1, OPERATOR_2, OPERATOR_3) \
case OPERATOR_1_SYMBOL :                     \
    if (chNext == OPERATOR_2_SYMBOL ) {      \
        increase(tokinizer);                 \
        if (chThird == OPERATOR_3_SYMBOL ) { \
            token->opt = OPERATOR_3 ;       \
            increase(tokinizer);             \
        } else token->opt = OPERATOR_2 ;    \
    } else token->opt = OPERATOR_1 ;        \
    break;

#define OPERATOR_CASE_SINGLE(OPERATOR_SYMBOL, OPERATOR)  \
    case OPERATOR_SYMBOL :\
        token->opt = OPERATOR ;\
    break;

#define RESULT_CHECK(RESULT)\
    if (RESULT != BRAMA_OK) {\
        return RESULT;\
    }

#define get_operator_type(token) token->opt
#define get_keyword_type(token) token->keyword

#define IS_ITEM(NAME, TYPE) \
    bool is_##NAME (t_token* token)\
    {\
        return token != NULL && token->type == TYPE ;\
    }

#define GET_ITEM(NAME, TYPE, OUT) \
    OUT get_##NAME (t_token* token)\
    {\
        return token-> TYPE ;\
    }

#define new_primative_ast_int(DATA)    new_primative_ast_int_internal( DATA , __FILE__, __LINE__ )
#define new_primative_ast_double(DATA) new_primative_ast_double_internal( DATA , __FILE__, __LINE__ )
#define new_primative_ast_text(DATA)   new_primative_ast_text_internal( DATA , __FILE__, __LINE__ )
#define new_primative_ast_bool(DATA)   new_primative_ast_bool_internal( DATA , __FILE__, __LINE__ )
#define new_primative_ast_empty(DATA)  new_primative_ast_empty_internal( DATA , __FILE__, __LINE__ )
#define new_primative_ast_array(DATA)  new_primative_ast_array_internal( DATA , __FILE__, __LINE__ )
#define new_primative_ast_dict(DATA)   new_primative_ast_dict_internal( DATA , __FILE__, __LINE__ )

#define NEW_PRIMATIVE_DEF(EXT, TYPE, PRI_TYPE, STR_TYPE)       \
    t_ast* new_primative_ast_##EXT##_internal ( TYPE value, int FILE__ , char_ptr LINE__ ) {         \
        t_ast* ast             = BRAMA_MALLOC_LINE(sizeof (t_ast), FILE__ , LINE__ );  \
        t_primative* primative = BRAMA_MALLOC_LINE(sizeof (t_primative), FILE__ , LINE__ ); \
        ast->primative_ptr     = primative;                    \
        ast->type              = AST_PRIMATIVE ;               \
        ast->ends_with_newline   = false;                      \
        ast->ends_with_semicolon = false;                      \
        primative-> STR_TYPE   = value;                        \
        primative->type        = PRI_TYPE ;                    \
        ast->create_new_storage = false ;                      \
        return ast;                                            \
    }


#define NEW_AST_DEF(NAME, INPUT, STR_TYPE, TYPE, NEWSTORAGE)       \
    t_ast* new_##NAME##_ast_internal(INPUT variable, int FILE__ , char_ptr LINE__ ) {   \
        t_ast_ptr ast           = BRAMA_MALLOC_LINE(sizeof (t_ast), FILE__ , LINE__ );  \
        ast->type               = STR_TYPE;    \
        ast-> TYPE              = variable;    \
        ast->ends_with_newline   = false;      \
        ast->ends_with_semicolon = false;      \
        ast->create_new_storage = NEWSTORAGE ; \
        return ast;                            \
    }

#define new_symbol_ast(DATA)       new_symbol_ast_internal   ( DATA , __FILE__, __LINE__ )
#define new_unary_ast(DATA)        new_unary_ast_internal    ( DATA , __FILE__, __LINE__ )
#define new_binary_ast(DATA)       new_binary_ast_internal   ( DATA , __FILE__, __LINE__ )
#define new_control_ast(DATA)      new_control_ast_internal  ( DATA , __FILE__, __LINE__ )
#define new_assign_ast(DATA)       new_assign_ast_internal   ( DATA , __FILE__, __LINE__ )
#define new_func_call_ast(DATA)    new_func_call_ast_internal( DATA , __FILE__, __LINE__ )
#define new_func_decl_ast(DATA)    new_func_decl_ast_internal( DATA , __FILE__, __LINE__ )
#define new_block_ast(DATA)        new_block_ast_internal    ( DATA , __FILE__, __LINE__ )
#define new_object_ast(DATA)       new_object_ast_internal   ( DATA , __FILE__, __LINE__ )
#define new_while_ast(DATA)        new_while_ast_internal    ( DATA , __FILE__, __LINE__ )
#define new_if_ast(DATA)           new_if_ast_internal       ( DATA , __FILE__, __LINE__ )
#define new_return_ast(DATA)       new_return_ast_internal   ( DATA , __FILE__, __LINE__ )
#define new_accessor_ast(DATA)     new_accessor_ast_internal ( DATA , __FILE__, __LINE__ )
#define new_keyword_ast(DATA)      new_keyword_ast_internal  ( DATA , __FILE__, __LINE__ )


#define BACKUP_PARSER_INDEX()                        size_t parser_index = context->parser->index;
#define RESTORE_PARSER_INDEX()                       context->parser->index = parser_index;
#define RESTORE_PARSER_INDEX_AND_RETURN(RETURN_CODE) { RESTORE_PARSER_INDEX(); return RETURN_CODE ; }
#define DESTROY_AST_AND_RETURN(RETURN_CODE, VAR) \
{                          \
    RESTORE_PARSER_INDEX();\
    CLEAR_AST( VAR );    \
    return RETURN_CODE ;   \
}

#define CLEAR_AST(AST)       if ( AST    != NULL ) { destroy_ast   ( AST );        BRAMA_FREE( AST );    AST    = NULL; }
#define CLEAR_VECTOR(VECTOR) if ( VECTOR != NULL ) { destroy_ast_vector( VECTOR ); BRAMA_FREE( VECTOR ); VECTOR = NULL; }

#define vector_get(VECTOR, INDEX) VECTOR ->data[ INDEX ]

#if defined(_WIN32)
#    define _CRTDBG_MAP_ALLOC
#    include <stdlib.h>
#    include <crtdbg.h>

#    define BRAMA_CALLOC(NUM, SIZE)                 _calloc_dbg( NUM , SIZE , _NORMAL_BLOCK , __FILE__, __LINE__ )
#    define BRAMA_MALLOC(SIZE)                      _malloc_dbg( SIZE , _NORMAL_BLOCK , __FILE__, __LINE__ )
#    define BRAMA_MALLOC_LINE(SIZE, FILE__, LINE__) _malloc_dbg( SIZE , _NORMAL_BLOCK , FILE__, LINE__ )

#    define BRAMA_FREE(PTR)      _free_dbg  ( PTR  , _NORMAL_BLOCK)
#else
#    define BRAMA_MALLOC_LINE(SIZE, FILE__, LINE__) malloc ( SIZE )
#    define BRAMA_CALLOC(NUM, SIZE)                 calloc ( NUM , SIZE )
#    define BRAMA_MALLOC( SIZE )                    malloc ( SIZE )
#    define BRAMA_FREE(PTR)                         free   ( PTR )

#endif

#endif // MACROS_H
