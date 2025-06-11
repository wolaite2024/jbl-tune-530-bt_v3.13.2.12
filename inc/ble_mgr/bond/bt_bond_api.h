#ifndef BT_BOND_MGR_H
#define BT_BOND_MGR_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "gap.h"
/**
 * @brief bt_bond_init
 * Initialize parameters of bt bond manager.
* \code{.c}
    static void app_bt_gap_init(void)
    {
        ......
        #if F_APP_BOND_MGR_SUPPORT
            bt_bond_init();
        #if ISOC_AUDIO_SUPPORT
            ble_audio_bond_init();
        #endif
        #endif
        ......
    }
 * \endcode
 */
void bt_bond_init(void);

/**
 * @brief bt_bond_mgr_handle_gap_msg
 * handle GAP message when receive GAP_MSG_APP_BOND_MANAGER_INFO
 * @param p_data
 * @return T_APP_RESULT
* \code{.c}
    static T_APP_RESULT app_ble_gap_cb(uint8_t cb_type, void *p_cb_data)
    {
        ......
        switch (cb_type)
        {
    #if F_APP_BOND_MGR_SUPPORT
        case GAP_MSG_APP_BOND_MANAGER_INFO:
            {
                result = bt_bond_mgr_handle_gap_msg(cb_data.p_le_cb_data);
            }
            break;
    #endif
        }
    }
 * \endcode
 */
T_APP_RESULT bt_bond_mgr_handle_gap_msg(void *p_data);

#ifdef __cplusplus
}
#endif

#endif /* FLASH_KEY_MGR_LE_H */
