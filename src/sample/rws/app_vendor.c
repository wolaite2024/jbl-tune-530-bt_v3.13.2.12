/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <string.h>
#include "gap.h"
#include "trace.h"
#include "gap_vendor.h"
#include "app_vendor.h"

#define HCI_VENDOR_SET_ADV_EXT_MISC 0xFC98
#define HCI_VENDOR_SET_ADV_EXT_MISC_SUBCODE 0x00

#define HCI_VENDOR_SET_LE_CH_MAP_REF_POLICY 0xFD80
#define HCI_VENDOR_SET_LE_CH_MAP_REF_POLICY_SUBCODE 0x1C

/**
 * @brief Set Advertising Extended Misc Command
 *
 * @param fix_channel  Aux ADV Fix Channel:
 * 0 Extended Advertising auxiliary packet channel unfixed.
 * 1 Extended Advertising auxiliary packet channel fixed to 1 (2406MHz).
 * @param offset  Aux ADV offset
 * 0~0xFF (Unit: slot)
 * The minimum value of the offset from a Extended Advertising packet to its auxiliary packet
 * The actual offset value might larger than Aux ADV offset because of collision with other protocol.
 * @return true  success
 * @return false fail
 */
bool app_vendor_set_adv_extend_misc(uint8_t fix_channel, uint8_t offset)
{
    uint16_t opcode = HCI_VENDOR_SET_ADV_EXT_MISC;
    uint8_t params[3];
    uint8_t params_len = 3;

    params[0] = HCI_VENDOR_SET_ADV_EXT_MISC_SUBCODE;
    params[1] = fix_channel;
    params[2] = offset;

    return gap_vendor_cmd_req(opcode, params_len, params);
}

/**
 * @brief Set Afh Policy Priority Command
 *
 * @param lea_conn_handle(2 byte)  CIS Connect Handle
 * @param afh policy priority (1 byte)
 * remote first = 0, local first = 1
 * @return true  success
 * @return false fail
 */
bool app_vendor_send_psd_policy(uint16_t lea_conn_handle)
{
    uint16_t opcode = HCI_VENDOR_SET_LE_CH_MAP_REF_POLICY;
    uint8_t params[4];
    uint8_t params_len = 4;

    params[0] = HCI_VENDOR_SET_LE_CH_MAP_REF_POLICY_SUBCODE;
    memcpy(&params[1], &lea_conn_handle, 2);
    params[3] = 1; /* local first; due to earbud is rx */

    return gap_vendor_cmd_req(opcode, params_len, params);
}

