/**
 * @copyright Copyright (C) 2022 Realtek Semiconductor Corporation.
 *
 * @file usb_audio_driver.h
 * @version 1.0
 * @brief Upper application can used the definitions&APIs to realize Audio function instances.
 *        The driver support multiple audio functions, which have the standalone audio control/streaming
 *
 * @note:
 */
#ifndef _USB_AUDIO_DRIVER_H_
#define _USB_AUDIO_DRIVER_H_
#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup USB_AUDIO_DRIVER
 * @{
 * The module mainly supply components to realize USB Audio Class. \n
 * This driver support UAC 1.0 & UAC 2.0, and also support multiple function instances, \n
 * such as 2 speakers that all in working. This driver also support multiple alternates per interface.
 * @}
  */

/**
 * @addtogroup USB_AUDIO_DRIVER
 * @{
 * @section USB_AUDIO_DRIVER_USAGE How to realize USB Audio interface.
 * Firstly, allocate a function instance by \ref usb_audio_driver_inst_alloc, and the input \ref version \n
 * is the version of UAC-- \ref USB_AUDIO_VERSION_1 \ \ref USB_AUDIO_VERSION_2.
 * @par Example
 * @code
 *      void *demo_instance = usb_audio_driver_inst_alloc(USB_AUDIO_VERSION_1, 2, 2);
 * @endcode
 *
 * Then, realize control interface & audio streaming interface as follows:
 *  - \b Audio \b Control \b interface. \n
 *    - step1: realize descriptors array. \n
 *    - step2: realize control related process. \n
 *             The core structure is \ref T_USB_AUDIO_DRIVER_CTRL to process individual control, such volume control. \n
 *             One control unit may includes multiple controls, such as one feature unit may includes volume\mute control \n
 *             simultaneously, so no less than 1  \ref T_USB_AUDIO_DRIVER_CTRL make up one control entity \ref T_USB_AUDIO_DRIVER_CTRL_ENTITY. \n
 *             All control entities make up the total control topology defined in \b step \b 1 . \n
 * @par Example -- USB Audio speaker
 * @code
 *      #include "usb_spec20.h"
 *      #include "uac1.h"
 *      #include "usb_audio1_ctrl.h"
 *
 *      T_USB_INTERFACE_DESC demo_ac_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_AC_HDR_DESC demo_ac_hdr_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_IT_DESC demo_input_terminal_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_OT_DESC demo_output_terminal_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_FEATURE_UNIT_DESC(UNIT_ID, 3) demo_feature_unit_desc =
 *      {
 *          ..init...
 *          .bUnitID = UNIT_ID,
 *          .bmaControls[0] = UAC_CONTROL_BIT(UAC1_FU_MUTE_CONTROL) | UAC_CONTROL_BIT(UAC1_FU_VOLUME_CONTROL),
 *          ...init...
 *      }
 *
 *      T_USB_DESC_HDR *demo_ctrl_descs_fs[] =
 *      {
 *          (T_USB_DESC_HDR*)&demo_ac_desc,
 *          (T_USB_DESC_HDR*)&demo_ac_hdr_desc,
 *          (T_USB_DESC_HDR*)&demo_input_terminal_desc,
 *          (T_USB_DESC_HDR*)&demo_output_terminal_desc,
 *          (T_USB_DESC_HDR*)&demo_feature_unit_desc,
 *          NULL
 *      };
 *
 *      ...
 *      T_USB_DESC_HDR *demo_ctrl_descs_hs[] =
 *      {
 *          ...
 *          NULL
 *      };
 *
 *
 *      T_CTRL_ATTR demo_vol_attr_spk =
 *      {
 *          ...init...
 *      };
 *
 *      static T_USB_AUDIO_DRIVER_CTRL_ATTR uac1_drv_ctrl_attr = {.data = NULL, .len = 0};
 *      T_USB_AUDIO_DRIVER_CTRL_ATTR *demo_vol_attr_get_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_attr,
 *                                                     uint8_t cmd)
 *      {
 *          T_CTRL_ATTR *vol_attr = (T_CTRL_ATTR *)drv_attr->data;
 *          int32_t data = ((int32_t *)(vol_attr->data))[cmd - 1];
 *          uac1_drv_ctrl_attr.data = &data;
 *          uac1_drv_ctrl_attr.len = sizeof(data);
 *          return &uac1_drv_ctrl_attr;
 *      }
 *
 *      int32_t demo_vol_attr_set_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_attr, uint8_t cmd, int value)
 *      {
 *          //Process
 *          return 0;
 *      }
 *      T_USB_AUDIO_DRIVER_CTRL demo_vol_ctrl_spk =
 *      {
 *          .type = UAC1_FU_VOLUME_CONTROL,
 *          .attr = {.data = (void *) &demo_vol_attr_spk, .len = sizeof(demo_vol_attr_spk)},
 *          .get = (T_USB_AUDIO_CTRL_GET_FUNC)demo_vol_attr_get_spk,
 *          .set = (T_USB_AUDIO_CTRL_SET_FUNC)demo_vol_attr_set_spk,
 *      };
 *
 *      T_CTRL_ATTR demo_mute_attr_spk =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_AUDIO_DRIVER_CTRL_ATTR *demo_mute_attr_get_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_attr,
 *                                                     uint8_t cmd)
 *      {
 *          T_CTRL_ATTR *mute_attr = (T_CTRL_ATTR *)drv_attr->data;
 *          int32_t data = mute_attr->attr.cur;
 *          uac1_drv_ctrl_attr.data = &data;
 *          uac1_drv_ctrl_attr.len = sizeof(data);
 *          return &uac1_drv_ctrl_attr;
 *      }
 *
 *      int32_t demo_mute_attr_set_spk(T_USB_AUDIO_DRIVER_CTRL_ATTR *drv_attr, uint8_t cmd, int value)
 *      {
 *          //Process
 *          return 0;
 *      }
 *      T_USB_AUDIO_DRIVER_CTRL demo_mute_ctrl_spk =
 *      {
 *          .type = UAC1_FU_VOLUME_CONTROL,
 *          .attr = {.data = (void *) &demo_mute_attr_spk, .len = sizeof(demo_mute_attr_spk)},
 *          .get = (T_USB_AUDIO_CTRL_GET_FUNC)demo_mute_attr_get_spk,
 *          .set = (T_USB_AUDIO_CTRL_SET_FUNC)demo_mute_attr_set_spk,
 *      };
 *
 *      T_USB_AUDIO_DRIVER_CTRL_ENTITY(UNIT_ID, 3) demo_ctrl_entity =
 *      {
 *          .entity_id = UNIT_ID,
 *          .ctrls[0] = &demo_vol_ctrl_spk,
 *          .ctrls[1] = &demo_mute_ctrl_spk,
 *          .ctrls[2] = NULL,
 *      };
 *
 *      T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR *demo_all_ctrls[] =
 *      {
 *          &demo_ctrl_entity,
 *          NULL,
 *      };
 *
 *      usb_audio_driver_desc_register(demo_instance, demo_ctrl_descs_fs, demo_ctrl_descs_hs);
 *      usb_audio_driver_ctrl_register(demo_instance, (T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR **)demo_all_ctrls);
 * @endcode
 *
 * - \b Audio \b Streaming \b interface \n
 *   - step 1: realize interface descriptors array. \n
 *   - step 2: initialize streaming attribute. \n
 *   - step 3: register cb to process audio data. \n
 * @par Example -- USB Audio speaker
 * @code
 *      #include "usb_spec20.h"
 *      #include "uac1.h"
 *
 *      T_USB_INTERFACE_DESC demo_if_alt0_desc =
 *      {
 *          ...init...
 *      };
 *      T_USB_INTERFACE_DESC demo_if_alt1_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_AS_HDR_DESC demo_as_hdr_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_FMT_TYPE_I_DESC(spk, sample rate num) demo_fmt_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_STD_ISO_EP_DESC demo_std_iso_out_ep_desc =
 *      {
 *          ...init...
 *      };
 *      T_UAC1_ISO_EP_DESC demo_cs_iso_out_ep_desc =
 *      {
 *          ...init...
 *      };
 *
 *      T_USB_DESC_HDR* demo_as_descs_spk_fs[] =
 *      {
 *          (T_USB_DESC_HDR*)&demo_if_alt0_desc,
 *          (T_USB_DESC_HDR*)&demo_if_alt1_desc,
 *          (T_USB_DESC_HDR*)&demo_as_hdr_desc,
 *          (T_USB_DESC_HDR*)&demo_fmt_desc,
 *          (T_USB_DESC_HDR*)&demo_std_iso_out_ep_desc,
 *          (T_USB_DESC_HDR*)&demo_cs_iso_out_ep_desc,
 *          NULL,
 *      }
 *
 *      ...
 *      T_USB_DESC_HDR* demo_as_descs_spk_hs[] =
 *      {
 *          ...
 *          NULL,
 *      }
 *
 *      usb_audio_driver_desc_register(demo_instance, demo_as_descs_spk_fs, demo_as_descs_spk_hs);
 *
 *      T_USB_AUDIO_DRIVER_ATTR demo_attr_alt0 =
 *      {
 *          .dir = USB_AUDIO_DIR_OUT,
 *          .chann_num = 0,
 *          .bit_width = 0,
 *          .cur_sample_rate = 0,
 *          .max_sample_rate = 0,
 *      };
 *      usb_audio_driver_attr_init(demo_instance, 0, demo_attr_alt0);
 *
 *      T_USB_AUDIO_DRIVER_ATTR demo_attr_alt1 =
 *      {
 *          .dir = USB_AUDIO_DIR_OUT,
 *          .chann_num = same as bNrChannels in demo_fmt_desc,
 *          .bit_width = same as bBitResolution in demo_fmt_desc,
 *          .cur_sample_rate = default sample rate,
 *          .max_sample_rate = max sample rate in demo_fmt_desc,
 *      };
 *      usb_audio_driver_attr_init(demo_instance, 1, demo_attr_alt1);
 *
 *      int demo_usb_audio_activate(uint8_t dir, uint8_t bit_res, uint32_t sample_rate, uint8_t chan_num)
 *      {
 *          //Process
 *      }
 *      int demo_usb_audio_deactivate(uint8_t dir)
 *      {
 *          //Process
 *      }
 *      int demo_usb_audio_downstream(uint8_t *buf, uint16_t len)
 *      {
 *          //Process data in buf which length is len
 *      }
 *      T_USB_AUDIO_DRIVER_CBS demo_cbs =
 *      {
 *          .activate = demo_usb_audio_activate,
 *          .deactivate = demo_usb_audio_deactivate,
 *          .upstream = NULL,
 *          .downstream = demo_usb_audio_downstream,
 *      }
 *      usb_audio_driver_cb_register(demo_instance, &demo_cbs);
 * @endcode
 *
 * Lastly, call \ref usb_audio_driver_init to initialize usb audio driver.
 */
