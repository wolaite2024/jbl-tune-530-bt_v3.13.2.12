/**
 * @copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * @file hal_usb.h
 * @version 1.0
 * @brief
 *
 * @note:
 */
#ifndef __HAL_USB__
#define __HAL_USB__
#include <stdint.h>

/**
 * @addtogroup USB_HAL
 * @{
 * This section introduce definitions and usage of the USB HAL APIs.
 *
 * |Terms         |Details                                               |
 * |--------------|------------------------------------------------------|
 * |\b HAL        |Hardware Abstraction Layer                            |
 * |\b urb        |USB request block                                     |
 *@}
 */
/**
 * @addtogroup USB_HAL
 * @{
 *
 * @section HAL_Usage_Chapter How to use USB HAL
 *
 * - \b Hardware \b setup. \n
 *  -step1: Call #hal_usb_speed_set(speed) to set USB target speed
 *  - step2: Call #hal_usb_init() to initialize hal software resource. \n
 *  - step2: Call #hal_usb_phy_power_on() to power on USB PHY
 *  - step3: Call #hal_usb_mac_init() to initialize USB mac
 * @par Example
 * @code
 *      void usb_hw_init(void)
 *      {
 *          hal_usb_speed_set(HAL_USB_SPEED_HIGH);
 *          hal_usb_init();
 *          hal_usb_phy_power_on();
 *          hal_usb_mac_init();
 *      }
 * @endcode
 *
 * - \b Prepare \b to \b enum. \n
 *  -step1: Setup hardware as above. \n
 *  - step2: Realize hal usb composite driver. \n
 *  - step3: Create task to handle interrupts. \n
 *  - step4: Initialize interrupts. \n
 *  - step5: USB soft connect. \n
 * @par Example
 * @code
 *
 *      uint8_t isr_pending = 0;
 *      void usb_common_isr_handler(T_HAL_USB_COMMON_ISR isr, T_HAL_USB_ISR_PARAM *param)
 *      {
 *          if process in task
 *          {
 *              isr_pending++;
 *              //send msg to usb irq task
 *          }
 *      }
 *
 *      void usb_common_isr_enter(void)
 *      {
 *          NVIC_DisableIRQ(USB_IP_IRQn);
 *      }
 *
 *      void usb_common_isr_exit(void)
 *      {
 *          if(isr_pending == 0)
 *          {
 *              NVIC_EnableIRQ(USB_IP_IRQn);
 *          }
 *      }
 *
 *      HAL_USB_COMMON_ISR_HOOKS common_isr_hooks =
 *      {
 *          .enter = usb_common_isr_enter,
 *          .handler = usb_common_isr_handler,
 *          .exit = usb_common_isr_exit,
 *      }
 *      void usb_isr_proc_task(void *)
 *      {
 *          while(1)
 *          {
 *              if receive msg from usb common isr
 *              {
 *                  process common isr
 *              }
 *              isr_pending--;
 *              if(isr_pending == 0)
 *              {
 *                  NVIC_EnableIRQ(USB_IP_IRQn);
 *              }
 *          }
 *      }
 *
 *      void usb_isr_proc_task_create(void)
 *      {
 *          //create task usb_isr_proc_task
 *      }
 *
 *      void usb_isr_init(void)
 *      {
 *          RamVectorTableUpdate(USB_IP_VECTORn, usb_common_isr_handler);
 *          NVIC_SetPriority(USB_IP_IRQn, 4);
 *          NVIC_EnableIRQ(USB_IP_IRQn);
 *      }
 *
 *      void usb_start(void)
 *      {
 *          usb_hw_init();//Setup hardware
 *          usb_isr_proc_task_create();//Create task to handle interrupts
 *          usb_isr_init();//Initialize interrupts
 *          hal_usb_soft_attach();//USB soft connect
 *
 *          os scheduler start
 *      }
 * @endcode
 *
 * - \b Endpoint \b setup. \n
 *   The endpoint should be enable when receive SET_ALT request and the alternate setting contains the endpoint. \n
 *   Please refer to "Prepare to enum" to know how to receive setup packets. \n
 * @par Example
 * @code
 *      int setup_cb(uint8_t *data)
 *      {
 *         if setup request is SET_ALT
 *         {
 *              void *ep_handle = hal_usb_ep_handle_get(ep addr);
 *              if alt value is m and alt setting m contains the endpoint epN
 *              {
 *                  hal_usb_ep_enable(ep_handle , ep desc);
 *
 *                  // \b bulk \b & \b interrupt \b endpoints
 *                  if endpoint direction is OUT
 *                  //alloc urb
 *                  T_HAL_USB_REQUEST_BLOCK *ep_urb = hal_usb_urb_alloc(len);
 *                  ...init urb...(refer to "Data transfer" in \ref HAL_Usage_Chapter)
 *                  //prepare to receive data
 *                  hal_usb_ep_rx(ep_handle, ep_urb);
 *
 *                  // \b isochronous \b endpoints
 *                  //alloc urb
 *                  T_HAL_USB_ISO_REQUEST_BLOCK *ep_urb = hal_usb_iso_urb_alloc(len);
 *                  ...init urb...(refer to "Data transfer" in \ref HAL_Usage_ChapterK)
 *                  //start transfer--For both send & receive
 *                  hal_usb_iso_ep_start(ep_handle, ep_urb);
 *              }
 *              else
 *              {
 *                  // \b isochronous \b endpoints
 *                  hal_usb_iso_ep_stop(ep_handle, ep_urb);
 *
 *                  // \b all \b endpoints
 *                  hal_usb_ep_disable(ep_handle);
 *              }
 *         }
 *      }
 * @endcode
 *
 * - \b Data \b transfer. \n
 *  Data transfer entity is defined in \ref T_HAL_USB_REQUEST_BLOCK for control/interrupt/bulk transfers, \n
 *  and \ref T_HAL_USB_ISO_REQUEST_BLOCK for isochronous transfer. \n
 *  To setup data transfer:
 *  - step1: setup endpoint, (refer to "Endpoint setup" in \ref HAL_Usage_Chapter)
 *  - step2: alloc urb
 *  - step2: start transfer
 * @par Example
 * @code
 *      // \b Control \b transfer
 *
 *      //enable ep0
 *      void *ep0_handle = hal_usb_ep_handle_get(0);
 *      hal_usb_ep_enable(ep0_handle , NULL);
 *
 *      int ep0_request_complete(T_HAL_USB_REQUEST_BLOCK *urb)
 *      {
 *          //for OUT data stage
 *          process data in urb->buf
 *      }
 *
 *      //alloc urb
 *      T_HAL_USB_REQUEST_BLOCK *ep0_urb = hal_usb_urb_alloc(len);
 *      ep0_urb->length = length of data to transfer
 *      memcpy(ep0_urb->buf, data, ep0_urb->length)
 *      ep0_urb->complete = ep0_request_complete;
 *      ep0_urb->ep_handle = ep0_handle;
 *
 *      //start transfer--For both send & receive
 *      //hal_usb_ep0_trx MUST be called in setup callback in \ref T_HAL_USB_DRIVER
 *      //for tx, if ep0_urb->length is 0, it means send ACK to host
 *      hal_usb_ep0_trx(ep0_handle, ep0_urb);
 *
 *     // \b bulk \b & \b interrupt \b transfer
 *
 *     //enable ep
 *     void *ep_handle = hal_usb_ep_handle_get(ep addr);
 *     hal_usb_ep_enable(ep_handle , ep desc);
 *
 *     int ep_request_complete(T_HAL_USB_REQUEST_BLOCK *urb)
 *     {
 *          //for OUT data stage
 *          process data in urb->buf
 *          ...reinit urb if needed...
 *          hal_usb_ep_rx(ep_handle, ep_urb);
 *
 *     }
 *
 *     //alloc urb
 *     T_HAL_USB_REQUEST_BLOCK *ep_urb = hal_usb_urb_alloc(len);
 *     ep_urb->complete = ep_request_complete;
 *     ep_urb->ep_handle = ep_handle;
 *     ep_urb->buf_proc_intrvl = process interval;
 *     ep_urb->data_per_frame = data length per host polling;
 *
 *     //start transfer
 *     //send
 *     if support zero length packet
 *     {
 *          ep_urb->zlp = (ep max packet size ==  ep_urb->length);
 *     }
 *     else
 *     {
 *          ep_urb->zlp = 0;
 *     }
 *     hal_usb_ep_tx(ep_handle, ep_urb);
 *     //Receive
 *     hal_usb_ep_rx(ep_handle, ep_urb);
 *
 *     // \b isochronous \b transfer
 *
 *     int iso_ep_request_complete(T_HAL_USB_ISO_REQUEST_BLOCK *urb, uint8_t proc_buf_num)
 *     {
 *          T_HAL_USB_ISO_PKT_INFO *iso_pkt = NULL;
 *          uint8_t *buf = NULL;
 *          uint8_t pkt_cnt = 0;
 *
 *          if(proc_buf_num == 0)
 *          {
 *              iso_pkt = urb->iso_pkt0;
 *              buf = urb->buf0;
 *          }
 *          else
 *          {
 *              iso_pkt = urb->iso_pkt1;
 *              buf = urb->buf1;
 *          }
 *          pkt_cnt = iso_pkt->pkt_cnt;
 *          for(uint8_t i = 0; i < pkt_cnt; i++)
 *          {
 *              if(iso_pkt[i].status == 0)
 *              {
 *              //process data store in buf + iso_pkt[i].offset,
 *                and data length is  iso_pkt[i].actual
 *              }
 *          }
 *          return 0;
 *     }
 *
 *     //enable ep
 *     void *ep_handle = hal_usb_ep_handle_get(ep addr);
 *     hal_usb_ep_enable(ep_handle , ep desc);
 *
 *     //alloc urb
 *     T_HAL_USB_ISO_REQUEST_BLOCK *iso_ep_urb = hal_usb_iso_urb_alloc(len);
 *     iso_ep_urb->length = length of data to transfer
 *     memcpy(iso_ep_urb->buf, data, iso_ep_urb->length)
 *     iso_ep_urb->complete = iso_ep_request_complete;
 *     iso_ep_urb->ep_handle = ep_handle;
 *
 *     //start transfer--For both send & receive
 *     hal_usb_iso_ep_start(ep_handle, iso_ep_urb);
 * @endcode
 *
 * - \b Power \b manager. \n
 *  To reduce power dissipation, the USB PHY will partially power down when device suspend by calling \ref hal_usb_suspend_enter in \n
 *  \ref HAL_USB_COMMON_ISR_SUSPEND. If exit suspend state, suspendn interrupt will be triggered, and \ref hal_usb_suspend_exit SHOULD be called
 *  in suspendn interrupt, and then \ref HAL_USB_COMMON_ISR_RESUME will be triggered.
 * @par Example
 * @code
 *
 *      void usb_common_isr_handler(T_HAL_USB_COMMON_ISR isr, T_HAL_USB_ISR_PARAM *param)
 *      {
 *          if (isr == HAL_USB_COMMON_ISR_SUSPEND)
 *          {
 *              hal_usb_suspend_enter();
 *          }
 *      }
 *
 *     void usb_suspendn_isr_handler(void)
 *     {
 *        hal_usb_suspend_exit();
 *     }
 *
 *     HAL_USB_SUSPENDN_ISR_HOOKS usb_suspendn_isr_hooks =
 *     {
 *          .enter = NULL,
 *          .handler = usb_suspendn_isr_handler,
 *          .exit = NULL,
 *      };
 *      enable suspendn interrupt
 *      hal_usb_suspendn_isr_handler_update(&usb_suspendn_isr_hooks);
 * @endcode
 */
