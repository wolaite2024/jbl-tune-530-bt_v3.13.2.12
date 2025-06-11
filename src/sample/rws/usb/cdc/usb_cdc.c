#if F_APP_USB_CDC_SUPPORT
#include <string.h>
#include <stdlib.h>
#include "os_queue.h"
#include "os_sync.h"
#include "usb_spec20.h"
#include "trace.h"
#include "cdc.h"
#include "usb_cdc.h"
#include "usb_pipe.h"

static T_USB_INTERFACE_DESC cdc_std_if_desc =
{
    .bLength            = sizeof(T_USB_INTERFACE_DESC),
    .bDescriptorType    = USB_DESC_TYPE_INTERFACE,
    .bInterfaceNumber   = 0,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 1,
    .bInterfaceClass    = USB_CLASS_CODE_COMM,
    .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
    .bInterfaceProtocol = 0,
    .iInterface         = 0,
};

static T_CDC_HEADER_FUNC_DESC cdc_header_desc =
{
    .bFunctionLength = sizeof(T_CDC_HEADER_FUNC_DESC),
    .bDescriptorType = USB_DESCTYPE_CLASS_INTERFACE,
    .bDescriptorSubtype = 0,
    .bcdCDC = 0x0110
};

static T_CM_FUNC_DESC cm_func_desc =
{
    .bFunctionLength = sizeof(T_CM_FUNC_DESC),
    .bDescriptorType = USB_DESCTYPE_CLASS_INTERFACE,
    .bDescriptorSubtype = USB_CDC_SUBCLASS_CM,
    .bmCapabilities = 0x03,
    .bDataInterface = 1
};

static T_CDC_ACM_FUNC_DESC acm_func_desc =
{
    .bFunctionLength = sizeof(T_CDC_ACM_FUNC_DESC),
    .bDescriptorType = USB_DESCTYPE_CLASS_INTERFACE,
    .bDescriptorSubtype = USB_CDC_SUBCLASS_ACM,
    .bmCapabilities = 0x02,
};

