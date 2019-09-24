TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CFLAGS += -std=c11 -Wint-to-void-pointer-cast

INCLUDEPATH += /usr/local/include

SOURCES += \
    main.c \
    static_py.c \
    map.c \
    vector.c \
    munit.c

HEADERS += \
    static_py.h \
    map.h \
    vector.h \
    string_stream.h \
    munit.h \
    tests_core.h \
    tests_token.h \
    tests.h
