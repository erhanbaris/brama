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

#define TOTAL_ARGS_VAR "!total_args"
#define RETURN_VAR "!return"

#define BRAMA_ASSERT(DATA)

#define vm_decode(instr, t) do {\
    (t).op   = ((instr) & OP_MASK  ) >> 24;\
    (t).reg1 = ((instr) & REG1_MASK) >> 16;\
    (t).reg2 = ((instr) & REG2_MASK) >> 8;\
    (t).reg3 = ((instr) & REG3_MASK);\
    (t).scal = ((instr) & SCAL_MASK);\
} while(0)

#define vm_encode(t)    ((((t).op   << 24) & OP_MASK  ) |\
                         (((t).reg1 << 16) & REG1_MASK) |\
                         (((t).reg2 << 8 ) & REG2_MASK) |\
                         (((t).reg3      ) & REG3_MASK) |\
                         (((t).scal      ) & SCAL_MASK))

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

#define COMPILE_CHECK()\
    if (context->status != BRAMA_OK) {\
        return ;\
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

#define TWO_VARIABLE_COMPARE() \
do { \
    if (IS_NUM(left)){\
        tmp_data_1.bits64 = left;\
        left_val = tmp_data_1.num;\
    }\
    else if (IS_BOOL(left))\
        left_val = IS_TRUE(left) ? 1 : 0;\
    else if (IS_STRING(left))\
        left_val = MurmurHash64B(AS_STRING(left), strlen(AS_STRING(left)), 1024);\
    else if (IS_UNDEFINED(left) || IS_NULL(left))\
        left_val = (QNAN | TAG_UNDEFINED);\
    if (IS_NUM(right)){\
        tmp_data_1.bits64 = right;\
        right_val = tmp_data_1.num;\
    }\
    else if (IS_BOOL(right))\
        right_val = IS_TRUE(right) ? 1 : 0;\
    else if (IS_STRING(right))\
        right_val = MurmurHash64B(AS_STRING(right), strlen(AS_STRING(right)), 1024);\
    else if (IS_UNDEFINED(right) || IS_NULL(right))\
        right_val = (QNAN | TAG_UNDEFINED);\
} while(false)


#define new_primative_ast_int(DATA)        new_primative_ast_int_internal(context, DATA , __FILE__, __LINE__ )
#define new_primative_ast_double(DATA)     new_primative_ast_double_internal(context, DATA , __FILE__, __LINE__ )
#define new_primative_ast_text(DATA)       new_primative_ast_text_internal(context, DATA , __FILE__, __LINE__ )
#define new_primative_ast_bool(DATA)       new_primative_ast_bool_internal(context, DATA , __FILE__, __LINE__ )
#define new_primative_ast_null(DATA)       new_primative_ast_null_internal(context, DATA , __FILE__, __LINE__ )
#define new_primative_ast_undefined(DATA)  new_primative_ast_undefined_internal(context, DATA , __FILE__, __LINE__ )
#define new_primative_ast_array(DATA)      new_primative_ast_array_internal(context, DATA , __FILE__, __LINE__ )
#define new_primative_ast_dict(DATA)       new_primative_ast_dict_internal(context, DATA , __FILE__, __LINE__ )

#define NEW_PRIMATIVE_DEF(EXT, TYPE, PRI_TYPE, STR_TYPE)       \
    t_ast* new_primative_ast_##EXT##_internal ( t_context_ptr context, TYPE value, int FILE__ , char_ptr LINE__ ) {         \
        t_ast* ast             = BRAMA_MALLOC_LINE(sizeof (t_ast), FILE__ , LINE__ );  \
        t_primative* primative = BRAMA_MALLOC_LINE(sizeof (t_primative), FILE__ , LINE__ ); \
        ast->primative_ptr     = primative;                    \
        ast->type              = AST_PRIMATIVE ;               \
        ast->ends_with_newline   = false;                      \
        ast->ends_with_semicolon = false;                      \
        primative-> STR_TYPE   = value;                        \
        primative->type        = PRI_TYPE ;                    \
        return ast;                                            \
    }


#define NEW_AST_DEF(NAME, INPUT, STR_TYPE, TYPE)       \
    t_ast* new_##NAME##_ast_internal(t_context_ptr context, INPUT variable, int FILE__ , char_ptr LINE__ ) {   \
        t_ast_ptr ast           = BRAMA_MALLOC_LINE(sizeof (t_ast), FILE__ , LINE__ );  \
        ast->type               = STR_TYPE;    \
        ast-> TYPE              = variable;    \
        ast->ends_with_newline   = false;      \
        ast->ends_with_semicolon = false;      \
        return ast;                            \
    }

