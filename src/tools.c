#include "tools.h"
#include "brama.h"

int string_stream_grow_buffer(t_string_stream* stream);

t_string_stream* string_stream_init(t_context_ptr context) {
    t_string_stream* stream = (t_string_stream*)malloc(sizeof(t_string_stream));
    stream->length          = 32;
    stream->index           = 0;
    stream->text_length     = 0;
    stream->data            = (char**)malloc(sizeof(char*) * stream->length);
    stream->context         = context;

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
    
    char* tmpData                 = (char*)malloc((sizeof(char) * 2));
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


int string_stream_get(t_string_stream* stream, char** text) {
    CHECK_STREAM_PTR(stream);
    
    char* tmpData         = (char*)malloc((sizeof(char) * stream->text_length) + 1);
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
