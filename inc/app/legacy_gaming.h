#ifndef __LEGACY_GAMING_H__
#define __LEGACY_GAMING_H__

typedef enum
{
    LEGACY_GAMING_STOP,
    LEGACY_GAMING_READY,
} T_LEGACY_GAMING_EVENT;

typedef enum
{
    LEGACY_GAMING_XFER_1,
    LEGACY_GAMING_XFER_2,
} T_LEGACY_GAMING_XFER_TYPE;

typedef void (*LEGACY_GAMING_EVENT_CBACK)(T_LEGACY_GAMING_EVENT event, uint8_t *addr);


/**
 * \brief   Start gaming transfer
 *
 * \param[in] addr       peer address
 * \param[in] type       gaming transfer type
 *
 * \return The result of this function.
 * \retval true     Set cmd successfully.
 * \retval false    Set cmd failed.
 */
bool legacy_gaming_xfer_start(uint8_t *addr, T_LEGACY_GAMING_XFER_TYPE type);

/**
 * \brief   Stop gaming transfer
 *
 * \param[in] addr       peer address
 * \param[in] type       gaming transfer type
 *
 * \return The result of this function.
 * \retval true     Set cmd successfully.
 * \retval false    Set cmd failed.
 */
bool legacy_gaming_xfer_stop(uint8_t *addr, T_LEGACY_GAMING_XFER_TYPE type);

/**
 * \brief   Handle gaming data exchange when dongle and headset connected
 *
 * \param[in] data       raw data to handle
 * \param[in] size       data size
 * \param[in] addr       peer addr
 *
 * \return               none
 */
void legacy_gaming_proc_data(uint8_t *data, uint16_t size, uint8_t *addr);

/**
 * \brief   Handle gaming verify procedure
 *
 * \param[in] addr       peer addr
 *
 * \return               none
 */
void legacy_gaming_verify(uint8_t *addr);

/**
 * \brief   Init legacy gaming transfer flow
 *
 * \param[in] is_dongle  is for dongle or headset
 * \param[in] cback      register gaming event callback function
 *
 * \return               none
 */
void legacy_gaming_init(bool is_dongle, LEGACY_GAMING_EVENT_CBACK cback);

#endif
