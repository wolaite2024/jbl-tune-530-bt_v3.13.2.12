/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     hids_cfu.c
  * @brief    Source file for using Human Interface Device Service.
  * @details  Global data and function implement.
  * @author   Leon_liu
  * @date     2022-9-21
  * @version  v1.0
  * *************************************************************************************
  */

#include <string.h>
#include "trace.h"
#include "hids_cfu.h"


#define GATT_UUID_HID                           0x1812
#define GATT_UUID_CHAR_PROTOCOL_MODE            0x2A4E
#define GATT_UUID_CHAR_REPORT                   0x2A4D
#define GATT_UUID_CHAR_REPORT_MAP               0x2A4B
#define GATT_UUID_CHAR_BOOT_MS_IN_REPORT        0x2A33
#define GATT_UUID_CHAR_HID_INFO                 0x2A4A
#define GATT_UUID_CHAR_HID_CONTROL_POINT        0x2A4C

static P_FUN_EXT_SERVER_GENERAL_CB pfn_hids_cb = NULL;
T_SERVER_ID service_id;

T_HID_INFO hid_info = {0, 0, 0};
T_HID_PROTOCOL_MODE hid_protocol_mode = REPORT_PROCOCOL_MODE;
uint8_t hid_suspand_mode = 0;
uint16_t external_report_refer = 0;

#define HIDS_CFU_REPORT_1   (0x2A)
#define HIDS_CFU_REPORT_2   (0x2C)
#define HIDS_CFU_REPORT_3   (0x2D)

const uint8_t hids_report_descriptor[] =
{
    0x06, 0x0b, 0xff,        /* USAGE_PAGE       Vendor          */
    0x0a, 0x04, 0x01,        /* USAGE                            */
    /* 8-bit data */
    0xa1, 0x01,              /* COLLECTION        (Application)  */
    0x15, 0x00,              /* LOGICAL_MINIMUM   (0)            */
    0x25, 0xff,              /* LOGICAL_MAXIMUM   (255)          */
    0x75, 0x08,              /* REPORT_SIZE       (8)            */
    0x95, 0x3c,              /* REPORT_COUNT      (60)           */
    0x85, HIDS_CFU_REPORT_1,              /* REPORT_ID         (42)           */
    0x09, 0x60,              /* USAGE             (0x60)         */
    0x82, 0x02, 0x01,        /* INPUT             (Data,Var,Abs) */
    0x09, 0x61,              /* USAGE             (0x61)         */
    0x92, 0x02, 0x01,        /* OUTPUT            (Data,Var,Abs) */
    0x09, 0x62,              /* USAGE             (0x62)         */
    0xb2, 0x02, 0x01,        /* FEATURE           (Data,Var,Abs) */
    /* 32-bit data */
    0x17, 0x00, 0x00, 0x00, 0x80,   /* LOGICAL_MINIMUM                 */
    0x27, 0xff, 0xff, 0xff, 0x7e,   /* LOGICAL_MAXIMUM                 */
    0x75, 0x20,                     /* REPORT_SIZE      (32)           */
    0x95, 0x04,                     /* REPORT_COUNT     (4)            */
    0x85, HIDS_CFU_REPORT_2,                     /* REPORT_ID        (44)           */
    0x19, 0x66,                     /* USAGE_MINIMUM    (0x66)         */
    0x29, 0x69,                     /* USAGE_MAXIMUM    (0x69)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x85, HIDS_CFU_REPORT_3,                     /* REPORT_ID        (45)           */
    0x19, 0x8a,                     /* USAGE_MINIMUM    (0x8a)         */
    0x29, 0x8d,                     /* USAGE_MAXIMUM    (0x8d)         */
    0x81, 0x02,                     /* INPUT            (Data,Var,Abs) */
    0x19, 0x8e,                     /* USAGE_MINIMUM    (0x8e)         */
    0x29, 0x91,                     /* USAGE_MAXIMUM    (0x91)         */
    0x91, 0x02,                     /* OUTPUT           (Data,Var,Abs) */
    0xc0,                           /* END_COLLECTION                  */
};


