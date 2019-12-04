#ifndef TESTS_CORE_H
#define TESTS_CORE_H

#include "munit.h"

#define ADD_TEST(TEST)  { "/" #TEST, TEST, setUp, tearDown, MUNIT_TEST_OPTION_NONE, NULL }

#define CHECK_OK(NAME, TEXT) \
    MunitResult NAME (const MunitParameter params[], void* user_data_or_fixture) { \
        t_context* context = brama_init(0);                                         \
        brama_compile(context,  TEXT );                                            \
        context->parser->index = 0;                                                \
        t_ast_ptr ast = NULL;                                                      \
        munit_assert_int(ast_declaration_stmt(context, &ast, AST_IN_NONE), == , BRAMA_OK);\
        brama_destroy(context); CLEAR_AST(ast);                                    \
        return MUNIT_OK;                                                           \
    }

#define CHECK_STATUS(NAME, TEXT, STATUS) \
    MunitResult NAME (const MunitParameter params[], void* user_data_or_fixture) { \
        t_context* context = brama_init(0);                                         \
        brama_compile(context,  TEXT );                                            \
        context->parser->index = 0;                                                \
        t_ast_ptr ast = NULL;                                                      \
        munit_assert_int(ast_declaration_stmt(context, &ast, AST_IN_NONE), == , STATUS  );\
        brama_destroy(context);  CLEAR_AST(ast);                                   \
        return MUNIT_OK;                                                           \
    }

#define BOOL_TEST(NAME, TEXT, STATUS) \
    MunitResult NAME (const MunitParameter params[], void* user_data_or_fixture) {\
    t_context* context = brama_init(0);\
    brama_compile(context, TEXT );\
    brama_run(context);\
    t_get_var_info_ptr var_info = NULL;\
    brama_status status         = brama_get_var(context, "result", &var_info);\
    munit_assert_int(status,         == , BRAMA_OK);\
    munit_assert_int(var_info->type, == , CONST_BOOL);\
    munit_assert_int(var_info->bool_,== , STATUS );\
    brama_destroy_get_var(context, &var_info);\
    brama_destroy(context);\
    return MUNIT_OK;\
}

#define BOOL_TRUE_TEST(NAME, TEXT)  BOOL_TEST(NAME, TEXT, true)
#define BOOL_FALSE_TEST(NAME, TEXT) BOOL_TEST(NAME, TEXT, false)

#if defined(_WIN32)

#    define _CRTDBG_MAP_ALLOC
#    include <stdlib.h>
#    include <crtdbg.h>

_CrtMemState *s1 = NULL;
#endif

static void* setUp(const MunitParameter params[], void* user_data) {
    #if defined(_WIN32)
    #pragma push_macro("malloc")
    #undef malloc

    s1 = (_CrtMemState*)malloc(sizeof(_CrtMemState));
    _CrtMemCheckpoint(s1);

    #pragma pop_macro("malloc")
    #endif
    return NULL;
}

static void tearDown(void* fixture) {
    #if defined(_WIN32)
        // Send all reports to STDOUT
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    //_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    //_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

    _CrtMemState s2;
    _CrtMemState s3;

    _CrtMemCheckpoint(&s2);
	if (_CrtMemDifference(&s3, s1, &s2)){ // only report memory leak if happens indeed
		_CrtMemDumpStatistics(&s3);
        _CrtDumpMemoryLeaks();
        getchar();
        exit(1);
	}

    #pragma push_macro("free")
    #undef free
    free(s1);
    #pragma pop_macro("free")
    s1 = NULL;
    
    #endif
}

#endif // TESTS_CORE_H
