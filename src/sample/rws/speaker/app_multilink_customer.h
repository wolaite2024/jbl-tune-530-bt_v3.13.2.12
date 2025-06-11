#ifndef _APP_MULTILINK_CUSTOMER_H_
#define _APP_MULTILINK_CUSTOMER_H_

#include "app_hfp.h"
#include "app_multilink.h"
#include "btm.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_MUTLILINK_SOURCE_PRIORITY_UI

#if F_APP_USB_AUDIO_SUPPORT
#define USB_AUDIO_LINK_SUPPORT  1
#else
#define USB_AUDIO_LINK_SUPPORT  0
#endif

#if F_APP_LINEIN_SUPPORT
#define LINEIN_AUDIO_LINK_SUPPORT   1
#else
#define LINEIN_AUDIO_LINK_SUPPORT   0
#endif

#define CUSTOMER_APP_MAX_LINK_NUM   (MULTILINK_SRC_CONNECTED + USB_AUDIO_LINK_SUPPORT + LINEIN_AUDIO_LINK_SUPPORT) // 2 bt multilink num, 1 usb link, 1 linein 

#if F_APP_USB_AUDIO_SUPPORT
#define USB_AUDIO_LINK_INDEX        (CUSTOMER_APP_MAX_LINK_NUM - USB_AUDIO_LINK_SUPPORT - LINEIN_AUDIO_LINK_SUPPORT)

#define APP_MULTILINK_USB_UPSTREAM_ACTIVE    (0x01)
#define APP_MULTILINK_USB_DOWNSTREAM_ACTIVE  (0x02)
#endif

#if F_APP_LINEIN_SUPPORT
#define LINE_IN_AUDIO_LINK_INDEX    (CUSTOMER_APP_MAX_LINK_NUM - LINEIN_AUDIO_LINK_SUPPORT)
#endif

#define MAX_PRIORITY_NUM    4

extern uint8_t multilink_usb_idx;
extern uint8_t multilink_line_in_idx;

typedef struct t_app_multilink_customer_device_mgr
{
    uint8_t idx;
    bool app_actived;
    bool hfp_profile_connected;
    uint8_t res;
    uint32_t connect_time;
    uint32_t app_end_time;
} T_APP_MULTILINK_CUSTOMER_DEVICE_MGR;

typedef struct t_app_multilink_customer_array_data
{
    uint8_t idx;

    uint8_t music_priority;
    T_APP_JUDGE_A2DP_EVENT music_event;
    uint8_t wait_resume_a2dp;

    uint8_t record_priority;
    bool record_status;
} T_APP_MULTILINK_CUSTOMER_ARRAY_DATA;

typedef enum t_app_multilink_customer_stream_event
{
    APP_MULTILINK_CUSTOMER_USB_UPSTREAM_START          = 0x00,
    APP_MULTILINK_CUSTOMER_USB_UPSTREAM_STOP           = 0x01,
    APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_START        = 0x02,
    APP_MULTILINK_CUSTOMER_USB_DOWNSTREAM_STOP         = 0x03,
    APP_MULTILINK_CUSTOMER_LINE_IN_DOWNSTREAM_START    = 0x04,
    APP_MULTILINK_CUSTOMER_LINE_IN_DOWNSTREAM_STOP     = 0x05,
} T_APP_MULTILINK_CUSTOMER_STREAM_EVENT;

void app_multilink_customer_init(void);
void app_multilink_customer_music_priority_set(uint8_t index, uint8_t priority);
void app_multilink_customer_music_event_handle(uint8_t index, T_APP_JUDGE_A2DP_EVENT event);
uint8_t app_multilink_customer_get_active_music_link_index(void);
uint8_t app_multilink_customer_get_active_index(void);
void app_multilink_customer_handle_link_connected(uint8_t link_index);
void app_multilink_customer_handle_link_disconnected(uint8_t link_index);
void app_multilink_customer_hfp_status_changed(uint8_t prev_call_status, uint8_t *bd_addr);
bool app_multilink_customer_line_in_start_check(void);
void app_multilink_customer_line_in_start(void);
void app_multilink_customer_line_in_stop(void);
void app_multilink_customer_line_in_plug(void);
void app_multilink_customer_line_in_unplug(void);

#if F_APP_GAMING_DONGLE_SUPPORT
bool app_multilink_customer_is_dongle_priority_higher(void);
void app_multilink_customer_set_dongle_priority(uint8_t link_index);
void app_multilink_customer_update_dongle_record_status(bool record_status);
#endif

void app_multilink_customer_sco_disconneted(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_MULTILINK_CUSTOMER_H_ */
