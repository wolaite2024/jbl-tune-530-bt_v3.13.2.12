#ifndef _COMMON_STREAM_H_
#define _COMMON_STREAM_H_


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>


//#include "ring_buffer.h"
#include "remote.h"


#define BLUETOOTH_ADDR_LENGTH       (6)



/**
 * @brief Parameters used in notify the events of transmission stream
 */
typedef enum
{
    COMMON_STREAM_CONNECTED_EVENT,      //!< Stream has established a connection with the remote end
    COMMON_STREAM_DISCONNECTED_EVENT,   //!< Stream is disconnected with the remote end
    COMMON_STREAM_READY_TO_READ_EVENT,  //!< Stream has data that is ready be read
    COMMON_STREAM_READY_TO_WRITE_EVENT, //!< Stream's state has changed from being unable to write to being able to write
} COMMON_STREAM_EVENT;



typedef struct _COMMON_STREAM_STRUCT *COMMON_STREAM;



typedef bool (*COMMON_STREAM_READ)(COMMON_STREAM stream, uint8_t **p_buf, uint32_t *p_len);
typedef bool (*COMMON_STREAM_WRITE)(COMMON_STREAM stream, uint8_t *data, uint32_t len);
typedef void (*COMMON_STREAM_GET_ADDR)(COMMON_STREAM stream, uint8_t *addr);
typedef uint32_t (*COMMON_STREAM_GET_MTU)(COMMON_STREAM stream);
typedef bool (*COMMON_STREAM_DEL)(COMMON_STREAM stream);
typedef void (*COMMON_STREAM_CB)(COMMON_STREAM stream, COMMON_STREAM_EVENT common_event);

typedef enum
{
    COMMON_STREAM_BT_BREDR = 0,
    COMMON_STREAM_BT_LE    = 1,
    COMMON_STREAM_BT_TYPE_MAX   = 2,
} COMMON_STREAM_BT_TYPE;



typedef struct _COMMON_STREAM_STRUCT
{
    bool                                write_block;
    bool                                is_connected;
    uint16_t                            mtu;
    uint16_t                            remote_credits;
    uint8_t                            *data;
    uint32_t                            data_len;
    uint8_t                             bd_addr[BLUETOOTH_ADDR_LENGTH];
    COMMON_STREAM_BT_TYPE                      bt_type;
    COMMON_STREAM_READ                  read;
    COMMON_STREAM_WRITE                 write;
    COMMON_STREAM_GET_ADDR              get_addr;
    COMMON_STREAM_GET_MTU               get_mtu;
    COMMON_STREAM_DEL                   del;
    COMMON_STREAM_CB                    stream_cb;
} COMMON_STREAM_STRUCT;


extern const uint8_t COMMON_STREAM_TEMP_ADDR[BLUETOOTH_ADDR_LENGTH];

bool __common_stream_read(COMMON_STREAM stream, uint8_t **p_buf, uint32_t *p_len);

void __common_stream_delete(COMMON_STREAM stream);
void __common_stream_default(COMMON_STREAM stream);
bool __common_stream_check_write_condition(COMMON_STREAM stream, uint32_t len);
bool __common_stream_connect(COMMON_STREAM stream, uint32_t mtu, uint32_t remote_credits);
bool __common_stream_disconnect(COMMON_STREAM stream);
bool __common_stream_receive_data(COMMON_STREAM stream, uint8_t *data, uint32_t length);
bool __common_stream_update_credits(COMMON_STREAM stream, uint32_t remote_credits);

uint32_t __common_stream_get_bytes_available(COMMON_STREAM stream);
uint32_t __common_stream_get_mtu(COMMON_STREAM stream);
void __common_stream_get_bd_addr(COMMON_STREAM stream, uint8_t *bd_addr);
void __make_hardfault(void);


inline bool common_stream_read(COMMON_STREAM stream, uint8_t **p_buf, uint32_t *p_len);


inline bool common_stream_write(COMMON_STREAM stream, uint8_t *data, uint32_t data_len);


inline void common_stream_get_addr(COMMON_STREAM stream, uint8_t *addr);


inline uint32_t common_stream_get_mtu(COMMON_STREAM stream);


inline bool common_stream_delete(COMMON_STREAM stream);




#endif