/** @}*/

/**
 * @addtogroup USB_HAL
 * @{
 * @section Definitions
 */
/**
* @brief USB Interrupt type
* @details USB_ISR_TYPE_COMMON: ALL USB interrupt except isochronous interrupt that indicates \n
*                               data stage of isochronous transfer is complete, it indicates the interrupt \ref USB_IP_IRQn \n
*          USB_ISR_TYPE_ISOC:isochronous interrupt that indicates data stage of isochronous transfer is complete, \n
*                            it indicates the interrupt \ref USB_ISOC_IRQn
*
*/

/**
 * @brief USB speed
 *
 */
typedef enum {HAL_USB_SPEED_FULL, HAL_USB_SPEED_HIGH, HAL_USB_SPEED_UNSUPPORTED} T_HAL_USB_SPEED;

/**
 * @brief USB request block--USB data transfer entity
 *
 * @param length length of data that will be sent or received
 * @param actual actual length of data that has been sent or received
 * @param buf buffer that stores data will be sent or has been received
 * @param zlp if send zero length packet when length is equal to maxpacketsize of the endpoint
 * @param complete_in_isr 1: \ref complete callback will be called in ISR. \n
 *                    0: \ref complete callback will not be called in ISR, and the urb will be passed through in \ref HAL_USB_COMMON_ISR_XFER_DONE.
 * @param status if data has been transferred successfully
 * @param ep_handle handle of endpoint that will transfer data
 * @param complete this callback will be called when data that has been sent or received. \n
 *                 For sending data, this callback is mainly used to indicate the result of data sending. \n
 *                 For receiving data, this callback is used to get the data already received and related information.
 * @param priv private data
 *
 */
