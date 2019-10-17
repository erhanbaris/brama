#ifndef MACROS_H
#define MACROS_H

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

#define OPERATOR_CASE_SINGLE(OPERATOR_SYMBOL, OPERATOR)  \
    case OPERATOR_SYMBOL :\
        token->int_ = OPERATOR ;\
    break;

#define RESULT_CHECK(RESULT)\
    if (RESULT != BRAMA_OK) {\
        return RESULT;\
    }

#define get_operator_type(token) token->int_
#define get_keyword_type(token) token->int_

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

#define new_primative_ast_int(DATA)    new_primative_ast_int_internal( DATA , __FILE__, __LINE__)
#define new_primative_ast_double(DATA) new_primative_ast_double_internal( DATA , __FILE__, __LINE__)
#define new_primative_ast_text(DATA)   new_primative_ast_text_internal( DATA , __FILE__, __LINE__)
#define new_primative_ast_bool(DATA)   new_primative_ast_bool_internal( DATA , __FILE__, __LINE__)
#define new_primative_ast_empty(DATA)  new_primative_ast_empty_internal( DATA , __FILE__, __LINE__)
#define new_primative_ast_array(DATA)  new_primative_ast_array_internal( DATA , __FILE__, __LINE__)
#define new_primative_ast_dict(DATA)   new_primative_ast_dict_internal( DATA , __FILE__, __LINE__)

#define NEW_PRIMATIVE_DEF(EXT, TYPE, PRI_TYPE, STR_TYPE)       \
    t_ast* new_primative_ast_##EXT##_internal ( TYPE value ) {         \
        t_ast* ast             = BRAMA_MALLOC(sizeof (t_ast));  \
        t_primative* primative = BRAMA_MALLOC(sizeof (t_primative)); \
        ast->primative_ptr     = primative;                    \
        ast->type              = AST_PRIMATIVE ;               \
        primative-> STR_TYPE   = value;                        \
        primative->type        = PRI_TYPE ;                    \
        return ast;                                            \
    }


#define NEW_AST_DEF(NAME, INPUT, STR_TYPE, TYPE)       \
    t_ast_ptr new_##NAME##_ast_internal(INPUT variable, int FILE__ , char_ptr LINE__ ) {       \
        t_ast_ptr ast = BRAMA_MALLOC_LINE(sizeof (t_ast), FILE__ , LINE__ );  \
        ast->type     = STR_TYPE;                      \
        ast-> TYPE    = variable;                      \
        return ast;                                    \
    }

#define NEW_AST_DEF_NULL(NAME, STR_TYPE)               \
    t_ast_ptr new_##NAME##_ast() {                     \
        t_ast_ptr ast = BRAMA_MALLOC(sizeof (t_ast));  \
        ast->type     = STR_TYPE;                      \
        ast->ast_ptr  = NULL;                          \
        return ast;                                    \
    }

#define new_symbol_ast(DATA)       new_symbol_ast_internal( DATA , __FILE__, __LINE__)
#define new_unary_ast(DATA)        new_unary_ast_internal( DATA , __FILE__, __LINE__)
#define new_binary_ast(DATA)       new_binary_ast_internal( DATA , __FILE__, __LINE__)
#define new_control_ast(DATA)      new_control_ast_internal( DATA , __FILE__, __LINE__)
#define new_assign_ast(DATA)       new_assign_ast_internal( DATA , __FILE__, __LINE__)
#define new_func_call_ast(DATA)    new_func_call_ast_internal( DATA , __FILE__, __LINE__)
#define new_func_decl_ast(DATA)    new_func_decl_ast_internal( DATA , __FILE__, __LINE__)
#define new_block_ast(DATA)        new_block_ast_internal( DATA , __FILE__, __LINE__)
#define new_object_ast(DATA)       new_object_ast_internal( DATA , __FILE__, __LINE__)
#define new_while_ast(DATA)        new_while_ast_internal( DATA , __FILE__, __LINE__)
#define new_if_ast(DATA)           new_if_ast_internal( DATA , __FILE__, __LINE__)
#define new_return_ast(DATA)       new_return_ast_internal( DATA , __FILE__, __LINE__)
#define new_accessor_ast(DATA)     new_accessor_ast_internal( DATA , __FILE__, __LINE__)
#define new_keyword_ast(DATA)      new_keyword_ast_internal( DATA , __FILE__, __LINE__)


#define BACKUP_PARSER_INDEX()                        size_t parser_index = context->parser->index;
#define RESTORE_PARSER_INDEX()                       context->parser->index = parser_index;
#define RESTORE_PARSER_INDEX_AND_RETURN(RETURN_CODE) { RESTORE_PARSER_INDEX(); return RETURN_CODE ; }
#define DESTROY_AST_AND_RETURN(RETURN_CODE, VAR) \
{                          \
    RESTORE_PARSER_INDEX();\
    CLEAR_AST( VAR );    \
    return RETURN_CODE ;   \
}

#define CLEAR_AST(AST)       if ( AST    != NULL ) { destroy_ast   ( AST );    BRAMA_FREE( AST );    AST    = NULL; }
#define CLEAR_VECTOR(VECTOR) if ( VECTOR != NULL ) { vector_destroy( VECTOR ); BRAMA_FREE( VECTOR ); VECTOR = NULL; }


#if defined(_WIN32)

#    define _CRTDBG_MAP_ALLOC
#    include <stdlib.h>
#    include <crtdbg.h>

#    define BRAMA_MALLOC(SIZE)                      _malloc_dbg( SIZE , _NORMAL_BLOCK , __FILE__, __LINE__ )
#    define BRAMA_MALLOC_LINE(SIZE, FILE__, LINE__) _malloc_dbg( SIZE , _NORMAL_BLOCK , FILE__, LINE__ )

#    define BRAMA_FREE(PTR)      _free_dbg  ( PTR  , _NORMAL_BLOCK)
#else
#    define BRAMA_MALLOC_LINE(SIZE, FILE__, LINE__) malloc ( SIZE )
#    define BRAMA_MALLOC( SIZE )                    malloc ( SIZE )
#    define BRAMA_FREE(PTR)                         free   ( PTR )

#endif

#endif // MACROS_H
