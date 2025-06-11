/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BT_OPP_H_
#define _BT_OPP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * \defgroup    BT_OPP BT OPP Profile
 *
 * \brief   Provide BT OPP profile interfaces.
 */


/**
 * bt_opp.h
 *
 * \brief  BT OPP data header indication.
 *
 * \ingroup BT_OPP
 */
typedef struct t_bt_opp_data_header_ind
{
    uint8_t    *name;
    uint16_t    name_len;
    uint8_t    *type;
    uint16_t    type_len;
    uint32_t    total_len;
} T_BT_OPP_DATA_HEADER_IND;

/**
 * bt_opp.h
 *
 * \brief  BT OPP data indication.
 *
 * \ingroup BT_OPP
 */
typedef struct t_bt_opp_data_ind
{
    uint8_t    *p_data;
    uint16_t    data_len;
    bool        data_end;
} T_BT_OPP_DATA_IND;

/**
 * bt_opp.h
 *
 * \brief  BT OPP response code.
 *
 * \ingroup BT_OPP
 */
typedef enum t_bt_opp_response_code
{
    BT_OPP_RSP_CONTINUE                   = 0x90,
    BT_OPP_RSP_SUCCESS                    = 0xA0,
    BT_OPP_RSP_FORBIDDEN                  = 0xC3,
    BT_OPP_RSP_NOT_FOUND                  = 0xC4,
    BT_OPP_RSP_REQUESTED_ENTITY_TOO_LARGE = 0xCD,
    BT_OPP_RSP_UNSUPPORTED_MEDIA_TYPE     = 0xCF,
} T_BT_OPP_RESPONSE_CODE;

/**
 * bt_opp.h
 *
 * \brief   Initialize OPP profile.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  link_num          OPP maximum connected link number.
 * \param[in]  server_chann      RFCOMM Server channel to be allocated for OPP.
 * \param[in]  l2c_psm           L2CAP PSM to be allocated for OPP.
 *
 * \return          The status of initializing OPP profile.
 * \retval true     OPP profile was initialized successfully.
 * \retval false    OPP profile was failed to initialize.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_init(uint8_t  link_num,
                 uint8_t  server_chann,
                 uint16_t l2c_psm);

/**
 * bt_opp.h
 *
 * \brief   Accept or reject the incoming OPP Connection.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  accept     Accept or reject the connection indication.
 * \arg    true     Accept the connection indication.
 * \arg    false    Reject the connection indication
 *
 * \return         The status of sending the OPP connection confirmation.
 * \retval true    OPP connection confirmation was sent successfully.
 * \retval false   OPP connection confirmation was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_connect_cfm(uint8_t bd_addr[6],
                        bool    accept);

/**
 * bt_opp.h
 *
 * \brief  Send a OPP connection request over RFCOMM.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr         Remote BT address.
 * \param[in]  server_chann    The remote server channel defined in SDP record.
 *
 * \return         The status of sending the OPP connection request.
 * \retval true    OPP connection request was sent successfully.
 * \retval false   OPP connection request was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_connect_over_rfc_req(uint8_t bd_addr[6],
                                 uint8_t server_chann);

/**
 * bt_opp.h
 *
 * \brief  Send a OPP connection request over L2CAP.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr         Remote BT address.
 * \param[in]  l2c_psm         The remote L2CAP psm defined in SDP record.
 *
 * \return         The status of sending the OPP connection request.
 * \retval true    OPP connection request was sent successfully.
 * \retval false   OPP connection request was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_connect_over_l2c_req(uint8_t  bd_addr[6],
                                 uint16_t l2c_psm);

/**
 * bt_opp.h
 *
 * \brief  Send a OPP disconnection request.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the OPP disconnection request.
 * \retval true    OPP disconnection request was sent successfully.
 * \retval false   OPP disconnection request was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_disconnect_req(uint8_t bd_addr[6]);

/**
 * bt_opp.h
 *
 * \brief  Request to push data header.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr     Remote BT address.
 * \param[in]  total_len   Total length of data.
 * \param[in]  name        Name of the data. Unicode String.
 * \param[in]  name_len    Length of name.
 * \param[in]  type        Type of the data. ASCII String of MIME type.
 * \param[in]  type_len    Length of type.
 * \param[in]  srm_enable  Single response mode enable or not.
 *
 * \return         The status of pushing data header.
 * \retval true    Request was sent successfully.
 * \retval false   Request was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_push_data_header_req(uint8_t   bd_addr[6],
                                 uint32_t  total_len,
                                 uint8_t  *name,
                                 uint16_t  name_len,
                                 uint8_t  *type,
                                 uint16_t  type_len,
                                 bool      srm_enable);

/**
 * bt_opp.h
 *
 * \brief  Request to push data.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr     Remote BT address.
 * \param[in]  data        The data to be sent.
 * \param[in]  data_len    Length of data.
 * \param[in]  more_data   True if the data is partial and more data to send later.
 *
 * \return         The status of pushing data.
 * \retval true    Request was sent successfully.
 * \retval false   Request was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_push_data_req(uint8_t   bd_addr[6],
                          uint8_t  *data,
                          uint16_t  data_len,
                          bool      more_data);

/**
 * bt_opp.h
 *
 * \brief  Acknowledge on receiving an OPP data Indication.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  rsp_code   Response code on on receiving an OPP data.
 *
 * \return         The status of sending the response.
 * \retval true    Response was sent successfully.
 * \retval false   Response was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_send_data_rsp(uint8_t                bd_addr[6],
                          T_BT_OPP_RESPONSE_CODE rsp_code);

/**
 * bt_opp.h
 *
 * \brief  Request to terminate the push operation.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr     Remote BT address.
 *
 * \return         The status of sending push abort message.
 * \retval true    Request was sent successfully.
 * \retval false   Request was failed to send.
 *
 * \ingroup BT_OPP
 */
bool bt_opp_push_abort(uint8_t bd_addr[6]);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BT_OPP_H_ */