typedef struct _hal_usb_request_block
{
    int length;
    int actual;
    uint8_t *buf;

    uint8_t zlp: 1;
    uint8_t complete_in_isr: 1;
    uint8_t rsv: 6;

    int status;
    void *ep_handle;
    int (*complete)(struct _hal_usb_request_block *urb);

    void *priv;
} T_HAL_USB_REQUEST_BLOCK;

/**
 * @brief USB isochronous packet information
 * @details For isochronous transfer, multiple packets can be sent once, for example, if usb speed is high speed, \n
 *          and bInterval of isochronous endpoint is 3, host will send TOKEN packet per 0.5ms, but upper application \n
 *          may prepare data per 2 ms. In this case, upper application can process 4 packets once.
 *
 * @param offset offset of current packet in \ref buf of \ref T_HAL_USB_ISO_REQUEST_BLOCK
 * @param actual actual length of current packet
 * @param status status of current packet transferred
 *
 */
typedef struct _hal_usb_iso_pkt_info
{
    uint32_t offset;
    uint32_t actual;
    int status;
} T_HAL_USB_ISO_PKT_INFO;

/**
 * @brief USB isochronous request block
 * @details For isochronous transfer, ping-pong buffers are used to transfer data, for example, if current data transfer uses buf0, \n
 *          the next data transfer will use buf1, and vice versa.
 *          Data process interval depends on both bInterval in endpoint descriptor and buf_proc_intrvl: 2^(bInterval - 1) * buf_proc_intrvl *125us
 *
 * @param buf0 buffer 0 of ping-pong buffers
 * @param buf1 buffer 1 of ping-pong buffers
 * @param data_per_frame data length per host polling
 * @param buf_proc_intrvl number of host polling per data process
 * @param pkt_cnt packet number per data process
 * @param iso_pkt0 packets information stored in \ref buf0, refer to \ref T_HAL_USB_ISO_PKT_INFO
 * @param iso_pkt1 packets information stored in \ref buf1, refer to \ref T_HAL_USB_ISO_PKT_INFO
 * @param ep_handle handle of endpoint that will transfer data
 * @param complete this callback will be called when data that has been sent or received. \n
 *                 For sending data, upper application SHOULD prepare data in buf indexed by \ref proc_buf_num. \n
 *                 For receiving data, upper application SHOULD process data stored in buf indexed by \ref proc_buf_num..
 * @param priv private data
 *
 *
 */