/** @}*/

/**
 * @addtogroup USB_AUDIO_DRIVER
 * @{
 * @section Definitions
 */
/**
 * usb_audio_driver.h
 *
 * @brief   USB Audio class version
 *
 */
#define USB_AUDIO_VERSION_1         0x01
#define USB_AUDIO_VERSION_2         0x02

/**
 * usb_audio_driver.h
 *
 * \brief  maximun tolerance of actual sample rate
 *
 * \ingroup USB_CORE
 */
#define PERCENT_SAMPLE_RATE_TOLERANCE   (10)

/**
 * usb_audio_driver.h
 *
 * \brief  change feedback value to 10.14 format as defined in chapter 5.12.4.2 of spec "usb_20.pdf".
 *
 * \ingroup USB_CORE
 */
#define TO_FB_FORMAT_10_14(f)    (((f/1000) << 14) | ((f%1000) << 4))

/**
 * usb_audio_driver.h
 *
 * \brief  change feedback value to 16.16 format as defined in chapter 5.12.4.2 of spec "usb_20.pdf".
 *
 * \ingroup USB_CORE
 */
#define TO_FB_FORMAT_16_16(f)    (((f/1000) << 13) | ((f%1000) << 3))

/**
 * usb_audio_driver.h
 *
 * @brief   USB Audio stream direction
 *
 */
