/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     sd_define.h
* @brief    This file provides definition of sdcard.
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __SD_DEFINE_H
#define __SD_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SDIO SDIO
  * @brief SDIO driver module
  * @{
  */

/** @defgroup SDIO_Exported_constants SDIO Exported Constants
  * @{
  */

typedef enum
{
    SD_OK                             = (0), /* Communication is normal */
    /* Error type in interrupt status register */
    SD_CMD_RSP_TIMEOUT                = (1), /* Command response timeout */
    SD_CMD_CRC_FAIL                   = (2), /* Command response received (but CRC check failed) */
    SD_CMD_END_BIT_ERR                = (3), /* The end bit of a command response is 0 */
    SD_CMD_INDEX_ERR                  = (4), /* A command index error occur in the command response */
    SD_DATA_TIMEOUT                   = (5), /* Data time out */
    SD_DATA_CRC_FAIL                  = (6), /* Data bock sent/received (CRC check Failed) */
    SD_DATA_END_BIT_ERR               = (7),
    SD_CURRENT_LIMIT_ERR              = (8),
    SD_AUTO_CMD12_ERR                 = (9),
    SD_ADMA_ERR                       = (10),
    /* Card status */
    SD_ADDR_OUT_OF_RANGE              = (11), /* Argument of the command is out of the allowed range of this card */
    SD_ADDR_MISALIGNED                = (12), /* Misaligned address */
    SD_BLOCK_LEN_ERR                  = (13), /* Transferred block length is not allowed for the card or the number of transferred bytes does not match the block length */
    SD_ERASE_SEQ_ERR                  = (14), /* An error in the sequence of erase command occurs.*/
    SD_BAD_ERASE_PARAM                = (15), /* An Invalid selection for erase groups */
    SD_WRITE_PROT_VIOLATION           = (16), /* Attempt to program a write protect block */
    SD_CARD_IS_LOCKED                 = (17), /* Card is locked by host */
    SD_LOCK_UNLOCK_FAILED             = (18), /* Sequence or password error has been detected in unlock command or if there was an attempt to access a locked card */
    SD_COM_CRC_FAILED                 = (19), /* CRC check of the previous command failed */
    SD_ILLEGAL_CMD                    = (20), /* Command is not legal for the card state */
    SD_CARD_ECC_FAILED                = (21), /* Card internal ECC was applied but failed to correct the data */
    SD_CC_ERROR                       = (22), /* Internal card controller error */
    SD_GENERAL_UNKNOWN_ERROR          = (23), /* General or Unknown error */
    SD_CID_CSD_OVERWRITE              = (24), /* CID/CSD overwrite error */
    SD_WP_ERASE_SKIP                  = (25), /* only partial address space was erased */
    SD_CARD_ECC_DISABLED              = (26), /* Command has been executed without using internal ECC */
    SD_ERASE_RESET                    = (27), /* Erase sequence was cleared before executing because an out of erase sequence command was received */
    SD_APP_CMD                        = (28), /* The card will expect ACMD */
    SD_AKE_SEQ_ERROR                  = (29), /* Error in sequence of authentication. */
    SD_CARD_BUSY                      = (30),
    /* Present status register type */
    SD_CARD_INSERT                    = (31),
    SD_CARD_REMOVAL                   = (32),
    SD_SDIO_CMD_INHIBIT               = (34), /* Can not issue next command */
    SD_CMD_INHIBIT_BEFORE_RESET       = (35),
    SD_SDIO_CMD_INHIBIT_AFTER_RESET   = (36),
    SD_HOST_SW_RESET_ERROR            = (37),
    SD_SDIO_CMD_COMPLETE              = (39),
    SD_HOST_INTER_CLOCK_ERROR         = (40),
    /* command index error in Response2 */
    SD_CMD_ERROR                      = (41),
    SD_RSP_PATTERN_ERROR              = (42), /* Response 0 register */
    SD_RSP_VHS_ERROR                  = (43),
    /* address error in data tranmssion */
    SD_PROG_TIMEOUT                   = (50), /* Card is in program or receive status */
    SD_READ_EXCEED_MAX_LEN            = (51), /* The length of the read data is beyond the specified range */
    SD_WRITE_EXCEED_MAX_LEN           = (52), /* The length of the write data is beyond the specified range */
    /* error recovery */
    SD_CMD_LINE_RECOVERABLE_ERROR     = (61),
    SD_DAT_LINE_RECOVERABLE_ERROR     = (62),
    SD_NON_RECOVERABLE_ERROR          = (63),
    SD_FORCE_RECOVERY                 = (64),
    SD_NO_FREE_SPACE                  = (65),
    SD_ERROR_STATUS                   = (66),
    SD_TYPE_NOT_SUPPORT               = (0xFB),
    SD_IF_VIOLATION                   = (0xFE),
    SD_INVALID_PARAMETER              = (0xFF),
} T_SD_STATUS;

#ifdef __cplusplus
}
#endif

#endif /*__SD_DEFINE_H*/

/** End of group SDIO_Exported_constants
  * @}
  */

/** @} */ /* End of group SDIO */

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/


