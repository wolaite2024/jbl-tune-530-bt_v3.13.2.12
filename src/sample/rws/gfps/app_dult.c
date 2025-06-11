/*
 * Copyright (c) 2024, Realsil Semiconductor Corporation. All rights reserved.
 */
#if GFPS_FINDER_SUPPORT

#include "stdlib.h"
#include "trace.h"
#include "app_dult_svc.h"
#include "app_dult_device.h"
#include "app_dult.h"
#include "app_cfg.h"
#include "app_main.h"
#include "string.h"
#include "app_gfps_finder.h"
#include "gfps_find_my_device.h"
#include "gfps.h"

static T_DULT_NEAR_OWNER_STATE     dult_near_owner_state = DULT_SEPERATED;
static uint8_t     dult_conn_handle = 0xFF;
static T_SERVER_ID dult_service_id = 0xFF;
static uint16_t    dult_cid = 0xFFFF;

void app_dult_handle_power_on(void)
{
#if GFPS_FINDER_DULT_ADV_SUPPORT
    app_dult_device_start_adv(dult_near_owner_state);
#endif
}

T_DULT_NEAR_OWNER_STATE app_dult_get_none_owner_state(void)
{
    return dult_near_owner_state;
}

void app_dult_handle_none_owner_state(T_DULT_NEAR_OWNER_STATE state)
{
    APP_PRINT_INFO1("app_dult_handle_none_owner_state: new state %d", state);
    dult_near_owner_state = state;
#if GFPS_FINDER_DULT_ADV_SUPPORT
    app_dult_device_update_adv_addr();
    app_dult_device_update_adv_data(dult_near_owner_state);
    app_dult_device_start_update_timer(dult_near_owner_state);
#endif
}

void app_dult_sound_complete_handle(void)
{
    uint16_t opcode = DULT_OPCODE_CTRL_SOUND_COMPLETE;
    app_dult_send_indication(dult_conn_handle, dult_cid, dult_service_id, (uint8_t *)&opcode,
                             sizeof(opcode));

    app_dult_device_set_ring_state(DULT_RING_STATE_STOPPED);
}

static T_DULT_BATT_LEVEL get_current_batt_level(void)
{
    uint8_t cur_level = app_db.local_batt_level;
    if (cur_level < 10)
    {
        return DULT_BATT_CRITICALLY_LOW;
    }
    else if (cur_level < 20)
    {
        return DULT_BATT_LOW;
    }
    else if (cur_level < 90)
    {
        return DULT_BATT_MEDIUM;
    }
    else
    {
        return DULT_BATT_FULL;
    }
}