typedef enum
{
    USB_AUDIO_DIR_OUT = 1,
    USB_AUDIO_DIR_IN = 2,
} T_USB_AUDIO_DIR;

typedef  int (*USB_AUDIO_DRV_CB_ACTIVATE)(uint8_t dir, uint8_t bit_res, uint32_t sample_rate,
                                          uint8_t chan_num);
typedef  int (*USB_AUDIO_DRV_CB_DEACTIVATE)(uint8_t dir);
typedef  int (*USB_AUDIO_DRV_CB_XMIT)(uint8_t *buf, uint16_t len);
/**
 * @brief USB Audio driver cbs to transport necessary informations or audio data to upper sw \n
 *        \ref activate: usb audio function is ready to transmit audio data, param \dir is \n
 *        defined in \ref T_USB_AUDIO_DIR, \ref bit_res, \ref sample_rate and \ref chan_num are used together \n
 *        to define audio data attribute \n
 *        \ref deactivate: usb audio function is no longer transmit audio data, param \dir is \n
 *        defined in \ref T_USB_AUDIO_DIR \n
 *        \ref upstream: transmit data from device to host, \ref buf is audio data will be sent, \ref len is length of \ref buf \n
 *        \ref downstream: transmit data from host to device, \ref buf is audio data has been received, \ref len is length of \ref buf \n
 *        \ref feedback_d: transmit feedback value of downstream from device to host,\ref buf is audio data will be sent, \ref len is length of \ref buf \n
 */
