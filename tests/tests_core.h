#ifndef TESTS_CORE_H
#define TESTS_CORE_H

#include "munit.h"

#define ADD_TEST(TEST)  { "/" #TEST, TEST, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }

#define CHECK_OK(NAME, TEXT) \
    MunitResult NAME (const MunitParameter params[], void* user_data_or_fixture) { \
        t_context* context = brama_init();                                         \
        brama_execute(context,  TEXT );                                            \
        context->parser->index = 0;                                                \
        t_ast_ptr ast = NULL;                                                      \
        munit_assert_int(ast_declaration_stmt(context, &ast, NULL), == , BRAMA_OK);\
        brama_destroy(context);                                                    \
        return MUNIT_OK;                                                           \
    }

#define CHECK_STATUS(NAME, TEXT, STATUS) \
    MunitResult NAME (const MunitParameter params[], void* user_data_or_fixture) { \
        t_context* context = brama_init();                                         \
        brama_execute(context,  TEXT );                                            \
        context->parser->index = 0;                                                \
        t_ast_ptr ast = NULL;                                                      \
        munit_assert_int(ast_declaration_stmt(context, &ast, NULL), == , STATUS  );\
        brama_destroy(context);                                                    \
        return MUNIT_OK;                                                           \
    }


#endif // TESTS_CORE_H
