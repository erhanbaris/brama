#ifndef TESTS_TOOLS_H
#define TESTS_TOOLS_H

#include "tests_core.h"
#include "tests_token.h"
#include "tools.h"

MunitResult vector_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();
    munit_assert_ptr_not_null(vector);
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();
    munit_assert_int(vector->length, ==, 32);
    munit_assert_int(vector->count,  ==, 0);
    munit_assert_ptr_not_null(vector->data);
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();
    int state = vector_add(vector, "hello world");
    munit_assert_int(state, ==, STRING_STREAM_OK);
    munit_assert_int(vector->count,  ==, 1);
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();
    vector_add(vector, "hello world");
    munit_assert_string_equal((char*)vector_get(vector, 0), "hello world");
    munit_assert_int(vector->count,  ==, 1);
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();
    vector_add(vector, "erhan");
    vector_add(vector, "baris");
    munit_assert_string_equal((char*)vector_get(vector, 0), "erhan");
    munit_assert_string_equal((char*)vector_get(vector, 1), "baris");
    munit_assert_int(vector->count,  ==, 2);
    munit_assert_int(vector->length, ==, 32);
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();

    for (int i = 0; i < 1024; ++i)
        vector_add(vector, i);

    for (int i = 0; i < 1024; ++i)
        munit_assert_int((int)vector_get(vector, i), ==, i);

    munit_assert_int(vector->count,  ==, 1024);
    munit_assert_int(vector->length, ==, 1024);
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_7(const MunitParameter params[], void* user_data_or_fixture) {
    typedef struct {
        int i;
    } t_test;

    t_vector* vector = vector_init();

    for (int i = 0; i < 1024; ++i)
    {
        t_test* data = malloc(sizeof(t_test));
        data->i = i;
        vector_add(vector, data);
    }

    for (int i = 0; i < 1024; ++i)
        munit_assert_int(((t_test*)vector_get(vector, i))->i, ==, i);

    munit_assert_int(vector->count,  ==, 1024);
    munit_assert_int(vector->length, ==, 1024);

    for (int i = 0; i < 1024; ++i)
        free(vector_get(vector, i));

    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();

    for (int i = 0; i < 1024 * 1024; ++i)
        vector_add(vector, i);

    for (int i = 0; i < 1024 * 1024; ++i)
        munit_assert_int((int)vector_get(vector, i), ==, i);

    munit_assert_int(vector->count,  ==, 1024 * 1024);
    munit_assert_int(vector->length, ==, 1024 * 1024);
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_9(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();

    for (int i = 0; i < 10; ++i)
        vector_add(vector, i);

    munit_assert_ptr_null(vector_get(vector, 1024));
    munit_assert_ptr_null(vector_get(vector, -1));
    vector_destroy(vector);

    return MUNIT_OK;
}

MunitResult vector_10(const MunitParameter params[], void* user_data_or_fixture) {
    t_vector* vector = vector_init();

    for (int i = 0; i < 10; ++i)
        vector_add(vector, i);

    vector_destroy(vector);
    munit_assert_ptr_null(vector->data);
    munit_assert_int(vector->count,  ==, 0);
    munit_assert_int(vector->length, ==, 0);

    return MUNIT_OK;
}

MunitResult vector_11(const MunitParameter params[], void* user_data_or_fixture) {
    munit_assert_int(vector_add     (NULL, NULL),  ==, STRING_STREAM_ERR_NULL_OBJ);
    munit_assert_int(vector_destroy (NULL),        ==, STRING_STREAM_ERR_NULL_OBJ);
    munit_assert_int((int)vector_get(NULL, 0),     ==, STRING_STREAM_ERR_NULL_OBJ);

    return MUNIT_OK;
}

/* STRING STREAM */

MunitResult string_stream_1(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    munit_assert_ptr_not_null(stream);
    string_stream_destroy(stream);

    return MUNIT_OK;
}

MunitResult string_stream_2(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    munit_assert_int(stream->length,       ==, 32);
    munit_assert_int(stream->index,        ==, 0);
    munit_assert_int(stream->text_length,  ==, 0);
    munit_assert_ptr_not_null(stream->data);
    string_stream_destroy(stream);

    return MUNIT_OK;
}

MunitResult string_stream_3(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    int state = string_stream_add(stream, "hello world");
    munit_assert_int(state, ==, STRING_STREAM_OK);
    munit_assert_string_equal(string_stream_get(stream), "hello world");
    string_stream_destroy(stream);

    return MUNIT_OK;
}

MunitResult string_stream_4(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    string_stream_add(stream, "hello ");
    string_stream_add(stream, "world");
    munit_assert_string_equal(string_stream_get(stream), "hello world");
    string_stream_destroy(stream);

    return MUNIT_OK;
}

MunitResult string_stream_5(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    char* tmpData = malloc((sizeof(char) * (1024 * 3)) + 1);
    char key[] = "abc";

    for (size_t i = 0; i < 1024; ++i)
    {
        string_stream_add(stream, key);
        strcpy(tmpData + (i * 3), key);
    }

    tmpData[(1024 * 3)] = '\0';

    munit_assert_string_equal(string_stream_get(stream), tmpData);
    munit_assert_int         (stream->text_length, ==, (1024 * 3));
    munit_assert_int         (stream->index,       ==, 1024);
    munit_assert_int         (stream->length,      ==, 1024);
    munit_assert_ptr_not_null(stream->data);
    string_stream_destroy(stream);

    return MUNIT_OK;
}

MunitResult string_stream_6(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    char* tmpData = malloc((sizeof(char) * (1024 * 3)) + 1);
    char key[] = "abc";

    for (size_t i = 0; i < 1024; ++i)
        string_stream_add(stream, key);
    string_stream_destroy(stream);

    munit_assert_int     (stream->text_length, ==, 0);
    munit_assert_int     (stream->index,       ==, 0);
    munit_assert_int     (stream->length,      ==, 0);
    munit_assert_ptr_null(stream->data);

    return MUNIT_OK;
}

MunitResult string_stream_7(const MunitParameter params[], void* user_data_or_fixture) {
    munit_assert_int(string_stream_add     (NULL, NULL), ==, STRING_STREAM_ERR_NULL_OBJ);
    munit_assert_int(string_stream_add_char(NULL, NULL), ==, STRING_STREAM_ERR_NULL_OBJ);
    munit_assert_int((int)string_stream_get(NULL),       ==, STRING_STREAM_ERR_NULL_OBJ);
    munit_assert_int(string_stream_destroy (NULL),       ==, STRING_STREAM_ERR_NULL_OBJ);

    return MUNIT_OK;
}

MunitResult string_stream_8(const MunitParameter params[], void* user_data_or_fixture) {
    t_string_stream* stream = string_stream_init();
    char* tmpData = malloc((sizeof(char) * (1024)) + 1);
    char key[] = "a";

    for (size_t i = 0; i < 1024; ++i) {
        string_stream_add_char(stream, 'a');
        strcpy(tmpData + i, key);
    }

    tmpData[1024] = '\0';

    munit_assert_string_equal(string_stream_get(stream), tmpData);
    munit_assert_int         (stream->text_length, ==, 1024);
    munit_assert_int         (stream->index,       ==, 1024);
    munit_assert_int         (stream->length,      ==, 1024);
    munit_assert_ptr_not_null(stream->data);

    string_stream_destroy(stream);

    munit_assert_int     (stream->text_length, ==, 0);
    munit_assert_int     (stream->index,       ==, 0);
    munit_assert_int     (stream->length,      ==, 0);
    munit_assert_ptr_null(stream->data);

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

    munit_assert_string_equal(string_stream_get(stream), "hello world");
    munit_assert_int         (stream->text_length, ==, 11);
    munit_assert_int         (stream->index,       ==, 7);
    munit_assert_int         (stream->length,      ==, 32);
    munit_assert_ptr_not_null(stream->data);

    string_stream_destroy(stream);

    munit_assert_int     (stream->text_length, ==, 0);
    munit_assert_int     (stream->index,       ==, 0);
    munit_assert_int     (stream->length,      ==, 0);
    munit_assert_ptr_null(stream->data);

    return MUNIT_OK;
}

MunitTest TOOLS_TESTS[] = {
    /* vector */
    ADD_TEST(vector_1),
    ADD_TEST(vector_2),
    ADD_TEST(vector_3),
    ADD_TEST(vector_4),
    ADD_TEST(vector_5),
    ADD_TEST(vector_6),
    ADD_TEST(vector_7),
    ADD_TEST(vector_8),
    ADD_TEST(vector_9),
    ADD_TEST(vector_10),
    ADD_TEST(vector_11),

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
