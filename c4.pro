TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CFLAGS += -std=c11 -Wint-to-void-pointer-cast
#QMAKE_CFLAGS += --coverage
#QMAKE_LFLAGS += --coverage

INCLUDEPATH += /usr/local/include
INCLUDEPATH += include/
INCLUDEPATH += tests/

SOURCES += \
    src/main.c \
    src/brama.c \
    src/map.c \
    tests/munit.c \
    src/tools.c

HEADERS += \
    include/brama.h \
    include/map.h \
    tests/munit.h \
    tests/tests_core.h \
    tests/tests_token.h \
    tests/tests.h \
    include/tools.h \
    tests/tests_tools.h \
    include/brama_internal.h \
    tests/tests_ast.h \
    tests/macros.h
