/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
*/

#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

/*============================================================================*
 *                               Header Files
 *============================================================================*/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup 87x3e_RING_BUFFER Ring Buffer Sets
  * @brief API sets for user application to use ring buffer
  * @{
  */

/*============================================================================*
 *                                   Types
 *============================================================================*/
/** @defgroup 87x3e_RING_BUFFER_Exported_Types Ring Buffer Sets Types
  * @{
  */

/**
 * @brief Data structure to control the ring buffer
 */
typedef struct
{
    uint8_t    *buf;  //!< memory buffer to store data
    uint32_t
    head;  //!< Indicates the distance between the current read pointer and the buffer starting address
    uint32_t
    tail;  //!< Indicates the distance between the current write pointer and the buffer starting address
    uint32_t   size;  //!< Size of memory buffer
} T_RING_BUFFER;

/** End of 87x3e_RING_BUFFER_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup 87x3e_RING_BUFFER_Exported_Functions Ring Buffer API Functions
 * @{
 */

/**
 * @brief      Initialize the ring buffer
 * @note       This API will use the ring buffer to manage an existing memory
 * @param[in]  rb    pointer to ring buffer structure
 * @param[in]  buf   an existing memory buffer
 * @param[in]  size  size of the memory buffer
 * @return     true if initialization success, false otherwise
 */
bool ring_buffer_init(T_RING_BUFFER *rb, void *buf, uint32_t size);

/**
 * @brief      Write data to ring buffer
 * @note       This API support patial write, it means that if the size of the
 *             data you write is larger than the remaining space of the ring
 *             buffer, it will write the data of the remaining space size
 * @param[in]  rb    pointer to ring buffer structure
 * @param[in]  data  data written to ring buffer
 * @param[in]  len   length of data to be written
 * @return     The length of data that successfully written to ring buffer
 */
uint32_t ring_buffer_write(T_RING_BUFFER *rb, const uint8_t *data, uint32_t len);

/**
 * @brief      Read data from ring buffer
 * @note       This API support patial read, it means that if you want to read data
 *             from ring buffer to be longer than the data in ring buffer, it will
 *             read all data in the ring buffer and return the corresponding data length.
 *             After read the data, the data will be removed from the ring buffer
 * @param[in]  rb    pointer to ring buffer structure
 * @param[in]  len   length of data to read
 * @param[out] data  data that be read from the ring buffer
 * @return     The length of data that successfully be read from ring buffer
 */
uint32_t ring_buffer_read(T_RING_BUFFER *rb, uint32_t len, uint8_t *data);

/**
 * @brief      Get the remainning space in ring buffer
 * @param[in]  rb    pointer to ring buffer structure
 * @return     The remainning space in ring buffer
 */
uint32_t ring_buffer_get_remaining_space(T_RING_BUFFER *rb);

/**
 * @brief      Peek data from ring buffer
 * @note       This API support patial peek, it means that if you want to peek data
 *             from ring buffer to be longer than the data in ring buffer, it will
 *             peek all data in the ring buffer and return the corresponding data length.
 *             Unlike read data, peek data does not remove data from ring buffer
 * @param[in]  rb    pointer to ring buffer structure
 * @param[in]  len   length of data to peek
 * @param[out] data  data that be peek from the ring buffer
 * @return     The length of data that successfully be peek from ring buffer
 */
uint32_t ring_buffer_peek(T_RING_BUFFER *rb, uint32_t len, uint8_t *data);

/**
 * @brief      Remove data from ring buffer
 * @note       This API support patial remove, it means that if you want to remove data
 *             from ring buffer to be longer than the data in ring buffer, it will
 *             remove all data in the ring buffer and return the corresponding data length
 * @param[in]  rb    pointer to ring buffer structure
 * @param[in]  len   length of data to removed
 * @return     The length of data that successfully be removed from ring buffer
 */
uint32_t ring_buffer_remove(T_RING_BUFFER *rb, uint32_t len);

/**
 * @brief      Get the length of data in the ring buffer
 * @param[in]  rb    pointer to ring buffer structure
 * @return     The length of data in ring buffer
 */
uint32_t ring_buffer_get_data_count(T_RING_BUFFER *rb);

/**
 * @brief      clear data in the ring buffer
 * @param[in]  rb    pointer to ring buffer structure
 * @return     void
 */
void ring_buffer_clear(T_RING_BUFFER *rb);

/**
 * @brief      Deinitialize the ring buffer
 * @note       This API will deinitialize the ring buffer, but you need free the
 *             memory buffer by yourself.
 * @param[in]  rb    pointer to ring buffer structure
 * @return     void
 */
void ring_buffer_deinit(T_RING_BUFFER *rb);

/** @} */ /* End of group 87x3e_RING_BUFFER_Exported_Functions */

/** @} */ /* End of group 87x3e_RING_BUFFER */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _RING_BUFFER_H_ */
