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
bool        is_next_new_line  (t_context_ptr context);

brama_status as_primative            (t_token_ptr token, t_ast_ptr_ptr ast);
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
brama_status ast_block_multiline_stmt(t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_block_singleline_stmt(t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_function_decleration(t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_assignable          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_new_object          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_while_loop          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_if_stmt             (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_return_stmt         (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);
brama_status ast_block_body          (t_context_ptr context, t_ast_ptr_ptr ast, void_ptr extra_data);

bool destroy_ast                (t_ast_ptr ast);
bool destroy_vector             (t_vector_ptr vector);
bool destroy_ast_primative      (t_primative_ptr primative);
bool destroy_ast_assignment     (t_assign_ptr assignment);
bool destroy_ast_binary         (t_binary_ptr binary);
bool destroy_ast_control        (t_control_ptr binary);
bool destroy_ast_while_loop     (t_while_loop_ptr while_ptr);
bool destroy_ast_func_call      (t_func_call_ptr func_call_ptr);
bool destroy_ast_unary          (t_unary_ptr unary_ptr);
bool destroy_ast_func_decl      (t_func_decl_ptr func_decl_ptr);
bool destroy_ast_object_creation(t_object_creation_ptr object_creation_ptr);
bool destroy_ast_if_stmt        (t_if_stmt_ptr if_stmt_ptr);

#endif // BRAMA_INTERNAL_H
