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

typedef struct _t_context* t_context_ptr;
typedef struct {
    size_t length;
    size_t text_length;
    size_t index;
    char** data;
    t_context_ptr context;
} t_string_stream;

t_string_stream* string_stream_init (t_context_ptr context);

int string_stream_add        (t_string_stream* stream, char const* data);
int string_stream_add_char   (t_string_stream* stream, char data);
int string_stream_get        (t_string_stream* stream, char** data);
int string_stream_destroy    (t_string_stream* stream);

#endif // STRING_STREAM_H
