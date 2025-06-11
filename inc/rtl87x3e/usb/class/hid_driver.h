#ifndef __HID_DRIVER_H__
#define __HID_DRIVER_H__
#include <stdbool.h>
#include <stdint.h>

/** @defgroup 87x3e_HID_Driver Hid Driver
  * @brief app usb module.
  * @{
  */


/**
 * hid_driver.h
 *
 * \brief   HID pipe attribute zero length packet
 *
 * \details If ZLP attribute is set, hid will send zero length packet if data length
 *          is equal to ep maxpacket size
 *
 */
#define HID_PIPE_ATTR_ZLP   0x0001

typedef uint32_t (*UINT_IN_FUNC)(uint8_t, void *, uint16_t *);
typedef uint32_t (*UINT_OUT_FUNC)(void *, uint16_t);
typedef uint32_t (*VOID_COMPLETE_FUNC)(void *, uint32_t, uint8_t *);



/**
 * hid_driver.h
 *
 * \brief   HID driver callback
 *
 * \param  get_report this api will be called when receiving HID GET_REPORT request
 * \param  set_report this api will be called when receiving HID SET_REPORT request
 *
 */
typedef struct _hid_driver_cbs
{
    UINT_IN_FUNC    get_report;
    UINT_OUT_FUNC   set_report;
} T_HID_DRIVER_CBS;

/**
 * hid_driver.h
 *
 * \brief   HID interface descriptor register
 *
 * \param[in]  desc HID interface descriptor
 *
 */
void hid_driver_if_desc_register(void *desc);

/**
 * hid_driver.h
 *
 * \brief   HID report descriptor register
 *
 * \param[in]  owner HID interface descriptor the report descriptor belongs to
 * \param[in]  desc HID report descriptor
 * \param[in]  desc length of HID report descriptor
 *
 */
void hid_driver_report_desc_register(void *owner, void *desc, uint16_t len);

/**
 * hid_driver.h
 *
 * \brief   open hid data pipe
 *
 * \param[in]  ep_addr ep address
 * \param[in]  attr HID pipe attribute such as zlp .etc
 *
 */
void *hid_driver_data_pipe_open(uint8_t ep_addr, uint16_t attr);

/**
 * hid_driver.h
 *
 * \brief   hid pipe send data
 *
 * \param[in]  ep_addr ep address
 * \param[in]  attr HID pipe attribute such as zlp .etc
 * \param[in]  cb callback which will be called after data is sent over
 *
 */
bool hid_driver_data_pipe_send(void *handle, void *buf, uint32_t len, VOID_COMPLETE_FUNC cb);

/**
 * hid_driver.h
 *
 * \brief   register hid driver callbacks to process custom SET_REPORT/GET_REPORT request
 *
 * \param[in]  ep_addr ep address
 * \param[in]  attr HID pipe attribute such as zlp .etc
 *
 */
void hid_driver_cbs_register(T_HID_DRIVER_CBS cbs);


/** @}*/
/** End of 87x3e_HID_Driver
*/
#endif
