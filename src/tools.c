#include "tools.h"
#include "macros.h"

int string_stream_grow_buffer(t_string_stream* stream);

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

    if (stream->index >= stream->length)
    {
        int status = string_stream_grow_buffer(stream);
        if (status != STRING_STREAM_OK)
            return status;
    }

    stream->data[stream->index++] = strdup(data);
    stream->text_length          += strlen(data);
    return STRING_STREAM_OK;
}

int string_stream_add_char(t_string_stream* stream, char data) {
    CHECK_STREAM_PTR(stream);

    if (stream->index >= stream->length)
    {
        int status = string_stream_grow_buffer(stream);
        if (status != STRING_STREAM_OK)
            return status;
    }

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
    char** tmpData   = (char**)malloc(sizeof(char*) * tmpLength);
    if (tmpData == NULL)
        return STRING_STREAM_ERR_NO_MEMORY;

    memcpy(tmpData, stream->data, stream->length * sizeof (char*));
    free(stream->data);

    stream->data   = tmpData;
    stream->length = tmpLength;
    return STRING_STREAM_OK;
}


char* string_stream_get(t_string_stream* stream) {
    CHECK_STREAM_PTR(stream);

    char* tmpData   = (char*)malloc((sizeof(char) * stream->text_length) + 1);
    if (tmpData == NULL)
        return NULL;

    size_t index = 0;
    for (size_t i = 0; i < stream->index; ++i) {
        strcpy(tmpData + index, stream->data[i]);
        index += strlen(stream->data[i]);
    }
    tmpData[stream->text_length] = '\0';
    return tmpData;
}

int string_stream_destroy(t_string_stream* stream) {
    CHECK_STREAM_PTR(stream);

    for (size_t i = 0; i < stream->index; ++i)
        free(stream->data[i]);

    free(stream->data);
    stream->text_length = 0;
    stream->index       = 0;
    stream->length      = 0;
    stream->data        = NULL;
    return STRING_STREAM_OK;
}


/* VECTOR CODES */

t_vector* vector_init() {
    t_vector* vector = (t_vector*)malloc(sizeof(t_vector));
    vector->length   = 32;
    vector->count    = 0;
    vector->data     = (void**)malloc(sizeof(void*) * vector->length);

    if (vector->data == NULL)
        return NULL;

    return vector;
}

int vector_add(t_vector* vector, void* data) {
    CHECK_VECTOR_PTR(vector);

    if (vector->count >= vector->length)
    {
        size_t tmpLength = vector->length * 2;
        void** tmpData   = (void**)malloc(sizeof(void*) * tmpLength);
        if (tmpData == NULL)
            return STRING_STREAM_ERR_NO_MEMORY;

        memcpy(tmpData, vector->data, vector->length * sizeof (void*));
        free(vector->data);

        vector->data   = tmpData;
        vector->length = tmpLength;
    }

    vector->data[vector->count++] = data;
    return STRING_STREAM_OK;
}

void* vector_get(t_vector* vector, size_t index) {
    CHECK_VECTOR_PTR(vector);
    if (vector->count > index)
        return vector->data[index];
    return NULL;
}

int vector_destroy(t_vector* vector) {
    CHECK_VECTOR_PTR(vector);
    free(vector->data);
    vector->count  = 0;
    vector->length = 0;
    vector->data   = NULL;
    return STRING_STREAM_OK;
}