typedef struct _hal_usb_iso_request_block
{
    uint8_t *buf0;
    uint8_t *buf1;

    uint32_t data_per_frame;
    uint32_t buf_proc_intrvl;

    uint8_t pkt_cnt;
    T_HAL_USB_ISO_PKT_INFO *iso_pkt0;
    T_HAL_USB_ISO_PKT_INFO *iso_pkt1;

    void *ep_handle;
    int (*complete)(struct _hal_usb_iso_request_block *urb, uint8_t proc_buf_num);

    void *priv;
} T_HAL_USB_ISO_REQUEST_BLOCK;

/**
 * @brief parameter of isr handler in \ref HAL_USB_ISR_HOOKS
 * @param enum_done: used in \ref HAL_USB_COMMON_ISR_ENUM_DONE
 * @param setup: used in \ref HAL_USB_COMMON_ISR_SETUP
 * @param xfer_done: used in \ref HAL_USB_COMMON_ISR_XFER_DONE
 *
 */
typedef union _hal_usb_isr_param
{
    struct
    {
        T_HAL_USB_SPEED speed;
    } enum_done;

    struct
    {
        uint8_t *setup_pkt;
    } setup;

    struct
    {
        T_HAL_USB_REQUEST_BLOCK *urb;
    } xfer_done;

} T_HAL_USB_ISR_PARAM;