static T_CDC_UNION_FUNC_DESC cdc_union_desc =
{
    .bFunctionLength = sizeof(T_CDC_UNION_FUNC_DESC),
    .bDescriptorType = USB_DESCTYPE_CLASS_INTERFACE,
    .bDescriptorSubtype = 0x06,
    .bControlInterface = 0,   // intf num of cdc_std_if_desc
    .bSubordinateInterface = 1   // intf num of cdc_std_data_if_desc
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

static T_USB_INTERFACE_DESC cdc_std_data_if_desc =
{
    .bLength = sizeof(T_USB_INTERFACE_DESC),
    .bDescriptorType = USB_DESC_TYPE_INTERFACE,
    .bInterfaceNumber = 1,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = USB_CLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0, // Common AT commands
    .iInterface = 0
};

static T_USB_ENDPOINT_DESC bulk_in_ep_desc_fs =
{
    .bLength           = sizeof(T_USB_ENDPOINT_DESC),
    .bDescriptorType   = USB_DESC_TYPE_ENDPOINT,
    .bEndpointAddress  = USB_DIR_IN | 0x02,
    .bmAttributes      = USB_EP_TYPE_BULK,
    .wMaxPacketSize    = 0x40,
    .bInterval         = 0,
};

static T_USB_ENDPOINT_DESC bulk_in_ep_desc_hs =
{
    .bLength           = sizeof(T_USB_ENDPOINT_DESC),
    .bDescriptorType   = USB_DESC_TYPE_ENDPOINT,
    .bEndpointAddress  = USB_DIR_IN | 0x02,
    .bmAttributes      = USB_EP_TYPE_BULK,
    .wMaxPacketSize    = 0x200,
    .bInterval         = 0,
};

static T_USB_ENDPOINT_DESC bulk_out_ep_desc_fs =
{
    .bLength           = sizeof(T_USB_ENDPOINT_DESC),
    .bDescriptorType   = USB_DESC_TYPE_ENDPOINT,
    .bEndpointAddress  = USB_DIR_OUT | 0x02,
    .bmAttributes      = USB_EP_TYPE_BULK,
    .wMaxPacketSize    = 0x40,
    .bInterval         = 0,
};

static T_USB_ENDPOINT_DESC bulk_out_ep_desc_hs =
{
    .bLength           = sizeof(T_USB_ENDPOINT_DESC),
    .bDescriptorType   = USB_DESC_TYPE_ENDPOINT,
    .bEndpointAddress  = USB_DIR_OUT | 0x02,
    .bmAttributes      = USB_EP_TYPE_BULK,
    .wMaxPacketSize    = 0x200,
    .bInterval         = 0,
};

static void *cdc_if0_descs_fs[] =
{
    (void *) &cdc_std_if_desc,
    (void *) &cdc_header_desc,
    (void *) &cm_func_desc,
    (void *) &acm_func_desc,
    (void *) &cdc_union_desc,
    (void *) &int_in_ep_desc_fs,
    NULL,
};

static void *cdc_if0_descs_hs[] =
{
    (void *) &cdc_std_if_desc,
    (void *) &cdc_header_desc,
    (void *) &cm_func_desc,
    (void *) &acm_func_desc,
    (void *) &cdc_union_desc,
    (void *) &int_in_ep_desc_hs,
    NULL,
};

static void *cdc_if1_descs_fs[] =
{
    (void *) &cdc_std_data_if_desc,
    (void *) &bulk_in_ep_desc_fs,
    (void *) &bulk_out_ep_desc_fs,
    NULL,
};

static void *cdc_if1_descs_hs[] =
{
    (void *) &cdc_std_data_if_desc,
    (void *) &bulk_in_ep_desc_hs,
    (void *) &bulk_out_ep_desc_hs,
    NULL,
};

void *usb_cdc_data_pipe_open(uint8_t ep_addr, T_USB_CDC_ATTR attr, uint8_t pending_req_num,
                             USB_CDC_DATA_PIPE_CB cb)
{
    T_USB_CDC_DRIVER_ATTR driver_attr;
    memcpy(&driver_attr, &attr, sizeof(T_USB_CDC_DRIVER_ATTR));
    return usb_cdc_driver_data_pipe_open(ep_addr, driver_attr, pending_req_num, cb);
}

bool usb_cdc_data_pipe_send(void *handle, void *buf, uint32_t len)
{
    usb_cdc_driver_data_pipe_send(handle, buf, len);
    return true;
}

int usb_cdc_data_pipe_close(void *handle)
{
    return usb_cdc_driver_data_pipe_close(handle);
}

void *cdc_in_handle = NULL;
void *cdc_out_handle = NULL;

uint32_t out_pipe_complete(void *handle, void *buf, uint32_t len, int status)
{
    USB_PRINT_INFO2("out_pipe_complete, len %d, status %d", len, status);
    usb_cdc_data_pipe_send(cdc_in_handle, buf, len);
    return 0;
}

void usb_cdc_init(void)
{
    void *inst0 = usb_cdc_driver_inst_alloc();
    usb_cdc_driver_if_desc_register(inst0, (void *)cdc_if0_descs_hs, (void *)cdc_if0_descs_fs);
    void *inst1 = usb_cdc_driver_inst_alloc();
    usb_cdc_driver_if_desc_register(inst1, (void *)cdc_if1_descs_hs, (void *)cdc_if1_descs_fs);

    usb_cdc_driver_init();

    T_USB_CDC_ATTR attr =
    {
        .zlp = 1,
        .high_throughput = 0,
        .congestion_ctrl = CDC_CONGESTION_CTRL_DROP_CUR,
        .rsv = 0,
        .mtu = 512
    };
    if (!cdc_in_handle)
    {
        cdc_in_handle = usb_cdc_data_pipe_open(CDC_BULK_IN_EP, attr, 2,
                                               NULL);
    }
    if (!cdc_out_handle)
    {
        cdc_out_handle = usb_cdc_data_pipe_open(CDC_BULK_OUT_EP, attr, 2,
                                                out_pipe_complete);
    }
}
#endif
