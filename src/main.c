#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>

#include <assert.h>
#include <stdint.h>

#include "tests.h"
#include "munit.h"

#include "brama.h"

int main(int argc, const char* argv[]) {
    t_context* context = brama_init();
    brama_execute(context, "~(ld | cols | rd) & all;");

    /*brama_execute(context, "self.addEventListener('message', function(e) {\n"
"  let count = 0;                                                         \n"
"  var findSolutions = function(ld, cols, rd, all) {                      \n"
"    let poss = ~(ld | cols | rd) & all;                                  \n"
"    if (cols === all) {                                                  \n"
"      count++;                                                           \n"
"    }                                                                    \n"
"    while (poss) {                                                       \n"
"      let negPoss = poss * -1;                                           \n"
"      let bit = poss & negPoss;                                          \n"
"      //let bit = poss & -poss;                                          \n"
"      poss = poss - bit;                                                 \n"
"      findSolutions((ld | bit) << 1, cols | bit, (rd | bit) >> 1, all);  \n"
"    }                                                                    \n"
"  };                                                                     \n"
"  findSolutions(e.data[0], e.data[1], e.data[2], e.data[3]);             \n"
"  self.postMessage(count);                                               \n"
"}, false);");             */                                               
    context->parser->index = 0;

    t_ast_ptr ast = (t_ast_ptr)vector_get(context->parser->asts, 0);
    munit_assert_int         (ast->type,                                 ==, AST_FUNCTION_CALL);
    munit_assert_ptr_not_null(ast->func_call_ptr);
    munit_assert_ptr_not_null(ast->func_call_ptr->func_decl_ptr);
    munit_assert_int         (ast->func_call_ptr->type, ==, FUNC_CALL_ANONY);
    munit_assert_ptr_not_null(ast->func_call_ptr->func_decl_ptr->body);
    munit_assert_int         (ast->func_call_ptr->func_decl_ptr->body->type, ==, AST_BLOCK);
    destroy_ast(ast);
    BRAMA_FREE(ast);

    brama_destroy(context);

#ifdef _WIN32
    int test_status = munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
    getchar();
    return test_status;
#else
    return 0;
#endif
}
