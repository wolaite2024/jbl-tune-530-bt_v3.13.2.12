/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_vendor.h
  * @brief
  * @details
  * @author  ranhui_xia
  * @date    2017-08-02
  * @version v1.0
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
  ******************************************************************************
  */
#ifndef GAP_VNR_H
#define GAP_VNR_H

#include <stdint.h>
#include <stdbool.h>
#include "gap.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup GAP GAP Module
  * @{
  */

/** @addtogroup GAP_VENDOR GAP Vendor
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAP_Vendor_Exported_Types GAP Vendor Exported Types
  * @{
  */

typedef struct
{
    uint16_t cause;
} T_GAP_VENDOR_CAUSE;

typedef union
{
    T_GAP_VENDOR_CAUSE gap_vendor_cause;
} T_GAP_VENDOR_CB_DATA;

/** @brief Mode of setting priority. */
typedef enum
{
    GAP_VENDOR_UPDATE_PRIORITY, //!< Set priority without operation of resetting priority
    GAP_VENDOR_SET_PRIORITY,    //!< Set priority after operation of resetting priority
    GAP_VENDOR_RESET_PRIORITY,  //!< Reset priority
} T_GAP_VENDOR_SET_PRIORITY_MODE;

/** @brief Definition of priority level. */
typedef enum
{
    GAP_VENDOR_PRIORITY_LEVEL_0,
    GAP_VENDOR_PRIORITY_LEVEL_1,
    GAP_VENDOR_PRIORITY_LEVEL_2,
    GAP_VENDOR_PRIORITY_LEVEL_3,
    GAP_VENDOR_PRIORITY_LEVEL_4,
    GAP_VENDOR_PRIORITY_LEVEL_5,
    GAP_VENDOR_PRIORITY_LEVEL_6,
    GAP_VENDOR_PRIORITY_LEVEL_7,
    GAP_VENDOR_PRIORITY_LEVEL_8,
    GAP_VENDOR_PRIORITY_LEVEL_9,
    GAP_VENDOR_PRIORITY_LEVEL_10,  //!< Highest priority
    GAP_VENDOR_RESERVED_PRIORITY,
} T_GAP_VENDOR_PRIORITY_LEVEL;

/** @brief Mode of setting link priority. */
typedef enum
{
    GAP_VENDOR_NOT_SET_LINK_PRIORITY,       //!< Not set priority of link
    GAP_VENDOR_SET_SPECIFIC_LINK_PRIORITY,  //!< Set priority of specific links
    GAP_VENDOR_SET_ALL_LINK_PRIORITY,       //!< Set priority of all link
} T_GAP_VENDOR_SET_LINK_PRIORITY_MODE;

/** @brief Definition of common priority. */
typedef struct
{
    bool set_priority_flag;
    T_GAP_VENDOR_PRIORITY_LEVEL priority_level; /**< Priority is valid if set_priority_flag is true. */
} T_GAP_VENDOR_COMMON_PRIORITY;

/** @brief Definition of connection priority. */
typedef struct
{
    uint8_t conn_id;
    T_GAP_VENDOR_PRIORITY_LEVEL conn_priority_level;/**< Priority of specific connection. */
} T_GAP_VENDOR_CONN_PRIORITY;

/** @brief  Parameters of setting priority.*/
typedef struct
{
    T_GAP_VENDOR_SET_PRIORITY_MODE set_priority_mode;/**< Mode of setting priority.
                                                            (@ref T_GAP_VENDOR_SET_PRIORITY_MODE). */
    T_GAP_VENDOR_COMMON_PRIORITY adv_priority;/**< Priority of advertising.
                                                     (@ref T_GAP_VENDOR_COMMON_PRIORITY). */
    T_GAP_VENDOR_COMMON_PRIORITY scan_priority;/**< Priority of scan.
                                                      (@ref T_GAP_VENDOR_COMMON_PRIORITY). */
    T_GAP_VENDOR_COMMON_PRIORITY initiate_priority;/**< Priority of initiating.
                                                          (@ref T_GAP_VENDOR_COMMON_PRIORITY). */
    T_GAP_VENDOR_SET_LINK_PRIORITY_MODE link_priority_mode;/**< Mode of setting link priority.
                                                                  (@ref T_GAP_VENDOR_SET_LINK_PRIORITY_MODE). */
    T_GAP_VENDOR_PRIORITY_LEVEL link_priority_level;/**< Priority of all link is valid
                                                           if link_priority_mode is GAP_VENDOR_SET_ALL_LINK_PRIORITY. */
    uint8_t num_conn_ids;/**< Number of specific links is valid if link_priority_mode is GAP_VENDOR_SET_SPECIFIC_LINK_PRIORITY. */
    T_GAP_VENDOR_CONN_PRIORITY p_conn_id_list[1];/**< List of connection priority is valid
                                                        if link_priority_mode is GAP_VENDOR_SET_SPECIFIC_LINK_PRIORITY.
                                                        (@ref T_GAP_VENDOR_CONN_PRIORITY). */
} T_GAP_VENDOR_PRIORITY_PARAM;
/** End of GAP_Vendor_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup GAP_VENDOR_Exported_Functions GAP vendor command Exported Functions
  * @brief GAP vendor command Exported Functions
  * @{
  */

/**
 * @brief  Used for APP send vendor command to stack.
 * @param[in] op       Opcode of command.
 * @param[in] len      Length of parameters.
 * @param[in] p_param  Pointer to parameters to write. p_param[0] is subcode.
 * @return Operation result.
 * @retval true      Send request successfully.
 * @retval false     Failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    #define HCI_VENDOR_SET_ADV_EXT_MISC 0xFC98
    #define HCI_VENDOR_SET_ADV_EXT_MISC_SUBCODE 0x00

    bool app_vendor_set_adv_extend_misc(uint8_t fix_channel, uint8_t offset)
    {
        APP_PRINT_INFO0("app_vendor_set_adv_extend_misc");
        uint16_t opcode = HCI_VENDOR_SET_ADV_EXT_MISC;

        uint8_t params[3];
        uint8_t params_len = 3;

        params[0] = HCI_VENDOR_SET_ADV_EXT_MISC_SUBCODE;
        params[1] = fix_channel;
        params[2] = offset;

        return gap_vendor_cmd_req(opcode, params_len, params);
    }

    //register callback, and handle response
    void app_gap_init(void)
    {
        gap_register_app_cb(app_gap_common_callback);
    }

    void app_gap_common_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_GAP_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_GAP_CB_DATA));
        APP_PRINT_INFO1("app_gap_common_callback: cb_type = %d", cb_type);

        switch (cb_type)
        {
        case GAP_MSG_VENDOR_CMD_RSP:
            {
                APP_PRINT_INFO4("app_gap_common_callback: GAP_MSG_VENDOR_CMD_RSP command 0x%x, cause 0x%x, subcode %d, param_len %d",
                                cb_data.p_gap_vendor_cmd_rsp->command,
                                cb_data.p_gap_vendor_cmd_rsp->cause,
                                cb_data.p_gap_vendor_cmd_rsp->param[0],
                                cb_data.p_gap_vendor_cmd_rsp->param_len);
            }
        break;
       ......
    }
 * \endcode
 */
bool gap_vendor_cmd_req(uint16_t op, uint8_t len, uint8_t *p_param);

/**
 * @brief  Register callback to vendor command, when messages in @ref GAP_VENDOR_MSG_TYPE happens, it will callback to app.
 * @param[in] vendor_callback Callback function provided by the APP to handle @ref GAP_VENDOR_MSG_TYPE messages sent from the GAP.
 *              @arg NULL -> Not send @ref GAP_VENDOR_MSG_TYPE messages to APP.
 *              @arg Other -> Use application defined callback function.
 * @return void
 *
 * <b>Example usage</b>
 * \code{.c}
   void app_le_gap_init(void)
    {
        ......
        gap_vendor_register_cb(app_ble_gap_vendor_callback);
    }
    void app_ble_gap_vendor_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_GAP_VENDOR_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_GAP_VENDOR_CB_DATA));
        APP_PRINT_INFO1("app_ble_gap_vendor_callback: cb_type is %d", cb_type);
        switch (cb_type)
        {
        case GAP_MSG_GAP_VENDOR_SET_ANT_CTRL:
            APP_PRINT_INFO1("GAP_MSG_GAP_VENDOR_SET_ANT_CTRL: cause 0x%x",
                            cb_data.gap_vendor_cause.cause);
            break;
        default:
            break;
        }
        return;
    }
   \endcode
 */
void gap_vendor_register_cb(P_FUN_GAP_APP_CB vendor_callback);

/**
 * @brief   Set ext adv handle priority,the result will be notified by callback regisgtered
            by @ref le_register_app_cb.
 * @param[0] subcode @ref HCI_EXT_SUB_SET_EXT_ADV_HANDLE_PRIORITY
 * @param[1] handle_bitmap if want to set handle 0 priority, handle_bitmap bit0 set 1,
 *           if want to set handle 1 priority, handle_bitmap bit1 set 1
 *           You can set multiple bits at the same time
 * @param[2] priority_levels Used to store the priority of all handles,unit 10
 *           priority_levels[0] The handle priority level of the first pull 1 in handle_bitmap
 *           priority_levels[1] The handle priority level of the second pull 1 in handle_bitmap,if it exists.
 *           if it does not exist, set priority_levels[1] to 0.
 *           priority_levels[2] The handle priority level of the third pull 1 in handle_bitmap,if it exists.
 *           if it does not exist, set priority_levels[2] to 0.
 *           priority_levels[3] The handle priority level of the forth pull 1 in handle_bitmap,if it exists.
 *           if it does not exist, set priority_levels[3] to 0.
 * @param[3] num  The number of elements in priority_levels ,the default num is 4, and must be no greater than 4
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS Operation success.
 * @retval Others Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    static T_USER_CMD_PARSE_RESULT cmd_set_ext_adv_handle_priority(T_USER_CMD_PARSED_VALUE *p_parse_value)
    {
        //for example: Set the priority of handle 2 to 100
        T_GAP_CAUSE cause;
        uint8_t handle_bitmap = 0x04;// if want to set handle 2 priority, handle_bitmap bit2 set 1
        uint8_t priority_levels[4] = [10,0,0,0];//priority_levels[0]:The handle priority level of the first pull 1 in handle_bitmap, the unit is 10, priority_levels[0]* 10 = 100
        uit8_t num = 4;//The number of elements in priority_levels

        cause = gap_vendor_ext_adv_handle_priority_set(handle_bitmap,priority_levels,num);
        return (T_USER_CMD_PARSE_RESULT)cause;
    }

    le_register_app_cb(app_ble_gap_cb);

    void app_ble_gap_cb(uint8_t cb_type, void *p_cb_data)
    {
        T_GAP_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_GAP_CB_DATA));
        APP_PRINT_INFO1("app_ble_gap_cb: cb_type = %d", cb_type);
        switch (cb_type)
        {
        case GAP_MSG_EXT_ADV_HANDLE_PRIORITY_SET:
            APP_PRINT_INFO1("GAP_MSG_EXT_ADV_HANDLE_PRIORITY_SET: cause 0x%x",
                            cb_data.p_gap_set_bd_addr_rsp->cause);
            break;
        default:
            break;
        }
        return;
    }
 * \endcode
 */
T_GAP_CAUSE gap_vendor_ext_adv_handle_priority_set(uint8_t handle_bitmap, uint8_t *priority_levels,
                                                   uint8_t num);

/**
 *
 * \brief  Set vendor feature of LE Host
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bit_number
 * \arg    0  : Only be configured when scan is idle. Whether optimize process about extended advertising report. Disabled by default.
 * \arg    1  : Only be configured when scan is idle. Whether optimize process about legacy advertising report. Disabled by default.
 * \param[in]  bit_value
 * \arg    0  : Disable
 * \arg    1  : Enable
 *
 * \return         The result of set parameter request
 * \retval true    Set parameter request is successful
 * \retval false   Set parameter request is failed
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        bool retval = gap_vendor_le_set_host_feature(bit_number, bit_value);
    }
 * \endcode
 */
bool gap_vendor_le_set_host_feature(uint16_t bit_number, uint8_t bit_value);

/** End of GAP_VENDOR_Exported_Functions
  * @}
  */

/** End of GAP_VENDOR
  * @}
  */

/** End of GAP
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* GAP_VNR_H */
