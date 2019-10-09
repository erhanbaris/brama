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
    brama_execute(context,  "{'hi': 'all', test: true, 'dict': {'empty': false}, 'array': [1,2,3]}");
    brama_destroy(context);
#ifdef _WIN32
    int test_status = munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);

    // Send all reports to STDOUT
    _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
    _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
    _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
    //_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
    //_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );
    _CrtDumpMemoryLeaks();

    getchar();
    return test_status;
#else
    return munit_suite_main(&ALL_SUITE, (void*) "µnit", NULL, NULL);
#endif
}