static const T_ATTRIB_APPL hids_attr_tbl[] =
{
    /* <<Primary Service>>, .. 0*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* wFlags     */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_HID),               /* service UUID */
            HI_WORD(GATT_UUID_HID)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },

    /* <<Characteristic>>, .. 1*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP,                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID Protocol Mode characteristic value ..2*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_PROTOCOL_MODE),
            HI_WORD(GATT_UUID_CHAR_PROTOCOL_MODE)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                        /* wPermissions */
    },

    /* <<Characteristic>>, .. 3*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_NOTIFY,           /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 4*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE /* wPermissions */
    },

    /* client characteristic configuration .. 5*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_NOTIFY), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_NOTIFY)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    },

    /*report ID map reference descriptor .. 6*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HIDS_CFU_REPORT_1,
            HID_INPUT_TYPE,
        },
        2,                                          /* bValueLen */
        NULL,//(void*)&cPointerInputReportIdMap,
        (GATT_PERM_READ)        /* wPermissions */
    },

    /* <<Characteristic>>, .. 7*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,   /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 8*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                          /* wPermissions */
    },

    /*report ID map reference descriptor .. 9*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                   /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HIDS_CFU_REPORT_1,
            HID_OUTPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ)          /* wPermissions */
    },

    /* <<Characteristic>>, .. 10*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE, /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 11*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                          /* wPermissions */
    },

    /*report ID map reference descriptor .. 12*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                   /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            HIDS_CFU_REPORT_1,
            HID_FEATURE_TYPE
        },
        2,                                          /* bValueLen */
        (void *)NULL,
        (GATT_PERM_READ)          /* wPermissions */
    },

    /* <<Characteristic>>, .. 13*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_NOTIFY,           /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 14*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE /* wPermissions */
    },

    /* client characteristic configuration .. 15*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_NOTIFY), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_NOTIFY)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    },

    /*report ID map reference descriptor .. 16*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HIDS_CFU_REPORT_2,
            HID_INPUT_TYPE,
        },
        2,                                          /* bValueLen */
        NULL,//(void*)&cPointerInputReportIdMap,
        (GATT_PERM_READ)        /* wPermissions */
    },

    /* <<Characteristic>>, .. 17*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_NOTIFY,           /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 18*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                       /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE /* wPermissions */
    },

    /* client characteristic configuration .. 19*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_NOTIFY), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_NOTIFY)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* wPermissions */
    },

    /*report ID map reference descriptor .. 20*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                  /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HIDS_CFU_REPORT_3,
            HID_INPUT_TYPE,
        },
        2,                                          /* bValueLen */
        NULL,//(void*)&cPointerInputReportIdMap,
        (GATT_PERM_READ)        /* wPermissions */
    },

    /* <<Characteristic>>, .. 21*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP,   /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* wPermissions */
    },

    /*  HID Report characteristic value .. 22*/
    {
        ATTRIB_FLAG_VALUE_APPL,                   /* wFlags */
        {                                         /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT),
            HI_WORD(GATT_UUID_CHAR_REPORT)
        },
        0,                                        /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE                          /* wPermissions */
    },

    /*report ID map reference descriptor .. 23*/
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL),                   /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_REPORT_REFERENCE),
            HIDS_CFU_REPORT_3,
            HID_OUTPUT_TYPE
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ)          /* wPermissions */
    },

    /* <<Characteristic>>, .. 24*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID report map characteristic value .. 25*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_REPORT_MAP),
            HI_WORD(GATT_UUID_CHAR_REPORT_MAP)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* <<EXTERNAL_REPORT_REFERENCE>>, .. 26*/
    {
        ATTRIB_FLAG_VALUE_APPL,                                 /* flags */
        {                                                       /* type_value */
            LO_WORD(GATT_UUID_CHAR_EXTERNAL_REPORT_REFERENCE),
            HI_WORD(GATT_UUID_CHAR_EXTERNAL_REPORT_REFERENCE),
        },
        0,                                                      /* bValueLen */
        (void *)NULL,
        GATT_PERM_READ                                          /* permissions */
    },

    /* <<Characteristic>>, .. 27*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ,                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID Information characteristic value .. 28*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_INFO),
            HI_WORD(GATT_UUID_CHAR_HID_INFO)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /* <<Characteristic>>, .. 29*/
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP,                      /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* wPermissions */
    },

    /*  HID controlPoint characteristic value .. 30*/
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* wFlags */
        {                                           /* bTypeValue */
            LO_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT),
            HI_WORD(GATT_UUID_CHAR_HID_CONTROL_POINT)
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ | GATT_PERM_WRITE            /* wPermissions */
    }

};