/**
 * @brief USB common ISR type
 * @param HAL_USB_COMMON_ISR_RESET: interrupt when device reset
 * @param HAL_USB_COMMON_ISR_ENUM_DONE: interrupt when speed enum is done
 * @param HAL_USB_COMMON_ISR_SETUP: interrupt when setup packet is received
 * @param HAL_USB_COMMON_ISR_SUSPEND: interrupt when device suspends
 * @param HAL_USB_COMMON_ISR_RESUME: interrupt when device resumes
 * @param HAL_USB_COMMON_ISR_XFER_DONE: interrupt when non isochronous data transfer is done and \ref complete_in_isr of the urb is 0
 *
 */
typedef enum
{
    HAL_USB_COMMON_ISR_RESET,
    HAL_USB_COMMON_ISR_ENUM_DONE,
    HAL_USB_COMMON_ISR_SETUP,
    HAL_USB_COMMON_ISR_SUSPEND,
    HAL_USB_COMMON_ISR_RESUME,
    HAL_USB_COMMON_ISR_XFER_DONE,
} T_HAL_USB_COMMON_ISR;

typedef void (*HAL_USB_COMMON_ISR_ENTER)(void);
typedef void (*HAL_USB_COMMON_ISR_HANDLER)(T_HAL_USB_COMMON_ISR, T_HAL_USB_ISR_PARAM *param);
typedef void (*HAL_USB_COMMON_ISR_EXIT)(void);

typedef void (*HAL_USB_SUSPENDN_ISR_ENTER)(void);
typedef void (*HAL_USB_SUSPENDN_ISR_HANDLER)(void);
typedef void (*HAL_USB_SUSPENDN_ISR_EXIT)(void);

typedef void (*HAL_USB_ISOC_ISR_ENTER)(void);
typedef void (*HAL_USB_ISOC_ISR_HANDLER)(void);
typedef void (*HAL_USB_ISOC_ISR_EXIT)(void);

/**
 * @brief USB ISR Hooks
 * @param enter: called at the beginning of the common isr
 * @param handler: handler of ISR defined in \ref T_HAL_USB_COMMON_ISR
 * @param exit: called at the end of the common isr
 *
 */
#define HAL_USB_ISR_HOOKS(type, t_enter, t_handler, t_exit)               \
    struct _hal_usb_isr_hooks_##type                                          \
    {                                                                         \
        t_enter enter;                                                        \
        t_handler handler;                                                    \
        t_exit exit;                                                          \
    }

typedef HAL_USB_ISR_HOOKS(common, HAL_USB_COMMON_ISR_ENTER, HAL_USB_COMMON_ISR_HANDLER,
                          HAL_USB_COMMON_ISR_EXIT) HAL_USB_COMMON_ISR_HOOKS;
typedef HAL_USB_ISR_HOOKS(suspend, HAL_USB_SUSPENDN_ISR_ENTER, HAL_USB_SUSPENDN_ISR_HANDLER,
                          HAL_USB_SUSPENDN_ISR_EXIT) HAL_USB_SUSPENDN_ISR_HOOKS;

/**
 * @brief update common isr handlers
 *
 * @param hooks: hooks to process common isr
 * @return int  result, refer to \ref "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_common_isr_handler_update(HAL_USB_COMMON_ISR_HOOKS *hooks);

/**
 * @brief update suspendn isr handlers
 *
 * @param hooks: hooks to process suspendn isr
 * @return int  result, refer to \ref "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_suspendn_isr_handler_update(HAL_USB_SUSPENDN_ISR_HOOKS *hooks);

/**
 * @brief Set USB device address, the function SHOULD be called to process SET_ADDRESS request
 *
 * @param addr device address
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * @code
 *      #include "usb_spec20.h"
 *      int setup_cb(uint8_t *data)
 *      {
 *          T_USB_DEVICE_REQUEST *ctrl_request = (T_USB_DEVICE_REQUEST *)data;
 *          ret = 0;
 *          if(ctrl_request->bmRequestType & USB_TYPE_MASK == USB_TYPE_STANDARD)
 *          {
 *              if(ctrl_request->bRequest == USB_REQ_CODE_SET_ADDRESS)
 *              {
 *                  ret = hal_usb_set_address(UGETW(ctrl_request->wValue));
 *              }
 *          }
 *          return ret;
 *      }
 * @endcode
 *
 */
int hal_usb_set_address(uint8_t addr);

/**
 * @brief allocate urb for data transferring
 *
 * @param buf_len length of buffer to store data
 * @return T_HAL_USB_REQUEST_BLOCK* NULL: fail, other: success
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 */
T_HAL_USB_REQUEST_BLOCK *hal_usb_urb_alloc(uint32_t buf_len);

