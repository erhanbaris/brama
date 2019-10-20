#ifndef TESTS_TOOLS_H
#define TESTS_TOOLS_H

#include "tests_core.h"
#include "tests_token.h"
#include "tools.h"

/* STRING STREAM */

MunitResult string_stream_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    munit_assert_ptr_not_null(stream);
    string_stream_destroy(stream);
    BRAMA_FREE(stream);

    return MUNIT_OK;
}

MunitResult string_stream_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    munit_assert_int(stream->length,       ==, 32);
    munit_assert_int(stream->index,        ==, 0);
    munit_assert_int(stream->text_length,  ==, 0);
    munit_assert_ptr_not_null(stream->data);
    string_stream_destroy(stream);
    BRAMA_FREE(stream);

    return MUNIT_OK;
}

MunitResult string_stream_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    int state = string_stream_add(stream, "hello world");
    munit_assert_int(state, ==, STRING_STREAM_OK);
    char_ptr result = NULL;
    string_stream_get(stream, &result);
    munit_assert_string_equal(result, "hello world");
    string_stream_destroy(stream);
    BRAMA_FREE(stream);
    BRAMA_FREE(result);

    return MUNIT_OK;
}

MunitResult string_stream_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    string_stream_add(stream, "hello ");
    string_stream_add(stream, "world");
    char_ptr result = NULL;
    string_stream_get(stream, &result);
    munit_assert_string_equal(result, "hello world");
    string_stream_destroy(stream);
    BRAMA_FREE(stream);
    BRAMA_FREE(result);

    return MUNIT_OK;
}

MunitResult string_stream_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    char* tmpData = (char*)malloc((sizeof(char) * (1024 * 3)) + 1);
    char key[] = "abc";

    for (size_t i = 0; i < 1024; ++i)
    {
        string_stream_add(stream, key);
        strcpy(tmpData + (i * 3), key);
    }

    tmpData[(1024 * 3)] = '\0';
    char_ptr result = NULL;
    string_stream_get(stream, &result);
    munit_assert_string_equal(result, tmpData);
    munit_assert_int         (stream->text_length, ==, (1024 * 3));
    munit_assert_int         (stream->index,       ==, 1024);
    munit_assert_int         (stream->length,      ==, 1024);
    munit_assert_ptr_not_null(stream->data);
    string_stream_destroy(stream);
    BRAMA_FREE(stream);
    BRAMA_FREE(tmpData);
    BRAMA_FREE(result);

    return MUNIT_OK;
}

MunitResult string_stream_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    char* tmpData = (char*)malloc((sizeof(char) * (1024 * 3)) + 1);
    char key[] = "abc";

    for (size_t i = 0; i < 1024; ++i)
        string_stream_add(stream, key);
    string_stream_destroy(stream);

    munit_assert_int     (stream->text_length, ==, 0);
    munit_assert_int     (stream->index,       ==, 0);
    munit_assert_int     (stream->length,      ==, 0);
    munit_assert_ptr_null(stream->data);
    BRAMA_FREE(stream);
    BRAMA_FREE(tmpData);

    return MUNIT_OK;
}

MunitResult string_stream_7(const MunitParameter params[], void* user_data_or_fixture) {
    munit_assert_int(string_stream_add     (NULL, NULL), ==, STRING_STREAM_ERR_NULL_OBJ);
    munit_assert_int(string_stream_add_char(NULL, NULL), ==, STRING_STREAM_ERR_NULL_OBJ);

    munit_assert_int(string_stream_get(NULL, NULL),      ==, STRING_STREAM_ERR_NULL_OBJ);
    munit_assert_int(string_stream_destroy (NULL),       ==, STRING_STREAM_ERR_NULL_OBJ);

    return MUNIT_OK;
}

MunitResult string_stream_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    char* tmpData = (char*)malloc((sizeof(char) * (1024)) + 1);
    char key[] = "a";

    for (size_t i = 0; i < 1024; ++i) {
        string_stream_add_char(stream, 'a');
        strcpy(tmpData + i, key);
    }

    tmpData[1024] = '\0';

    char_ptr result = NULL;
    string_stream_get(stream, &result);
    munit_assert_string_equal(result, tmpData);
    munit_assert_int         (stream->text_length, ==, 1024);
    munit_assert_int         (stream->index,       ==, 1024);
    munit_assert_int         (stream->length,      ==, 1024);
    munit_assert_ptr_not_null(stream->data);

    string_stream_destroy(stream);

    munit_assert_int     (stream->text_length, ==, 0);
    munit_assert_int     (stream->index,       ==, 0);
    munit_assert_int     (stream->length,      ==, 0);
    munit_assert_ptr_null(stream->data);
    BRAMA_FREE(stream);
    BRAMA_FREE(tmpData);
    BRAMA_FREE(result);

    return MUNIT_OK;
}

MunitResult string_stream_9(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();

    string_stream_add_char(stream, 'h');
    string_stream_add_char(stream, 'e');
    string_stream_add_char(stream, 'l');
    string_stream_add_char(stream, 'l');
    string_stream_add_char(stream, 'o');
    string_stream_add_char(stream, ' ');

    string_stream_add     (stream, "world");

    char_ptr result = NULL;
    string_stream_get(stream, &result);
    munit_assert_string_equal(result, "hello world");
    munit_assert_int         (stream->text_length, ==, 11);
    munit_assert_int         (stream->index,       ==, 7);
    munit_assert_int         (stream->length,      ==, 32);
    munit_assert_ptr_not_null(stream->data);

    string_stream_destroy(stream);

    munit_assert_int     (stream->text_length, ==, 0);
    munit_assert_int     (stream->index,       ==, 0);
    munit_assert_int     (stream->length,      ==, 0);
    munit_assert_ptr_null(stream->data);
    BRAMA_FREE(stream);
    BRAMA_FREE(result);

    return MUNIT_OK;
}

MunitTest TOOLS_TESTS[] = {
    /* string stream */
    ADD_TEST(string_stream_1),
    ADD_TEST(string_stream_2),
    ADD_TEST(string_stream_3),
    ADD_TEST(string_stream_4),
    ADD_TEST(string_stream_5),
    ADD_TEST(string_stream_6),
    ADD_TEST(string_stream_7),
    ADD_TEST(string_stream_8),
    ADD_TEST(string_stream_9),
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

#endif // TESTS_TOOLS_H