/**
 * @brief       Set a HID service parameter.
 *
 * @param[in]   param_type  HID service parameter type: @ref T_HIDS_PARAM_TYPE
 * @param[in]   len         Length of data to write
 * @param[in]   p_value Pointer to data to write.  This is dependent on
 *                      the parameter type and WILL be cast to the appropriate
 *                      data type
 *
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t mode = 1;
        hids_set_parameter(HID_PROTOCOL_MODE, 1, &mode);
    }
 * \endcode
 */
bool hids_set_parameter(T_HIDS_PARAM_TYPE param_type, uint8_t length, void *value_ptr)
{
    bool ret = true;

    switch (param_type)
    {
    case HID_PROTOCOL_MODE:
        {
            hid_protocol_mode = (T_HID_PROTOCOL_MODE) * ((uint8_t *)value_ptr);
        }
        break;

    case HID_REPORT_INPUT:
        break;

    case HID_REPORT_OUTPUT:
        break;

    case HID_REPORT_FEATURE:
        break;

    case HID_REPORT_MAP:
        break;

    case HID_EXTERNAL_REPORT_REFER:
        {
            external_report_refer = *(uint16_t *)value_ptr;
        }
        break;

    case HID_INFO:
        {
            memcpy((void *)&hid_info, value_ptr, length);
        }
        break;

    case HID_CONTROL_POINT:
        hid_suspand_mode = *((uint8_t *)value_ptr);
        break;

    default:
        ret = false;
        break;
    }
    return ret;
}


static T_APP_RESULT hids_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                      uint16_t attrib_index,
                                      uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_HID_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE;
    callback_data.conn_handle = conn_handle;
    callback_data.cid = cid;

    switch (attrib_index)
    {
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;

    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_PROTOCOL_MODE_INDEX;
        cause = pfn_hids_cb(service_id, (void *)&callback_data);
        *pp_value = (uint8_t *)&hid_protocol_mode;
        *p_length = sizeof(hid_protocol_mode);
        break;

    case GATT_SVC_HID_REPORT_MAP_INDEX:
        *pp_value = (uint8_t *)hids_report_descriptor;
        *p_length = sizeof(hids_report_descriptor);
        break;

    case GATT_SVC_HID_EXTERNAL_REPORT_REFER_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_EXTERNAL_REPORT_REFER_INDEX;
        cause = pfn_hids_cb(service_id, (void *)&callback_data);
        *pp_value = (uint8_t *)&external_report_refer;
        *p_length = sizeof(external_report_refer);
        break;

    case GATT_SVC_HID_INFO_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_INFO_INDEX;
        cause = pfn_hids_cb(service_id, (void *)&callback_data);
        *pp_value = (uint8_t *)&hid_info;
        *p_length = sizeof(hid_info);
        break;

    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        callback_data.msg_data.read_value_index = GATT_SVC_HID_CONTROL_POINT_INDEX;
        cause = pfn_hids_cb(service_id, (void *)&callback_data);
        *pp_value = (uint8_t *)&hid_suspand_mode;
        *p_length = sizeof(hid_suspand_mode);
        break;
    case GATT_SVC_HID_CFU_REPORT1_INPUT_INDEX:
        break;
    case GATT_SVC_HID_CFU_REPORT2_INPUT_INDEX:
        break;
    case GATT_SVC_HID_CFU_REPORT3_INPUT_INDEX:
        break;
    }

    return cause;
}