/**
 * @brief free urb allocated by \ref hal_usb_urb_alloc
 *
 * @param urb allocated by \ref hal_usb_urb_alloc
 * @return int  result, refer to \ref "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 */
int hal_usb_urb_free(T_HAL_USB_REQUEST_BLOCK *urb);

/**
 * @brief allocate isochronous urb for data transferring
 *
 * @param buf_len length of buffer to store data
 * @return T_HAL_USB_ISO_REQUEST_BLOCK* NULL: fail, other: success
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
T_HAL_USB_ISO_REQUEST_BLOCK *hal_usb_iso_urb_alloc(uint32_t buf_len);

/**
 * @brief free isochronous urb allocated by \ref hal_usb_iso_urb_alloc
 *
 * @param urb allocated by \ref hal_usb_iso_urb_alloc
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_iso_urb_free(T_HAL_USB_ISO_REQUEST_BLOCK *urb);

/**
 * @brief get endpoint handle, the return value is necessary for all endpoint related functions, \n
 *        include hal_usb_ep_xxx & hal_usb_iso_ep_xxx
 *
 * @param addr endpoint address
 * @return void* endpoint handle NULL: invalid other: valid
 * @par Example
 * Please refer to "Endpoint setup" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void *hal_usb_ep_handle_get(uint8_t addr);

/**
 * @brief enable endpoint, the function should be called when receiving SET_ALT request.
 *
 * @param ep_handle returned by \ref hal_usb_ep_handle_get
 * @param desc endpoint descriptor
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Endpoint setup" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_ep_enable(void *ep_handle, void *desc);

/**
 * @brief disable endpoint, the function should be called when receiving SET_ALT request.
 *
 * @param ep_handle returned by \ref hal_usb_ep_handle_get
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_ep_disable(void *ep_handle);

/**
 * @brief endpoint0 send data to host or receive data from host
 *
 * @param ep_handle handle of endpoint0
 * @param urb urb returned by \ref hal_usb_urb_alloc
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_ep0_trx(void *ep_handle, T_HAL_USB_REQUEST_BLOCK *urb);

/**
 * @brief non zero endpoint send data to host except isochronous endpoint
 *
 * @param ep_handle handle of endpoint, , returned by \ref hal_usb_ep_handle_get
 * @param urb urb returned by \ref hal_usb_urb_alloc
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_ep_tx(void *ep_handle, T_HAL_USB_REQUEST_BLOCK *urb);

/**
 * @brief non zero endpoint receive data from host except isochronous endpoint
 *
 * @param ep_handle handle of endpoint
 * @param urb urb returned by \ref hal_usb_urb_alloc
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_ep_rx(void *ep_handle, T_HAL_USB_REQUEST_BLOCK *urb);

/**
 * @brief halt the interrupt/bulk endpoints or return STALL in the IN status stage of control transfer. \n
 *        For endpoint0, the api is used when request is not supported or other errors occur, \n
 *        the device will return STALL in the IN status stage of control transfer. \n
 *        For non zero endpoint, the api is used when process SetFeature(ENDPOINT_HALT) request
 *
 * @param ep_handle endpoint handle, , returned by \ref hal_usb_ep_handle_get
 * @param err error that causing stall
 * @return int  result, refer to "errno.h"
 * @par Example
 * @code
 *      //endpoint0
 *       int setup_cb(uint8_t *data)
 *      {
 *          if request is not supported
 *          {
 *              hal_usb_ep_stall_set(ep0_handle, -ENOTSUPP);
 *          }
 *          return -ENOTSUPP;
 *      }
 *
 *      //interrupt/bulk endpoints
 *  *   int setup_cb(uint8_t *data)
 *      {
 *          if request is SetFeature(ENDPOINT_HALT)
 *          {
*               uint8_t addr = UGETW(wIndex);
                void *ep_handle = hal_usb_ep_handle_get(addr);
 *              hal_usb_ep_stall_set(ep_handle, 0);
 *          }
 *          return 0;
 *      }
 *
 * @endcode
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_ep_stall_set(void *ep_handle, int err);

/**
 * @brief clear endpoint halt when process ClearFeature(ENDPOINT_HALT) request
 *
 * @param ep_handle endpoint handle, , returned by \ref hal_usb_ep_handle_get
 * @return int result, refer to "errno.h"
 * @par Example
 * @code
 *
 *      //interrupt/bulk endpoints
 *  *   int setup_cb(uint8_t *data)
 *      {
 *          if request is ClearFeature(ENDPOINT_HALT)
 *          {
*               uint8_t addr = UGETW(wIndex);
                void *ep_handle = hal_usb_ep_handle_get(addr);
 *              hal_usb_ep_stall_clear(ep_handle);
 *          }
 *          return 0;
 *      }
 *
 * @endcode
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_ep_stall_clear(void *ep_handle);

/**
 * @brief get endpoint stall status when process GetStatus(ENDPOINT) request
 *
 * @param ep_handle endpoint handle
 * @return uint16_t status
 * @par Example
 * @code
 *
 *      int setup_cb(uint8_t *data)
 *      {
 *          ...
 *          if request is  GetStatus(ENDPOINT)
 *          {
*               uint8_t addr = UGETW(wIndex);
                void *ep_handle = hal_usb_ep_handle_get(addr);
 *              uint16_t status = hal_usb_ep_stall_status_get(ep_handle);
                memcpy(ctrl_urb->buf, &status, 2);
                ....
 *          }
 *          return 0;
 *      }
 *
 * @endcode
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
uint16_t hal_usb_ep_stall_status_get(void *ep_handle);

/**
 * @brief start isochronous transfer
 *
 * @param ep_handle isochronous endpoint, returned by \ref hal_usb_ep_handle_get
 * @param iso_urb isochronous urb, returned by \ref hal_usb_iso_urb_alloc
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_iso_ep_start(void *ep_handle, T_HAL_USB_ISO_REQUEST_BLOCK *iso_urb);

/**
 * @brief  stop isochronous transfer
 *
 * @param ep_handle isochronous endpoint, returned by \ref hal_usb_ep_handle_get
 * @param iso_urb isochronous urb, returned by \ref hal_usb_iso_urb_alloc
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Data transfer" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_iso_ep_stop(void *ep_handle, T_HAL_USB_ISO_REQUEST_BLOCK *iso_urb);

/**
 * @brief init HAL software resource
 * @return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to "Hardware setup" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_init(void);

/**
 * @brief deinit HAL software resource
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_deinit(void);

/**
 * @brief set usb default speed, it is used to init hardware
 *
 * @param speed refer to \ref T_HAL_USB_SPEED
 * @return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to "Hardware setup" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_speed_set(T_HAL_USB_SPEED speed);

/**
 * @brief remote wakeup host, if the bmAttributes support remote wakeup in configure descriptor
 *
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_remote_wakeup(void);

/**
 * @brief init USB mac
 *
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Hardware setup" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_mac_init(void);

/**
 * @brief deinit USB mac
 *
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_mac_deinit(void);

/**
 * @brief power on USB phy
 *
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Hardware setup" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_phy_power_on(void);

/**
 * @brief power down USB phy
 *
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_phy_power_down(void);

/**
 * @brief generate connect signal to host
 *
 * @return int result, refer to "errno.h"
 * @par Example
 * Please refer to "Hardware setup" in \ref HAL_Usage_Chapter
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_soft_attach(void);

/**
 * @brief generate disconnect signal to host
 *
 * @return int result, refer to "errno.h"
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_soft_detach(void);

/**
 * @brief enable dedicated isochronous interrupt, refer to \ref T_USB_ISR_TYPE
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void hal_usb_isoc_interrupt_enable(void);

/**
 * @brief disable dedicated isochronous interrupt, refer to \ref T_USB_ISR_TYPE
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void hal_usb_isoc_interrupt_disable(void);

/**
 * @brief disable global interrupt to the application
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void hal_usb_global_isr_disable(void);

/**
 * @brief enable global interrupt to the application
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
void hal_usb_global_isr_enable(void);

/**
 * @brief USB phy enter suspend
 *
 * @return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_suspend_enter(void);

/**
 * @brief USB phy exit suspend
 *
 * @return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_suspend_exit(void);

/**
 * @brief USB phy set sts
 *
 * @return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_wakeup_status_clear(void);

/**
 * @brief USB phy get sts
 *
 * @return int result, if sts is set to 1, USB wake up.
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_wakeup_status_get(void);

/**
 * @brief USB process test mode request
 *
 * @param sel test mode selector
 * @return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int hal_usb_do_test_mode(uint8_t sel);
/** @}*/

#endif
