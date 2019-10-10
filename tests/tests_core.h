#ifndef TESTS_CORE_H
#define TESTS_CORE_H

#include "munit.h"

#define ADD_TEST(TEST)  { "/" #TEST, TEST, setUp, tearDown, MUNIT_TEST_OPTION_NONE, NULL }

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

#if defined(_WIN32)

#    define _CRTDBG_MAP_ALLOC
#    include <stdlib.h>
#    include <crtdbg.h>

_CrtMemState *s1 = NULL;
#endif

void setUp(const MunitParameter params[], void* user_data) {
    #if defined(_WIN32)
    s1 = (_CrtMemState*)malloc(sizeof(_CrtMemState));
    _CrtMemCheckpoint(s1);
    #endif
}

void tearDown(void* fixture) {
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

    free(s1);
    s1 = NULL;
    #endif
}

#endif // TESTS_CORE_H
