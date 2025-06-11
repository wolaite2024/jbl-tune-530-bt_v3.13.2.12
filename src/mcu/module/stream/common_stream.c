
#include <string.h>


#include "common_stream.h"
#include "trace.h"
#include "os_mem.h"

const uint8_t COMMON_STREAM_TEMP_ADDR[BLUETOOTH_ADDR_LENGTH] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

void __common_stream_delete(COMMON_STREAM stream)
{
    return;
}


void __common_stream_default(COMMON_STREAM stream)
{
    stream->is_connected = false;
    stream->mtu = 0;
    stream->data = NULL;
    stream->data_len = 0;
    stream->remote_credits = 0;
    stream->write_block  = false;
    stream->read = NULL;
    stream->write = NULL;
    stream->get_addr = NULL;
    stream->get_mtu = NULL;
    stream->del = NULL;
    stream->stream_cb = NULL;
    stream->bt_type = COMMON_STREAM_BT_BREDR;

    memcpy(stream->bd_addr, COMMON_STREAM_TEMP_ADDR, sizeof(stream->bd_addr));
}


bool __common_stream_check_write_condition(COMMON_STREAM stream, uint32_t len)
{
    if (true != stream->is_connected)
    {
        APP_PRINT_ERROR0("__common_stream_check_write_condition: spp stream not connected");
        return false;
    }

    if (len > stream->mtu)
    {
        APP_PRINT_ERROR0("__common_stream_check_write_condition: data length is longer than mtu");
        return false;
    }

    if (stream->remote_credits == 0)
    {
        APP_PRINT_ERROR0("__common_stream_check_write_condition: have no credits");
        stream->write_block = true;
        return false;
    }

    return true;
}


bool __common_stream_connect(COMMON_STREAM stream, uint32_t mtu, uint32_t remote_credits)
{
    if (stream->is_connected != false)
    {
        APP_PRINT_ERROR0("__common_stream_connect: cannot connect twice");
        return false;
    }

    stream->remote_credits = remote_credits;
    stream->mtu            = mtu;
    stream->is_connected      = true;
    stream->write_block    = false;

    COMMON_STREAM_CB stream_cb = stream->stream_cb;

    if (stream_cb)
    {
        stream_cb(stream, COMMON_STREAM_CONNECTED_EVENT);
    }

    return true;
}


bool __common_stream_disconnect(COMMON_STREAM stream)
{
    if (stream->is_connected != true)
    {
        APP_PRINT_ERROR0("__common_stream_disconnect: cannot disconnect a not connecting stream");
        return false;
    }

    stream->mtu = 0;

    stream->remote_credits = 0;
    stream->is_connected = false;
    stream->write_block = false;

    COMMON_STREAM_CB stream_cb = stream->stream_cb;

    if (stream_cb)
    {
        stream_cb(stream, COMMON_STREAM_DISCONNECTED_EVENT);
    }

    return true;
}


bool __common_stream_receive_data(COMMON_STREAM stream, uint8_t *data, uint32_t length)
{
    if (stream->is_connected != true)
    {
        APP_PRINT_ERROR0("__common_stream_receive_data: stream_state should be STREAM_CONNECTED");
        return false;
    }

    stream->data = data;
    stream->data_len = length;

    COMMON_STREAM_CB stream_cb = stream->stream_cb;

    if (stream_cb)
    {
        stream_cb(stream, COMMON_STREAM_READY_TO_READ_EVENT);
    }

    return true;
}


bool __common_stream_update_credits(COMMON_STREAM stream, uint32_t remote_credits)
{
    if (stream->is_connected != true)
    {
        APP_PRINT_ERROR0("__common_stream_update_credits: stream_state should be STREAM_CONNECTED");
        return false;
    }

    stream->remote_credits = remote_credits;

    if (stream->write_block != true)
    {
        return false;
    }

    stream->write_block = false;

    COMMON_STREAM_CB stream_cb = stream->stream_cb;

    if (stream_cb)
    {
        stream_cb(stream, COMMON_STREAM_READY_TO_WRITE_EVENT);
    }

    return true;
}


bool __common_stream_read(COMMON_STREAM stream, uint8_t **p_buf, uint32_t *p_len)
{
    if (true != stream->is_connected)
    {
        return false;
    }

    *p_buf = stream->data;
    *p_len = stream->data_len;

    stream->data = NULL;
    stream->data_len = 0;

    return true;
}


uint32_t __common_stream_get_mtu(COMMON_STREAM stream)
{
    return stream->mtu;
}


void __common_stream_get_bd_addr(COMMON_STREAM stream, uint8_t *bd_addr)
{
    memcpy(bd_addr, stream->bd_addr, BLUETOOTH_ADDR_LENGTH);
}


void __make_hardfault(void)
{
    uint32_t *p = (uint32_t *)0xBBBBBBBB;
    *p = 0x12345678;
}


bool common_stream_read(COMMON_STREAM stream, uint8_t **p_buf, uint32_t *p_len)
{
    if (stream == NULL)
    {
        return false;
    }

    if (stream->read == NULL)
    {
        return false;
    }

    return stream->read(stream, p_buf, p_len);
}


bool common_stream_write(COMMON_STREAM stream, uint8_t *data, uint32_t data_len)
{
    if (stream == NULL)
    {
        return false;
    }

    if (stream->write == NULL)
    {
        return false;
    }

    return stream->write(stream, data, data_len);
}


void common_stream_get_addr(COMMON_STREAM stream, uint8_t *addr)
{
    if (stream == NULL)
    {
        return;
    }

    if (stream->get_addr == NULL)
    {
        return;
    }

    stream->get_addr(stream, addr);
}



uint32_t common_stream_get_mtu(COMMON_STREAM stream)
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


bool common_stream_delete(COMMON_STREAM stream)
{
    if (stream == NULL)
    {
        return false;
    }

    if (stream->del == NULL)
    {
        return false;
    }

    return stream->del(stream);
}