typedef struct _usb_audio_driver_cbs
{
    USB_AUDIO_DRV_CB_ACTIVATE activate;
    USB_AUDIO_DRV_CB_DEACTIVATE deactivate;
    USB_AUDIO_DRV_CB_XMIT upstream;
    USB_AUDIO_DRV_CB_XMIT downstream;
    USB_AUDIO_DRV_CB_XMIT feedback_d;
} T_USB_AUDIO_DRIVER_CBS;

/**
 * @brief usb audio driver control attribute \n
 *        \ref data: contents of attribute, for example, \n
 *        usb audio1.0 attribute can be defined as int attribute[5], the array is index by  \n
 *        xxx_CUR/xxx_MIN/xxx_MAX/xxx_RES/xxx_MEM defined in ch5 in https://www.usb.org/document-library/audio-device-document-10 \n
 *        usb audio2.0 attribute is defined in ch5.2.3 in https://www.usb.org/document-library/audio-devices-rev-20-and-adopters-agreement \n
 *        \ref len: length of data
 *
 */
typedef struct _usb_audio_driver_ctrl_attr
{
    void *data;
    uint32_t len;
} T_USB_AUDIO_DRIVER_CTRL_ATTR;
/**
 * @brief USB Audio callbacks to process control selectors cmd
 *
 */
typedef T_USB_AUDIO_DRIVER_CTRL_ATTR *(*T_USB_AUDIO_CTRL_GET_FUNC)(T_USB_AUDIO_DRIVER_CTRL_ATTR
                                                                   *attr, uint8_t cmd);
typedef int (*T_USB_AUDIO_CTRL_SET_FUNC)(T_USB_AUDIO_DRIVER_CTRL_ATTR *attr, uint8_t cmd,
                                         int value);
/**
 * @brief USB Audio driver control to realize control selectors process \n
 *        \ref type: control type, for usb audio1.0,refer to A.10 of https://www.usb.org/document-library/audio-device-document-10 \n
 *        for usb audio2.0,refer to A.17 of https://www.usb.org/document-library/audio-devices-rev-20-and-adopters-agreement \n
 *        \ref attr stores attributes defined in \ref T_USB_AUDIO_DRIVER_CTRL_ATTR \n
 *        \ref get is used to process GET_XXX request, for example, GET_CUR .etc
 *
 */
typedef struct _usb_audio_driver_ctrl
{
    uint8_t type;
    T_USB_AUDIO_DRIVER_CTRL_ATTR attr;
    T_USB_AUDIO_CTRL_SET_FUNC set;
    T_USB_AUDIO_CTRL_GET_FUNC get;
} T_USB_AUDIO_DRIVER_CTRL;
/**
 * @brief USB Audio driver control per control entity \n
 *        \ref entity_id: id of entity defined in control interface, \n
 *        it MUST be EQUAL TO bTerminalID of the corresponding unit descriptor
 *
 *        \ref ctrls: control functions of the given control selector
 *
 */
#define T_USB_AUDIO_DRIVER_CTRL_ENTITY(id, num)     \
    struct _usb_audio_driver_ctrl_entity##id            \
    {                                                   \
        uint8_t     entity_id;                          \
        uint8_t     ctrl_num;                           \
        T_USB_AUDIO_DRIVER_CTRL *ctrls[num];                \
    }
/**
 * @brief common header of controls defined in \ref T_USB_AUDIO_DRIVER_CTRL_ENTITY
 *
 */
typedef struct _usb_audio_driver_ctrl_entity_hdr
{
    uint8_t     entity_id;
    uint8_t     ctrl_num;
} T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR;
/**
 * usb_audio_driver.h
 *
 * @brief   USB Audio stream atrributes
 *          \ref dir: ref to \ref T_USB_AUDIO_DIR
 *          \ref chann_num: channel number of the given alternate setting
 *          \ref bit_width: bit width of the given alternate setting
 *  *       \ref max_sample_rate: maximum  sample rate of the given alternate setting
 *
 */
