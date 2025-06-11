#if F_APP_USB_HID_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "os_queue.h"
#include "os_sync.h"
#include "usb_hid_desc.h"
#include "usb_spec20.h"
#include "trace.h"
#include "hid.h"
#include "usb_hid.h"
#include "usb_pipe.h"
#if F_APP_CFU_FEATURE_SUPPORT
#include "app_hid_descriptor.h"
#endif
static const char report_descs[] =
{
#if F_APP_CFU_FEATURE_SUPPORT
    APP_CFU_HID_DESC_ATTRIB,
#endif
    HID_REPORT_DESCS
};

typedef struct _t_hid_ual
{
    struct _t_hid_ual *p_next;
    T_HID_CBS cbs;
} T_HID_UAL;

T_OS_QUEUE ual_list;

static T_USB_INTERFACE_DESC hid_std_if_desc =
{
    .bLength            = sizeof(T_USB_INTERFACE_DESC),
    .bDescriptorType    = USB_DESC_TYPE_INTERFACE,
    .bInterfaceNumber   = 0,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 1,
    .bInterfaceClass    = USB_CLASS_CODE_HID,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface         = 0,
};

static T_HID_CS_IF_DESC  hid_cs_if_desc =
{
    .bLength            = sizeof(T_HID_CS_IF_DESC),
    .bDescriptorType    = DESC_TYPE_HID,
    .bcdHID             = 0x0110,
    .bCountryCode       = 0,
    .bNumDescriptors    = 1,
    .desc[0]            =
    {
        .bDescriptorType = DESC_TYPE_REPORT,
        .wDescriptorLength = sizeof(report_descs),
    },

};

static T_USB_ENDPOINT_DESC int_in_ep_desc_fs =
{
    .bLength           = sizeof(T_USB_ENDPOINT_DESC),
    .bDescriptorType   = USB_DESC_TYPE_ENDPOINT,
    .bEndpointAddress  = USB_DIR_IN | 0x01,
    .bmAttributes      = USB_EP_TYPE_INT,
    .wMaxPacketSize    = 0x40,
    .bInterval         = 1,
};

static T_USB_ENDPOINT_DESC int_in_ep_desc_hs =
{
    .bLength           = sizeof(T_USB_ENDPOINT_DESC),
    .bDescriptorType   = USB_DESC_TYPE_ENDPOINT,
    .bEndpointAddress  = USB_DIR_IN | 0x01,
    .bmAttributes      = USB_EP_TYPE_INT,
    .wMaxPacketSize    = 0x40,
    .bInterval         = 4,
};

static void *hid_if_descs_fs[] =
{
    (void *) &hid_std_if_desc,
    (void *) &hid_cs_if_desc,
    (void *) &int_in_ep_desc_fs,
    NULL,
};

static void *hid_if_descs_hs[] =
{
    (void *) &hid_std_if_desc,
    (void *) &hid_cs_if_desc,
    (void *) &int_in_ep_desc_hs,
    NULL,
};

void *usb_hid_data_pipe_open(uint8_t ep_addr, T_USB_HID_ATTR attr, uint8_t pending_req_num,
                             USB_HID_DATA_PIPE_CB cb)
{
    T_USB_HID_DRIVER_ATTR driver_attr;
    memcpy(&driver_attr, &attr, sizeof(T_USB_HID_DRIVER_ATTR));
    return usb_hid_driver_data_pipe_open(ep_addr, driver_attr, pending_req_num, cb);
}

bool usb_hid_data_pipe_send(void *handle, void *buf, uint32_t len)
{
    usb_hid_driver_data_pipe_send(handle, buf, len);
    return true;
}

int usb_hid_data_pipe_close(void *handle)
{
    return usb_hid_driver_data_pipe_close(handle);
}

int usb_hid_ual_register(T_HID_CBS cbs)
{
    T_HID_UAL *ual_node = malloc(sizeof(T_HID_UAL));
    memcpy(&ual_node->cbs, &cbs, sizeof(T_USB_HID_DRIVER_CBS));
    os_queue_in(&ual_list, ual_node);
    return 0;
}

int32_t usb_hid_get_report(T_HID_DRIVER_REPORT_REQ_VAL req_value, void *buf, uint16_t *len)
{
    uint32_t ret = 0;
    T_HID_UAL *ual = (T_HID_UAL *)ual_list.p_first;
    while (ual)
    {
        if (ual->cbs.get_report)
        {
            ret += ual->cbs.get_report(req_value, buf, len);
        }
        ual = ual->p_next;
    }
    return ret;
}

int32_t usb_hid_set_report(T_HID_DRIVER_REPORT_REQ_VAL req_value, void *buf, uint16_t len)
{
    uint32_t ret = 0;
    T_HID_UAL *ual = (T_HID_UAL *)ual_list.p_first;
    while (ual)
    {
        if (ual->cbs.set_report)
        {
            ret += ual->cbs.set_report(req_value, buf, len);
        }
        ual = ual->p_next;
    }
    return ret;
}

void usb_hid_init(void)
{
    void *inst = usb_hid_driver_inst_alloc();
    os_queue_init(&ual_list);
    usb_hid_driver_if_desc_register(inst, (void *)hid_if_descs_hs, (void *)hid_if_descs_fs,
                                    (void *)report_descs);

    T_USB_HID_DRIVER_CBS cbs;
    cbs.get_report = usb_hid_get_report;
    cbs.set_report = usb_hid_set_report;
    usb_hid_driver_cbs_register(inst, &cbs);
    usb_hid_driver_init();
}
#endif