static T_APP_RESULT hids_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                       uint16_t attrib_index, T_WRITE_TYPE write_type,
                                       uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause  = APP_RESULT_SUCCESS;
    T_HID_CALLBACK_DATA callback_data;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
    callback_data.conn_handle = conn_handle;
    callback_data.msg_data.write_msg.write_type = write_type;
    callback_data.msg_data.write_msg.write_parameter.report_data.reportLen = length;
    callback_data.msg_data.write_msg.write_parameter.report_data.report = p_value;

    if (!p_value)
    {
        cause = APP_RESULT_INVALID_PDU;
        return cause;
    }

    switch (attrib_index)
    {
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;

    case GATT_SVC_HID_PROTOCOL_MODE_INDEX:
        break;

    case GATT_SVC_HID_CFU_REPORT1_INPUT_INDEX:
        //2A
        callback_data.msg_data.write_msg.write_parameter.report_data.report_id = HIDS_CFU_REPORT_1;
        callback_data.msg_data.write_msg.write_parameter.report_data.report_type = HID_INPUT_TYPE;
        break;

    case GATT_SVC_HID_CFU_REPORT1_OUTPUT_INDEX:
        //2A
        callback_data.msg_data.write_msg.write_parameter.report_data.report_id = HIDS_CFU_REPORT_1;
        callback_data.msg_data.write_msg.write_parameter.report_data.report_type = HID_OUTPUT_TYPE;
        break;

    case GATT_SVC_HID_CFU_REPORT1_FEATURE_INDEX:
        //2A
        callback_data.msg_data.write_msg.write_parameter.report_data.report_id = HIDS_CFU_REPORT_1;
        callback_data.msg_data.write_msg.write_parameter.report_data.report_type = HID_FEATURE_TYPE;
        break;

    case GATT_SVC_HID_CFU_REPORT2_INPUT_INDEX:
        //2C
        callback_data.msg_data.write_msg.write_parameter.report_data.report_id = HIDS_CFU_REPORT_2;
        callback_data.msg_data.write_msg.write_parameter.report_data.report_type = HID_INPUT_TYPE;
        break;

    case GATT_SVC_HID_CFU_REPORT3_INPUT_INDEX:
        //2D
        callback_data.msg_data.write_msg.write_parameter.report_data.report_id = HIDS_CFU_REPORT_3;
        callback_data.msg_data.write_msg.write_parameter.report_data.report_type = HID_INPUT_TYPE;
        break;

    case GATT_SVC_HID_CFU_REPORT3_OUTPUT_INDEX:
        //2D
        callback_data.msg_data.write_msg.write_parameter.report_data.report_id = HIDS_CFU_REPORT_3;
        callback_data.msg_data.write_msg.write_parameter.report_data.report_type = HID_OUTPUT_TYPE;
        break;

    case GATT_SVC_HID_INFO_INDEX:
        break;

    case GATT_SVC_HID_CONTROL_POINT_INDEX:
        break;
    }

    if (pfn_hids_cb && (cause == APP_RESULT_SUCCESS))
    {
        pfn_hids_cb(service_id, (void *)&callback_data);
    }

    return cause;

}

