#include "tools.h"
#include "brama.h"

int string_stream_grow_buffer(t_string_stream* stream);

t_string_stream* string_stream_init_(t_context_ptr context, char* file_name, size_t line_number) {
    t_string_stream* stream = (t_string_stream*)context->malloc_func(NULL, sizeof(t_string_stream), file_name, line_number);
    stream->length          = 32;
    stream->index           = 0;
    stream->text_length     = 0;
    stream->data            = (char**)malloc(sizeof(char*) * stream->length);
    stream->context         = context;

    if (stream->data == NULL)
        return NULL;

    return stream;
}

int string_stream_add_(t_string_stream* stream, char const* data, char* file_name, size_t line_number) {
    CHECK_STREAM_PTR(stream);

    if (stream->index >= stream->length)
    {
        int status = string_stream_grow_buffer_(stream, file_name, line_number);
        if (status != STRING_STREAM_OK)
            return status;
    }

    stream->data[stream->index++] = strdup(data); // todo: not good, fix it
    stream->text_length          += strlen(data);
    return STRING_STREAM_OK;
}

int string_stream_add_char_(t_string_stream* stream, char data, char* file_name, size_t line_number) {
    CHECK_STREAM_PTR(stream);

    if (stream->index >= stream->length)
    {
        int status = string_stream_grow_buffer_(stream, file_name, line_number);
        if (status != STRING_STREAM_OK)
            return status;
    }
    
    char* tmpData                 = (char*)stream->context->malloc_func(NULL, (sizeof(char) * 2), file_name, line_number);
    tmpData[0]                    = data;
    tmpData[1]                    = '\0';
    stream->data[stream->index++] = tmpData;
    stream->text_length          += 1;
    return STRING_STREAM_OK;
}

int string_stream_grow_buffer_(t_string_stream* stream, char* file_name, size_t line_number) {
    CHECK_STREAM_PTR(stream);

    size_t tmpLength = stream->length * 2;
    char** tmpData   = (char**)stream->context->malloc_func(NULL, sizeof(char*) * tmpLength, file_name, line_number);
    if (tmpData == NULL)
        return STRING_STREAM_ERR_NO_MEMORY;

    memcpy(tmpData, stream->data, stream->length * sizeof (char*));
    stream->context->free_func(NULL, stream->data, file_name, line_number);

    stream->data   = tmpData;
    stream->length = tmpLength;
    return STRING_STREAM_OK;
}


int string_stream_get_(t_string_stream* stream, char** text, char* file_name, size_t line_number) {
    CHECK_STREAM_PTR(stream);
    
    char* tmpData         = (char*)stream->context->malloc_func(NULL, (sizeof(char) * stream->text_length) + 1, file_name, line_number);
    if (tmpData == NULL)
        return STRING_STREAM_ERR_NO_MEMORY;

    size_t index = 0;
    for (size_t i = 0; i < stream->index; ++i) {
        strcpy(tmpData + index, stream->data[i]);
        index += strlen(stream->data[i]);
    }
    tmpData[stream->text_length] = '\0';
    *text = tmpData;
    return STRING_STREAM_OK;
}

int string_stream_destroy_(t_string_stream* stream, char* file_name, size_t line_number) {
    CHECK_STREAM_PTR(stream);

    for (size_t i = 0; i < stream->index; ++i)
        stream->context->free_func(NULL, stream->data[i], file_name, line_number);

    stream->context->free_func(NULL, stream->data, file_name, line_number);
    stream->text_length = 0;
    stream->index       = 0;
    stream->length      = 0;
    stream->data        = NULL;
    return STRING_STREAM_OK;
}
