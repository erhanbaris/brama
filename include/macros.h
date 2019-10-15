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

#define NEW_PRIMATIVE_DEF(EXT, TYPE, PRI_TYPE, STR_TYPE)       \
    t_ast* new_primative_ast_##EXT ( TYPE value ) {         \
        t_ast* ast             = BRAMA_MALLOC(sizeof (t_ast));  \
        t_primative* primative = BRAMA_MALLOC(sizeof (t_primative)); \
        ast->primative_ptr     = primative;                    \
        ast->type              = AST_PRIMATIVE ;               \
        primative-> STR_TYPE   = value;                        \
        primative->type        = PRI_TYPE ;                    \
        return ast;                                            \
    }


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

#    define BRAMA_MALLOC(SIZE) _malloc_dbg( SIZE , _NORMAL_BLOCK , __FILE__, __LINE__ )
#    define BRAMA_FREE(PTR)    _free_dbg  ( PTR  , _NORMAL_BLOCK)

#else
#define BRAMA_MALLOC malloc
#define BRAMA_FREE   free
#endif

#endif // MACROS_H
