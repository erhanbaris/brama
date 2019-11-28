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

brama_status check_end_of_line(t_context_ptr context, int operators);

t_token_ptr ast_peek          (t_context_ptr context);
t_token_ptr ast_previous      (t_context_ptr context);
t_token_ptr ast_next          (t_context_ptr context);
t_token_ptr ast_consume       (t_context_ptr context);
bool        ast_is_at_end     (t_context_ptr context);
bool        is_primative      (t_token_ptr token);
bool        is_next_new_line  (t_context_ptr context);
void        set_semicolon_and_newline(t_context_ptr context, t_ast_ptr ast);

brama_status as_primative            (t_context_ptr context, t_token_ptr token, t_ast_ptr_ptr ast);
brama_status ast_parser              (t_context_ptr context);
brama_status ast_declaration_stmt    (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_expression          (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_unary_expr          (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_func_call           (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_symbol_expr         (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_primary_expr        (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_multiplication_expr (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_modulo_expr         (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_addition_expr       (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_bitwise_shift_expr  (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_control_expr        (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_equality_expr       (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_and_expr            (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_or_expr             (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_assignment_expr     (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_block_multiline_stmt(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_function_decleration(t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_assignable          (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_new_object          (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_while_loop          (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_for_loop            (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_if_stmt             (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_return_stmt         (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_block_body          (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_continue            (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_break               (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_bitwise_or_expr     (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_bitwise_xor_expr    (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_continue_stmt       (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_accessor_stmt       (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);
brama_status ast_switch_stmt         (t_context_ptr context, t_ast_ptr_ptr ast, brama_ast_extra_data_type extra_data);

bool destroy_ast                (t_context_ptr context, t_ast_ptr ast);
bool destroy_ast_vector         (t_context_ptr context, vec_ast_ptr vector);
bool destroy_ast_case_vector    (t_context_ptr context, vec_case_item_ptr vector);
bool destroy_token_vector       (t_context_ptr context, vec_token_ptr vector);
bool destroy_ast_primative      (t_context_ptr context, t_primative_ptr primative);
bool destroy_ast_assignment     (t_context_ptr context, t_assign_ptr assignment);
bool destroy_ast_binary         (t_context_ptr context, t_binary_ptr binary);
bool destroy_ast_control        (t_context_ptr context, t_control_ptr binary);
bool destroy_ast_while_loop     (t_context_ptr context, t_while_loop_ptr while_ptr);
bool destroy_ast_for_loop       (t_context_ptr context, t_for_loop_ptr for_ptr);
bool destroy_ast_func_call      (t_context_ptr context, t_func_call_ptr func_call_ptr);
bool destroy_ast_unary          (t_context_ptr context, t_unary_ptr unary_ptr);
bool destroy_ast_func_decl      (t_context_ptr context, t_func_decl_ptr func_decl_ptr);
bool destroy_ast_object_creation(t_context_ptr context, t_object_creation_ptr object_creation_ptr);
bool destroy_ast_if_stmt        (t_context_ptr context, t_if_stmt_ptr if_stmt_ptr);
bool destroy_ast_accessor       (t_context_ptr context, t_accessor_ptr accessor_ptr);
bool destroy_ast_switch_stmt    (t_context_ptr context, t_switch_stmt_ptr switch_stmt_ptr);

void brama_dump_vector_internal (vec_ast_ptr vector, int level);
void brama_dump_ast_internal    (t_ast_ptr ast, int level);

void run                        (t_context_ptr context);
void brama_compile_dump         (t_context_ptr context);
void brama_compile_dump_storage (t_context_ptr context, t_storage_ptr storage);
void brama_compile_dump_memory  (t_context_ptr context, t_brama_value* variables, map_size_t_ptr variable_names, size_t size);
void brama_compile_dump_codes   (t_context_ptr context);

brama_status brama_get_var        (t_context_ptr context, char_ptr var_name, t_get_var_info** var_info);
brama_status brama_destroy_get_var(t_context_ptr context, t_get_var_info** var_info);

int    get_constant_address  (t_context_ptr context, t_storage_ptr storage, t_brama_value value);
void   add_constant          (t_context_ptr context, t_storage_ptr storage, t_brama_value value);
int    get_variable_address  (t_context_ptr context, t_storage_ptr storage, char_ptr name);
bool   is_text_defined_to_storage(t_context_ptr context, t_storage_ptr storage, char_ptr name);
void   add_variable          (t_context_ptr context, t_storage_ptr storage, char_ptr name, t_brama_value value, memory_prototype_item_type type);
void   locate_variables_to_memory(t_context_ptr context, t_storage_ptr storage);

void compile                    (t_context_ptr context);
void compile_internal           (t_context_ptr context, t_ast_ptr const ast,        t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_primative          (t_context_ptr context, t_primative_ptr const ast,  t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_binary             (t_context_ptr context, t_binary_ptr const ast,     t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_control            (t_context_ptr context, t_control_ptr const ast,    t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_symbol             (t_context_ptr context, char_ptr const ast,         t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_if                 (t_context_ptr context, t_if_stmt_ptr const ast,    t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_for                (t_context_ptr context, t_for_loop_ptr const ast,   t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_while              (t_context_ptr context, t_while_loop_ptr const ast, t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_block              (t_context_ptr context, vec_ast_ptr const ast,      t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_unary              (t_context_ptr context, t_unary_ptr const ast,      t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_assignment         (t_context_ptr context, t_assign_ptr const ast,     t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_keyword            (t_context_ptr context, t_ast_ptr const ast,        t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_func_call          (t_context_ptr context, t_func_call_ptr const ast,  t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_func_decl          (t_context_ptr context, t_func_decl_ptr const ast,  t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_return             (t_context_ptr context, t_ast_ptr const ast,        t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_switch             (t_context_ptr context, t_switch_stmt_ptr const ast,t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_add_to_dict        (t_context_ptr context, t_assign_ptr const ast,     t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_get_from_dict      (t_context_ptr context, t_ast_ptr const ast,        t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
void compile_accessor           (t_context_ptr context, t_accessor_ptr const ast,   t_storage_ptr storage, t_compile_info_ptr compile_info, brama_ast_type upper_ast);
brama_status compile_is_up_value(t_context_ptr context, char_ptr const ast,         t_storage_ptr storage, size_t* storage_id, size_t* variable_index);

t_compile_stack_ptr new_compile_stack         (t_context_ptr context, brama_compile_block_type ast_type, void_ptr ast, void_ptr compile_obj) ;
brama_status        find_compile_stack        (t_context_ptr context, brama_compile_block_type ast_type, t_compile_stack_ptr* stack);
void                remove_from_compile_stack (t_context_ptr context, t_compile_stack_ptr stack);
void                destroy_from_compile_stack(t_context_ptr context, t_compile_stack_ptr stack);

t_vm_object_ptr new_vm_object(t_context_ptr context);
t_brama_value   numberToValue(double num);
double          valueToNumber(t_brama_value num);

#endif // BRAMA_INTERNAL_H
