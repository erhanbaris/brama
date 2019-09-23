#ifndef STRING_STREAM_H
#define STRING_STREAM_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define STRING_STREAM_OK            0
#define STRING_STREAM_ERR_NO_MEMORY 1
#define STRING_STREAM_ERR_NULL_OBJ  2

#define CHECK_STREAM_PTR(stream) if (stream == NULL) return STRING_STREAM_ERR_NULL_OBJ;

typedef struct {
    size_t length;
    size_t text_length;
    size_t index;
    char** data;
} t_string_stream;

t_string_stream* string_stream_init       ();
int              string_stream_add        (t_string_stream* stream, char const* data);
int              string_stream_add_char   (t_string_stream* stream, char data);
int              string_stream_grow_buffer(t_string_stream* stream);
char*            string_stream_get        (t_string_stream* stream);
int              string_stream_destroy    (t_string_stream* stream);

t_string_stream* string_stream_init() {
    t_string_stream* stream = (t_string_stream*)malloc(sizeof(t_string_stream));
    stream->length          = 32;
    stream->index           = 0;
    stream->text_length     = 0;
    stream->data            = (char**)malloc(sizeof(char*) * stream->length);

    if (stream->data == NULL)
        return NULL;

    return stream;
}

int string_stream_add(t_string_stream* stream, char const* data) {
    CHECK_STREAM_PTR(stream);

    if (stream->index >= stream->index)
        string_stream_grow_buffer(stream);

    stream->data[stream->index++] = strdup(data);
    stream->text_length          += strlen(data);
    return STRING_STREAM_OK;
}

int string_stream_add_char(t_string_stream* stream, char data) {
    CHECK_STREAM_PTR(stream);

    if (stream->index >= stream->index)
        string_stream_grow_buffer(stream);

    char* tmpData                 = (char*)malloc(sizeof(char) * 2);
    tmpData[0]                    = data;
    tmpData[1]                    = '\0';
    stream->data[stream->index++] = tmpData;
    stream->text_length          += 1;
    return STRING_STREAM_OK;
}

int string_stream_grow_buffer(t_string_stream* stream) {
    CHECK_STREAM_PTR(stream);

    size_t tmpLength = stream->length * 2;
    char** tmpData   = (char**)malloc(sizeof(char*) * stream->length);
    if (tmpData == NULL)
        return STRING_STREAM_ERR_NO_MEMORY;

    memcpy(tmpData, stream->data, stream->length);
    free(stream->data);

    stream->data   = tmpData;
    stream->length = tmpLength;
    return STRING_STREAM_OK;
}


char* string_stream_get(t_string_stream* stream) {
    CHECK_STREAM_PTR(stream);

    char* tmpData   = (char*)malloc(sizeof(char) * stream->text_length);
    if (tmpData == NULL)
        return NULL;

    size_t index = 0;
    for (size_t i = 0; i < stream->index; ++i) {
        strcpy(tmpData + index, stream->data[i]);
        index += strlen(stream->data[i]);
    }
    return tmpData;
}

int string_stream_destroy(t_string_stream* stream) {
    CHECK_STREAM_PTR(stream);

    for (size_t i = 0; i < stream->index; ++i)
        free(stream->data[i]);
    return STRING_STREAM_OK;
}

#endif // STRING_STREAM_H
