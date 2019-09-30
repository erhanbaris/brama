#pragma once

#ifndef STRING_STREAM_H
#define STRING_STREAM_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STRING_STREAM_OK            0
#define STRING_STREAM_ERR_NO_MEMORY 1
#define STRING_STREAM_ERR_NULL_OBJ  2

#define CHECK_STREAM_PTR(stream) if (stream == NULL) return STRING_STREAM_ERR_NULL_OBJ;
#define CHECK_VECTOR_PTR(vector) if (vector == NULL) return STRING_STREAM_ERR_NULL_OBJ;

typedef struct {
    size_t length;
    size_t text_length;
    size_t index;
    char** data;
} t_string_stream;

t_string_stream* string_stream_init       ();
int              string_stream_add        (t_string_stream* stream, char const* data);
int              string_stream_add_char   (t_string_stream* stream, char data);
char*            string_stream_get        (t_string_stream* stream);
int              string_stream_destroy    (t_string_stream* stream);

/* VECTOR START --> */

typedef struct {
    size_t length;
    size_t count;
    void** data;
} t_vector;

t_vector* vector_init   ();
int       vector_add    (t_vector* vector, void* data);
void*     vector_get    (t_vector* vector, size_t index);
int       vector_destroy(t_vector* vector);


/* <-- VECTOR END */

#endif // STRING_STREAM_H