void hids_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id, uint16_t index,
                         uint16_t ccc_bits)
{
    bool cause = true;
    T_HID_CALLBACK_DATA callback_data;
    callback_data.conn_handle = conn_handle;
    callback_data.cid = cid;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;

    PROFILE_PRINT_INFO2("hids_cccd_update_cb index = %d ccc_bits %x", index, ccc_bits);

    switch (index)
    {
    default:
        cause = false;
        break;

    case GATT_SVC_HID_CFU_REPORT1_INPUT_CCCD_INDEX:
        {
            callback_data.msg_data.not_ind_data.index = GATT_SVC_HID_CFU_REPORT1_INPUT_CCCD_INDEX;
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_DISABLE;
            }
            break;
        }

    case GATT_SVC_HID_CFU_REPORT2_INPUT_CCCD_INDEX:
        {
            callback_data.msg_data.not_ind_data.index = GATT_SVC_HID_CFU_REPORT2_INPUT_CCCD_INDEX;
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_DISABLE;
            }
            break;
        }
    case GATT_SVC_HID_CFU_REPORT3_INPUT_CCCD_INDEX:
        {
            callback_data.msg_data.not_ind_data.index = GATT_SVC_HID_CFU_REPORT3_INPUT_CCCD_INDEX;
            if (ccc_bits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_ENABLE;
            }
            else
            {
                callback_data.msg_data.not_ind_data.value = NOTIFY_DISABLE;
            }
            break;
        }
    }

    if (pfn_hids_cb && (cause == true))
    {
        pfn_hids_cb(service_id, (void *)&callback_data);
    }

    return;
}


/**
 * @brief       Send HIDS notification data .
 *
 *
 * @param[in]   conn_id  Connection handle.
 * @param[in]   report_id  Report id.
 * @param[in]   index  hids characteristic index.
 * @param[in]   p_data report value pointer.
 * @param[in]   data_len length of report data.
 * @return Operation result.
 * @retval true Operation success.
 * @retval false Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        uint8_t hid_report_input[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
        hids_send_report(le_get_conn_handle(conn_id), 2A, hid_report_input, sizeof(hid_report_input));
    }
 * \endcode
 */
bool hids_cfu_send_report(uint8_t conn_handle, uint8_t report_id, uint8_t *p_data,
                          uint16_t data_len)
{
    bool ret = true;
    PROFILE_PRINT_INFO3("hids_cfu_send_report conn_handle %d, report id %d, data_len %d", conn_handle,
                        report_id, data_len);
    if (report_id == HIDS_CFU_REPORT_1)
    {
        return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, service_id,
                                  GATT_SVC_HID_CFU_REPORT1_INPUT_INDEX, p_data,
                                  data_len, GATT_PDU_TYPE_NOTIFICATION);
    }
    else if (report_id == HIDS_CFU_REPORT_2)
    {
        return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, service_id,
                                  GATT_SVC_HID_CFU_REPORT2_INPUT_INDEX, p_data,
                                  data_len, GATT_PDU_TYPE_NOTIFICATION);
    }
    else if (report_id == HIDS_CFU_REPORT_3)
    {
        return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, service_id,
                                  GATT_SVC_HID_CFU_REPORT3_INPUT_INDEX, p_data,
                                  data_len, GATT_PDU_TYPE_NOTIFICATION);
    }
    else
    {
        //non-normal
        ret = false;
    }
    return ret;
}



uint16_t hids_attr_tbl_len = sizeof(hids_attr_tbl);

const T_FUN_GATT_EXT_SERVICE_CBS hids_cbs =
{
    hids_attr_read_cb,      // Read callback function pointer
    hids_attr_write_cb,     // Write callback function pointer
    hids_cccd_update_cb,    // Authorization callback function pointer
};

/**
  * @brief       Add HID service to the BLE stack database.
  *
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval Others Service id assigned by stack.
  *
  * <b>Example usage</b>
  * \code{.c}
     void profile_init()
     {
         server_init(1);
         hids_id = hids_add_service(app_handle_profile_message);
     }
  * \endcode
  */
T_SERVER_ID hids_add_service(void *p_func)
{
    if (false == gatt_svc_add(&service_id, (uint8_t *)hids_attr_tbl, hids_attr_tbl_len, &hids_cbs,
                              NULL))
    {
        PROFILE_PRINT_ERROR1("hids_add_service: ServiceId %d", service_id);
        service_id = 0xff;
    }

    pfn_hids_cb = (P_FUN_EXT_SERVER_GENERAL_CB)p_func;
    return service_id;
}
