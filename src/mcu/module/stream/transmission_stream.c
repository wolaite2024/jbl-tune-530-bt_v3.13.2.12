#include "transmission_stream.h"
#include <stdlib.h>
#include "os_mem.h"

typedef struct T_TRANSMISSION_STREAM_STRUCT
{
    P_STREAM_WRITE               write;
    P_STREAM_READ                read;
    P_STREAM_GET_BYTES_AVAILABLE get_bytes_available;
    P_STREAM_REGISTER_CALLBACK   register_callback;
    P_STREAM_GET_MTU             get_mtu;
    P_STREAM_DEINIT              deinit;
    void                         *stream_instance;
} T_TRANSMISSION_STREAM_STRUCT;

bool transmission_stream_write(T_TRANSMISSION_STREAM stream, const uint8_t *data, uint32_t length)
{
    if ((stream == NULL) || (data == NULL))
    {
        return false;
    }
    if (stream->write == NULL)
    {
        return false;
    }

    return stream->write(stream, data, length);
}

bool transmission_stream_read(T_TRANSMISSION_STREAM stream, uint32_t length, uint8_t *buffer)
{
    if ((stream == NULL) || (buffer == NULL))
    {
        return false;
    }
    if (stream->read == NULL)
    {
        return false;
    }

    return stream->read(stream, length, buffer);
}

uint32_t transmission_stream_get_bytes_available(T_TRANSMISSION_STREAM stream)
{
    if (stream == NULL)
    {
        return 0;
    }
    if (stream->get_bytes_available == NULL)
    {
        return 0;
    }

    return stream->get_bytes_available(stream);
}

bool transmission_stream_register_callback(T_TRANSMISSION_STREAM stream,
                                           T_TRANSMISSION_STREAM_CB callback)
{
    if ((stream == NULL) || (callback == NULL))
    {
        return false;
    }
    if (stream->register_callback == NULL)
    {
        return false;
    }

    return stream->register_callback(stream, callback);
}

uint32_t transmission_stream_get_mtu(T_TRANSMISSION_STREAM stream)
{
    if (stream == NULL)
    {
        return 0;
    }
    if (stream->get_mtu == NULL)
    {
        return 0;
    }

    return stream->get_mtu(stream);
}

void transmission_stream_deinit(T_TRANSMISSION_STREAM *stream)
{
    if (*stream == NULL)
    {
        return;
    }
    if ((*stream)->deinit == NULL)
    {
        return;
    }

    (*stream)->deinit(*stream);
    *stream = NULL;
}

bool transmission_stream_set_instance_for_write(T_TRANSMISSION_STREAM stream, P_STREAM_WRITE write)
{
    if ((stream == NULL) || (write == NULL))
    {
        return false;
    }

    stream->write = write;

    return true;
}

bool transmission_stream_set_instance_for_read(T_TRANSMISSION_STREAM stream, P_STREAM_READ read)
{
    if ((stream == NULL) || (read == NULL))
    {
        return false;
    }

    stream->read = read;

    return true;
}

bool transmission_stream_set_instance_for_get_bytes_available(T_TRANSMISSION_STREAM stream,
                                                              P_STREAM_GET_BYTES_AVAILABLE get_bytes_available)
{
    if ((stream == NULL) || (get_bytes_available == NULL))
    {
        return false;
    }

    stream->get_bytes_available = get_bytes_available;

    return true;
}

bool transmission_stream_set_instance_for_register_callback(T_TRANSMISSION_STREAM stream,
                                                            P_STREAM_REGISTER_CALLBACK register_callback)
{
    if ((stream == NULL) || (register_callback == NULL))
    {
        return false;
    }

    stream->register_callback = register_callback;

    return true;
}

bool transmission_stream_set_instance_for_get_mtu(T_TRANSMISSION_STREAM stream,
                                                  P_STREAM_GET_MTU get_mtu)
{
    if ((stream == NULL) || (get_mtu == NULL))
    {
        return false;
    }

    stream->get_mtu = get_mtu;

    return true;
}

bool transmission_stream_set_instance_for_deinit(T_TRANSMISSION_STREAM stream,
                                                 P_STREAM_DEINIT deinit)
{
    if ((stream == NULL) || (deinit == NULL))
    {
        return false;
    }

    stream->deinit = deinit;

    return true;
}

bool transmission_stream_set_instance(T_TRANSMISSION_STREAM stream, void *stream_instance)
{
    if ((stream == NULL) || (stream_instance == NULL))
    {
        return false;
    }

    stream->stream_instance = stream_instance;

    return true;
}

void *transmission_stream_get_instance(T_TRANSMISSION_STREAM stream)
{
    return stream->stream_instance;
}

T_TRANSMISSION_STREAM transmission_stream_create(void)
{
    return (T_TRANSMISSION_STREAM)os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(T_TRANSMISSION_STREAM_STRUCT));
}

void transmission_stream_destroy(T_TRANSMISSION_STREAM *stream)
{
    if (*stream)
    {
        os_mem_free(*stream);
        *stream = NULL;
    }
}