typedef struct _usb_audio_driver_attr
{
    T_USB_AUDIO_DIR dir;
    uint8_t         chann_num;
    uint8_t         bit_width;
    uint32_t        cur_sample_rate;
    uint32_t        max_sample_rate;
} T_USB_AUDIO_DRIVER_ATTR;
/**
 * @brief  common header of all USB descriptors
 *
 */
typedef struct _usb_audio_driver_desc_hdr
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
} __attribute__((packed)) T_USB_AUDIO_DRIVER_DESC_HDR;

/**
 * @brief alloc audio function instance, which has the standalone audio control/streaming
 * @param version \ref USB_AUDIO_VERSION_1 & \ref USB_AUDIO_VERSION_2
 * @param proc_interval_out
 * @return void*  audio function instance
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_AUDIO_DRIVER_USAGE
 */
void *usb_audio_driver_inst_alloc(uint8_t version, uint8_t proc_interval_out,
                                  uint8_t proc_interval_in);

/**
 * @brief free audio function instance,alloacted by \ref usb_audio_driver_inst_alloc
 * @param inst, instance alloacted by \ref usb_audio_driver_inst_alloc
 * @return int result, refer to "errno.h"
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @par Example
 * Please refer to \ref USB_AUDIO_DRIVER_USAGE
 */
int usb_audio_driver_inst_free(void *inst);
/**
 * @brief register inteface descriptors
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 * @param desc_fs full speed inteface descriptors
 * @param desc_hs high speed inteface descriptors
 * @return int result, refer to "errno.h"
 *
 * @par Example
 * Please refer to \b Audio \b Control \b interface & \b Audio \b Streaming \b interface in \ref USB_AUDIO_DRIVER_USAGE
 *
 */
int usb_audio_driver_desc_register(void *inst, T_USB_AUDIO_DRIVER_DESC_HDR **descs_fs,
                                   T_USB_AUDIO_DRIVER_DESC_HDR **descs_hs);
/**
 * @brief register usb audio callbacks
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 * @param cbs \ref T_USB_AUDIO_DRIVER_CBS
 *
 * @par Example
 * Please refer to \b Audio \b Streaming \b interface in \ref USB_AUDIO_DRIVER_USAGE
 */
int usb_audio_driver_cb_register(void *inst, T_USB_AUDIO_DRIVER_CBS *cbs);
/**
 * @brief register usb audio control process
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 * @param ctrl  refer to \ref T_USB_AUDIO_DRIVER_CTRL_ENTITY
 *
 * @par Example
 * Please refer to \b Audio \b Control \b interface in \ref USB_AUDIO_DRIVER_USAGE
 *
 */
int usb_audio_driver_ctrl_register(void *inst, T_USB_AUDIO_DRIVER_CTRL_ENTITY_HDR *ctrl[]);
/**
 * @brief unregister inteface descriptors
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 *
 */
int usb_audio_driver_desc_unregister(void *inst);
/**
 * @brief unregister inteface callbacks
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 *
 */
int usb_audio_driver_cb_unregister(void *inst);
/**
 * @brief unregister inteface control process
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 *
 */
int usb_audio_driver_ctrl_unregister(void *inst);
/**
 * @brief USB audio interface frequency set to USB audio driver.
 *        it is only used in USB Audio2.0, because its sample freq control is realized in control interface
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 * @param dir \ref T_USB_AUDIO_DIR
 * @param freq sample frequency
 *
 * @note
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 */
int usb_audio_driver_freq_change(void *inst, T_USB_AUDIO_DIR dir, uint32_t freq);
/**
 * @brief initialize  USB Audio attribute of the given alternate setting
 *
 * @param inst audio instance returned in \ref usb_audio_driver_inst_alloc
 * @param alt_num alternate setting defined in bAlternateSetting of interface descriptor
 * @param attr ref T_USB_AUDIO_DRIVER_ATTR
 *
 * @par Example
 * Please refer to \b Audio \b Streaming \b interface in \ref USB_AUDIO_DRIVER_USAGE
 *
 */
int usb_audio_driver_attr_init(void *inst, uint8_t alt_num, T_USB_AUDIO_DRIVER_ATTR attr);
/** @}*/
#endif
