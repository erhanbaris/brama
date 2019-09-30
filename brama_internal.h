#ifndef BRAMA_INTERNAL_H
#define BRAMA_INTERNAL_H

#include "static_py.h"

t_token_ptr ast_consume_keyword  (t_context_ptr context, int keyword_type);
t_token_ptr ast_consume_token    (t_context_ptr context, int token_type);
t_token_ptr ast_consume_operator (t_context_ptr context, int operator_type);

bool        ast_match_keyword  (t_context_ptr context, int types_count, ...);
bool        ast_match_operator (t_context_ptr context, int types_count, ...);
bool        ast_match_token    (t_context_ptr context, int types_count, ...);

bool        ast_check_keyword  (t_context_ptr context, int keyword_type);
bool        ast_check_operator (t_context_ptr context, int operator_type);
bool        ast_check_token    (t_context_ptr context, int token_type);

t_token_ptr ast_peek         (t_context_ptr context);
t_token_ptr ast_previous     (t_context_ptr context);
t_token_ptr ast_consume      (t_context_ptr context);
bool        ast_is_at_end    (t_context_ptr context);
bool        is_primative     (t_token_ptr token);

STATIC_PY_STATUS as_primative(t_token_ptr token, t_ast_ptr_ptr ast);

STATIC_PY_STATUS ast_parser             (t_context_ptr context);
STATIC_PY_STATUS ast_declaration_stmt   (t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_expression         (t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_unary_expr         (t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_call               (t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_symbol_expr        (t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_primary_expr       (t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_multiplication_expr(t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_addition_expr      (t_context_ptr context, t_ast_ptr_ptr ast);
STATIC_PY_STATUS ast_control_expr       (t_context_ptr context, t_ast_ptr_ptr ast);



#endif // BRAMA_INTERNAL_H
