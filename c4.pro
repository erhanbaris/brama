TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

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
