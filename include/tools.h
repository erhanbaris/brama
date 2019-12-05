#pragma once

#ifndef STRING_STREAM_H
#define STRING_STREAM_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "macros.h"

#define STRING_STREAM_OK            1
#define STRING_STREAM_ERR_NO_MEMORY 2
#define STRING_STREAM_ERR_NULL_OBJ  3

#define CHECK_STREAM_PTR(stream) if (stream == NULL) return STRING_STREAM_ERR_NULL_OBJ;
#define CHECK_VECTOR_PTR(vector) if (vector == NULL) return STRING_STREAM_ERR_NULL_OBJ;

#define string_stream_init(context)          string_stream_init_(context,          __FILE__, __LINE__)
#define string_stream_add(stream, data)      string_stream_add_(stream, data,      __FILE__, __LINE__)
#define string_stream_add_char(stream, data) string_stream_add_char_(stream, data, __FILE__, __LINE__)
#define string_stream_get(stream, data)      string_stream_get_(stream, data,      __FILE__, __LINE__)
#define string_stream_destroy(stream)        string_stream_destroy_(stream,        __FILE__, __LINE__)

typedef struct _t_context* t_context_ptr;
typedef struct {
    size_t length;
    size_t text_length;
    size_t index;
    char** data;
    t_context_ptr context;
} t_string_stream;

t_string_stream* string_stream_init_     (t_context_ptr context,                     char* file_name, size_t line_number);
int              string_stream_add_      (t_string_stream* stream, char const* data, char* file_name, size_t line_number);
int              string_stream_add_char_ (t_string_stream* stream, char data,        char* file_name, size_t line_number);
int              string_stream_get_      (t_string_stream* stream, char** data,      char* file_name, size_t line_number);
int              string_stream_destroy_  (t_string_stream* stream,                   char* file_name, size_t line_number);

#endif // STRING_STREAM_H
