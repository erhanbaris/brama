#ifndef BRAMA_INTERNAL_H
#define BRAMA_INTERNAL_H

#include "static_py.h"

STATIC_PY_STATUS ast_parser(t_context* context);

t_token* ast_consume_keyword  (t_context* context, int keyword_type);
t_token* ast_consume_token    (t_context* context, int token_type);
t_token* ast_consume_operator (t_context* context, int operator_type);

bool ast_match_keyword  (t_context* context, int types_count, ...);
bool ast_match_operator (t_context* context, int types_count, ...);
bool ast_match_token    (t_context* context, int types_count, ...);

bool ast_check_keyword  (t_context* context, int keyword_type);
bool ast_check_operator (t_context* context, int operator_type);
bool ast_check_token    (t_context* context, int token_type);

t_token* ast_peek         (t_context* context);
t_token* ast_previous     (t_context* context);
t_token* ast_consume      (t_context* context);
bool     ast_is_at_end    (t_context* context);
bool     is_primative     (t_token* token);

STATIC_PY_STATUS as_primative(t_token* token, t_ast** ast);

STATIC_PY_STATUS ast_declaration_stmt (t_context* context, t_ast** ast);
STATIC_PY_STATUS ast_expression       (t_context* context, t_ast** ast);
STATIC_PY_STATUS ast_unary_expr       (t_context* context, t_ast** ast);
STATIC_PY_STATUS ast_call             (t_context* context, t_ast** ast);
STATIC_PY_STATUS ast_primary_expr     (t_context* context, t_ast** ast);



#endif // BRAMA_INTERNAL_H
