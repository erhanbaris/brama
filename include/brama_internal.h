#ifndef BRAMA_INTERNAL_H
#define BRAMA_INTERNAL_H

#include "brama.h"

t_token_ptr ast_consume_keyword  (t_context_ptr context, brama_keyword_type  keyword_type);
t_token_ptr ast_consume_token    (t_context_ptr context, brama_token_type    token_type);
t_token_ptr ast_consume_operator (t_context_ptr context, brama_operator_type operator_type);

bool        ast_match_keyword  (t_context_ptr context, size_t types_count, ...);
bool        ast_match_operator (t_context_ptr context, size_t types_count, ...);
bool        ast_match_token    (t_context_ptr context, size_t types_count, ...);

bool        ast_check_keyword  (t_context_ptr context, brama_keyword_type  keyword_type);
bool        ast_check_operator (t_context_ptr context, brama_operator_type operator_type);
bool        ast_check_token    (t_context_ptr context, brama_token_type    token_type);

t_token_ptr ast_peek          (t_context_ptr context);
t_token_ptr ast_previous      (t_context_ptr context);
t_token_ptr ast_next          (t_context_ptr context);
t_token_ptr ast_consume       (t_context_ptr context);
bool        ast_is_at_end     (t_context_ptr context);
bool        is_primative      (t_token_ptr token);

brama_status ast_parser              (t_context_ptr context);
brama_status ast_declaration_stmt    (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_expression          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_unary_expr          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_call                (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_symbol_expr         (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_primary_expr        (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_multiplication_expr (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_addition_expr       (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_control_expr        (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_equality_expr       (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_and_expr            (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_or_expr             (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_assignment_expr     (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_block_stmt          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_function_decleration(t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_assignable          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_new_object          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_while_loop          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_if_stmt             (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);

void destroy_ast           (t_ast_ptr ast);
void destroy_vector        (t_vector_ptr vector);
void destroy_ast_primative (t_primative_ptr primative);
void destroy_ast_assignment(t_assign_ptr assignment);
void destroy_ast_binary    (t_binary_ptr binary);

#endif // BRAMA_INTERNAL_H