static void app_dult_handle_info_opcode(T_SERVER_ID service_id, T_DULT_CALLBACK_DATA *p_callback)
{
    uint8_t conn_handle = p_callback->conn_handle;
    uint16_t cid = p_callback->cid;

    T_DULT_OPCODE_INFO opcode = (T_DULT_OPCODE_INFO)p_callback->msg_data.opcode;
    uint8_t len = p_callback->msg_data.operands_len;
    uint8_t *p_operands = p_callback->msg_data.p_operands;

    APP_PRINT_INFO2("app_dult_handle_info_opcode: opcode %d, len %d", opcode, len);

    if (dult_near_owner_state == DULT_NEARING_OWNER)
    {
        app_dult_report_cmd_rsp(conn_handle, cid, service_id,
                                p_callback->msg_data.opcode, DULT_CMD_RSP_STATE_INVALID_CMD);
        return;
    }

    switch (opcode)
    {
    case DULT_OPCODE_INFO_GET_PRODUCT_DATA:
        {
            struct
            {
                uint16_t opcode;
                uint8_t  product_data[8];
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_PRODUCT_DATA_RSP;

            rpt.product_data[5] = extend_app_cfg_const.gfps_model_id[0];
            rpt.product_data[6] = extend_app_cfg_const.gfps_model_id[1];
            rpt.product_data[7] = extend_app_cfg_const.gfps_model_id[2];

            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_MANUFT_NAME:
        {
            struct
            {
                uint16_t opcode;
                uint8_t  manufacturer_name[64];
            } __attribute__((packed)) rpt = {};
            rpt.opcode = DULT_OPCODE_INFO_GET_MANUFT_NAME_RSP;
            // uint8_t name[] = DULT_MANUFACTURE_NAME;
            // memcpy(rpt.manufacturer_name, name, sizeof(name));
            memcpy(rpt.manufacturer_name, extend_app_cfg_const.gfps_company_name,
                   sizeof(extend_app_cfg_const.gfps_company_name));
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_MODEL_NAME:
        {
            struct
            {
                uint16_t opcode;
                uint8_t  model_name[64];
            } __attribute__((packed)) rpt = {};

            memcpy(rpt.model_name, extend_app_cfg_const.gfps_device_name,
                   sizeof(extend_app_cfg_const.gfps_device_name));
            rpt.opcode = DULT_OPCODE_INFO_GET_MODEL_NAME_RSP;
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_ACCESSORY_CATEGORY:
        {
            struct
            {
                uint16_t opcode;
                uint8_t  accessory_category[8];
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_ACCESSORY_CATEGORY_RSP;
            rpt.accessory_category[0] = extend_app_cfg_const.gfps_device_type;
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_PROTOCAL_VER:
        {
            struct
            {
                uint16_t opcode;
                uint32_t protocal_ver;
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_PROTOCAL_VER_RSP;
            rpt.protocal_ver = DULT_PROTOCAL_VERSION;
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_ACCESSORY_CAP:
        {
            struct
            {
                uint16_t opcode;
                uint32_t accessory_cap;
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_ACCESSORY_CAP_RSP;
            rpt.accessory_cap = DULT_ACCESSORY_CAP_PLAY_SOUND | DULT_ACCESSORY_CAP_LOOK_UP_BY_LE;
            APP_PRINT_INFO1("DULT_OPCODE_INFO_GET_ACCESSORY_CAP: %d", rpt.accessory_cap);
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_NETWORK_ID:
        {
            struct
            {
                uint16_t opcode;
                uint8_t  network_id;
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_NETWORK_ID_RSP;
            rpt.network_id = DULT_NETWORK_ID;
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_FIRMWARE_VER:
        {
            struct
            {
                uint16_t opcode;
                uint32_t firmware_ver;
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_FIRMWARE_VER_RSP;
            rpt.firmware_ver = DULT_FIRMWARE_VERSION;
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_BATT_TYPE:
        {
            struct
            {
                uint16_t opcode;
                uint8_t  battery_type;
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_BATT_TYPE_RSP;
            rpt.battery_type = DULT_BATTERY_TYPE;
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    case DULT_OPCODE_INFO_GET_BATT_LEVEL:
        {
            struct
            {
                uint16_t opcode;
                uint8_t  battery_level;
            } __attribute__((packed)) rpt = {};

            rpt.opcode = DULT_OPCODE_INFO_GET_BATT_LEVEL_RSP;
            rpt.battery_level = get_current_batt_level();
            app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("app_dult_handle_ctrl_opcode: unhandled opcode %d", opcode);
        }
        break;
    }
}

static void app_dult_handle_ctrl_opcode(T_SERVER_ID service_id, T_DULT_CALLBACK_DATA *p_callback)
{
    uint8_t conn_handle = p_callback->conn_handle;
    uint8_t cid = p_callback->cid;

    T_DULT_OPCODE_CTRL opcode = (T_DULT_OPCODE_CTRL)p_callback->msg_data.opcode;
    uint8_t len = p_callback->msg_data.operands_len;
    uint8_t *p_operands = p_callback->msg_data.p_operands;

    APP_PRINT_INFO2("app_dult_handle_ctrl_opcode: opcode 0x%x, len %d", opcode, len);

    switch (opcode)
    {
    case DULT_OPCODE_CTRL_SOUND_START:
        {
            if (dult_near_owner_state == DULT_NEARING_OWNER)
            {
                app_dult_report_cmd_rsp(p_callback->conn_handle, cid, service_id,
                                        p_callback->msg_data.opcode, DULT_CMD_RSP_STATE_INVALID_STATE);
            }
            else
            {
                dult_service_id = service_id;
                dult_conn_handle = conn_handle;
                dult_cid = cid;
                app_dult_device_sound_start(service_id, p_callback);
            }

        }
        break;

    case DULT_OPCODE_CTRL_SOUND_STOP:
        {
            if (dult_near_owner_state == DULT_NEARING_OWNER)
            {
                app_dult_report_cmd_rsp(p_callback->conn_handle, cid, service_id,
                                        p_callback->msg_data.opcode, DULT_CMD_RSP_STATE_INVALID_STATE);
            }
            else
            {
                dult_service_id = service_id;
                dult_conn_handle = conn_handle;
                dult_cid = cid;
                app_dult_device_sound_stop(service_id, p_callback);
            }
        }
        break;

    case DULT_OPCODE_CTRL_GET_IDENTIFIER:
        {
            if (app_dult_id_read_state_get())
            {
                uint8_t recovery_key[8];
                uint8_t ephemeral_identifier[32];
                app_gfps_get_ei(ephemeral_identifier);
                gfps_finder_get_recovery_key(recovery_key);
                uint8_t p_output[32];
                gfps_hmac_sha256(ephemeral_identifier, 10, p_output, recovery_key, 8);

                struct
                {
                    uint16_t opcode;
                    uint8_t  first_ten_ei[10];
                    uint8_t  encrypted_ei[8];
                } __attribute__((packed)) rpt = {};
                rpt.opcode = DULT_OPCODE_CTRL_GET_IDENTIFIER_RSP;
                memcpy(rpt.first_ten_ei, ephemeral_identifier, 10);
                memcpy(rpt.encrypted_ei, p_output, 8);
                app_dult_send_indication(conn_handle, cid, service_id, (uint8_t *)&rpt, sizeof(rpt));
            }
            else
            {
                app_dult_report_cmd_rsp(conn_handle, cid, service_id,
                                        DULT_OPCODE_CTRL_GET_IDENTIFIER, DULT_CMD_RSP_STATE_INVALID_STATE);
            }
        }
        break;

    default:
        {
            APP_PRINT_ERROR1("app_dult_handle_ctrl_opcode: unhandled opcode %d", opcode);
        }
        break;
    }
}

static bool is_info_op(uint16_t opcode)
{
    return (opcode >= DULT_OPCODE_INFO_GET_PRODUCT_DATA &&
            opcode <= DULT_OPCODE_INFO_MAX) ? true : false;
}

static bool is_ctrl_op(uint16_t opcode)
{
    return (opcode >= DULT_OPCODE_CTRL_SOUND_START && opcode <= DULT_OPCODE_CTRL_MAX ||
            opcode == DULT_OPCODE_CTRL_GET_IDENTIFIER) ? true : false;
}

static T_APP_RESULT app_dult_cb(T_SERVER_ID service_id, void *p_data)
{
    uint8_t ret = 0;
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;

    T_DULT_CALLBACK_DATA *p_callback = (T_DULT_CALLBACK_DATA *)p_data;
    APP_PRINT_INFO1("app_dult_cb: msg_type %d", p_callback->msg_type);

    switch (p_callback->msg_type)
    {
    case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
//            APP_PRINT_INFO1("app_dult_cb: notify_type %d", p_callback->msg_data.notify_type);
        }
        break;

    case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            if (is_info_op(p_callback->msg_data.opcode))
            {
                app_dult_handle_info_opcode(service_id, p_callback);
            }
            else if (is_ctrl_op(p_callback->msg_data.opcode))
            {
                app_dult_handle_ctrl_opcode(service_id, p_callback);
            }
            else
            {
                APP_PRINT_ERROR1("app_dult_cb: invalid opcode %d", p_callback->msg_data.opcode);
            }
        }
        break;

    default:
        break;
    }

    return app_result;
}

void app_dult_svc_init(void)
{
    dult_service_id = app_dult_add_service(app_dult_cb);
}

#endif