#define new_symbol_ast(DATA)       new_symbol_ast_internal   (context,  DATA , __FILE__, __LINE__ )
#define new_unary_ast(DATA)        new_unary_ast_internal    (context,  DATA , __FILE__, __LINE__ )
#define new_binary_ast(DATA)       new_binary_ast_internal   (context,  DATA , __FILE__, __LINE__ )
#define new_control_ast(DATA)      new_control_ast_internal  (context,  DATA , __FILE__, __LINE__ )
#define new_assign_ast(DATA)       new_assign_ast_internal   (context,  DATA , __FILE__, __LINE__ )
#define new_func_call_ast(DATA)    new_func_call_ast_internal(context,  DATA , __FILE__, __LINE__ )
#define new_func_decl_ast(DATA)    new_func_decl_ast_internal(context,  DATA , __FILE__, __LINE__ )
#define new_block_ast(DATA)        new_block_ast_internal    (context,  DATA , __FILE__, __LINE__ )
#define new_object_ast(DATA)       new_object_ast_internal   (context,  DATA , __FILE__, __LINE__ )
#define new_while_ast(DATA)        new_while_ast_internal    (context,  DATA , __FILE__, __LINE__ )
#define new_for_ast(DATA)          new_for_ast_internal      (context,  DATA , __FILE__, __LINE__ )
#define new_if_ast(DATA)           new_if_ast_internal       (context,  DATA , __FILE__, __LINE__ )
#define new_switch_ast(DATA)       new_switch_ast_internal   (context,  DATA , __FILE__, __LINE__ )
#define new_return_ast(DATA)       new_return_ast_internal   (context,  DATA , __FILE__, __LINE__ )
#define new_accessor_ast(DATA)     new_accessor_ast_internal (context,  DATA , __FILE__, __LINE__ )
#define new_keyword_ast(DATA)      new_keyword_ast_internal  (context,  DATA , __FILE__, __LINE__ )


#define BACKUP_PARSER_INDEX()                        size_t parser_index = context->parser->index;
#define RESTORE_PARSER_INDEX()                       context->parser->index = parser_index;
#define RESTORE_PARSER_INDEX_AND_RETURN(RETURN_CODE) \
do {\
    if ( RETURN_CODE != BRAMA_DOES_NOT_MATCH_AST) \
        context->error_location_on_code = __LINE__; \
    RESTORE_PARSER_INDEX(); \
    return RETURN_CODE ;\
} while(0)
#define DESTROY_AST_AND_RETURN(RETURN_CODE, VAR) \
do { \
    RESTORE_PARSER_INDEX();\
    CLEAR_AST( VAR );    \
    return RETURN_CODE ;   \
} while(0)

#define CLEAR_AST(AST)       if ( AST    != NULL ) do { destroy_ast       ( context, AST );   BRAMA_FREE( AST );    AST    = NULL; } while(0)
#define CLEAR_VECTOR(VECTOR) if ( VECTOR != NULL ) do { destroy_ast_vector(context, VECTOR ); BRAMA_FREE( VECTOR ); VECTOR = NULL; } while(0)

#define vector_get(VECTOR, INDEX) VECTOR ->data[ INDEX ]

#if defined(_WIN32)

#    define BRAMA_MALLOC_LINE(SIZE, FILE__, LINE__) context->malloc(context->allocator, SIZE)
#    define BRAMA_CALLOC(NUM, SIZE)                 context->calloc(context->allocator, NUM , SIZE )
#    define BRAMA_MALLOC( SIZE )                    context->malloc(context->allocator, SIZE)
#    define BRAMA_FREE(PTR)                         context->free  (context->allocator, PTR )
#else
#    define BRAMA_MALLOC_LINE(SIZE, FILE__, LINE__) context->malloc(context->allocator, SIZE)
#    define BRAMA_CALLOC(NUM, SIZE)                 context->calloc(context->allocator, NUM , SIZE )
#    define BRAMA_MALLOC( SIZE )                    context->malloc(context->allocator, SIZE)
#    define BRAMA_FREE(PTR)                         context->free  (context->allocator, PTR )

#endif

#endif // MACROS_H
