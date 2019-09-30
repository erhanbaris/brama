TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CFLAGS += -std=c11 -Wint-to-void-pointer-cast
#QMAKE_CFLAGS += --coverage
#QMAKE_LFLAGS += --coverage

INCLUDEPATH += /usr/local/include

SOURCES += \
    main.c \
    brama.c \
    map.c \
    munit.c \
    tools.c

HEADERS += \
    brama.h \
    map.h \
    munit.h \
    tests_core.h \
    tests_token.h \
    tests.h \
    tools.h \
    tests_tools.h \
    brama_internal.h \
    tests_ast.h
