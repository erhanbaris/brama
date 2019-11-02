cmake_minimum_required(VERSION 3.10)
project(brama) 

# Enforce C11
if ((NOT CMAKE_C_STANDARD) OR (CMAKE_C_STANDARD LESS 11))
    set(CMAKE_C_STANDARD 11)
endif()
set(CMAKE_C_STANDARD_REQUIRED ON)

OPTION( ENABLE_CODECOVERAGE "Enable code coverage testing support" )

if ( ENABLE_CODECOVERAGE )

    if ( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )
        message( WARNING "Code coverage results with an optimised (non-Debug) build may be misleading" )
    endif ( NOT CMAKE_BUILD_TYPE STREQUAL "Debug" )

    if ( NOT DEFINED CODECOV_OUTPUTFILE )
        set( CODECOV_OUTPUTFILE cmake_coverage.output )
    endif ( NOT DEFINED CODECOV_OUTPUTFILE )

    if ( NOT DEFINED CODECOV_HTMLOUTPUTDIR )
        set( CODECOV_HTMLOUTPUTDIR coverage_results )
    endif ( NOT DEFINED CODECOV_HTMLOUTPUTDIR )

    if ( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCXX )
        find_program( CODECOV_GCOV gcov )
        find_program( CODECOV_LCOV lcov )
        find_program( CODECOV_GENHTML genhtml )
        add_definitions( -fprofile-arcs -ftest-coverage )
        link_libraries( gcov )
        set( CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} --coverage )
        add_custom_target( coverage_init ALL ${CODECOV_LCOV} --base-directory .  --directory ${CMAKE_BINARY_DIR} --output-file ${CODECOV_OUTPUTFILE} --capture --initial )
        add_custom_target( coverage ${CODECOV_LCOV} --base-directory .  --directory ${CMAKE_BINARY_DIR} --output-file ${CODECOV_OUTPUTFILE} --capture COMMAND genhtml -o ${CODECOV_HTMLOUTPUTDIR} ${CODECOV_OUTPUTFILE} )
    endif ( CMAKE_COMPILER_IS_GNUCXX )

endif (ENABLE_CODECOVERAGE)

add_definitions( -DSTATIC_PY_VERBOSE_MODE )
include_directories(include/)
include_directories(tests/)

add_executable(brama
        src/main.c
        src/map.c
        src/vec.c
        include/map.h
        tests/munit.c
        tests/munit.h
        src/brama.c
        include/brama.h
        include/tools.h
        src/tools.c
        tests/tests.h
        tests/tests_core.h
        tests/tests_token.h
        tests/tests_ast.h
        tests/tests_compile.h)

