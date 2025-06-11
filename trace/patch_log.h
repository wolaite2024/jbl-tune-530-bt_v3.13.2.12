#ifndef _PATCH_LOG_H_
#define _PATCH_LOG_H_

/*=======================================MODULE NAME====================================*/
//#define YOUR_LOG_INDEX        0
/*======================================================================================*/

/*=========================================PHY==========================================*/
#define PHY_START               0
#define PHY_LOG_001             1       //"[EFUSE_PHY_RECORD] Apply rck result from efuse = 0x%x"
#define PHY_LOG_002             2       //"[EFUSE_PHY_RECORD] Apply rxadck result from efuse = 0x%x"
#define PHY_LOG_003             3       //"[EFUSE_PHY_RECORD] Apply vco_4g8_currentk result from efuse = 0x%x"
#define PHY_LOG_004             4       //"[EFUSE_PHY_RECORD] Apply vco_6g4_currentk result from efuse = 0x%x"
#define PHY_LOG_005             5       //"[EFUSE_PHY_RECORD] Apply iqm_pad_currentk result from efuse = 0x%x"
#define PHY_LOG_006             6       //"[EFUSE_PHY_RECORD] Apply iqm_tx_pak_ft result from efuse = 0x%x"
#define PHY_LOG_007             7       //"[EFUSE_PHY_RECORD] Apply iqm_lna_currentk result from efuse = 0x%x"
#define PHY_LOG_008             8       //"[EFUSE_PHY_RECORD] Apply iqm_tia_currentk result from efuse = 0x%x"
#define PHY_LOG_009             9       //"[EFUSE_PHY_RECORD] Apply iqm_rxiqgen_currentk result from efuse = 0x%x"
#define PHY_LOG_010             10      //"[EFUSE_PHY_RECORD] Apply rx_iip2k result from efuse = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x"
#define PHY_LOG_011             11      //"[EFUSE_PHY_RECORD] Apply iqm_txgaink result from efuse = 0x%x"
#define PHY_LOG_012             12      //"[EFUSE_PHY_RECORD] Apply iqm_txgain_flatk result from efuse = %d, %d, %d, %d"
#define PHY_LOG_013             13      //"[EFUSE_PHY_RECORD] Apply iqm_rxgaink result from efuse = 0x%x"
#define PHY_LOG_014             14      //"[EFUSE_PHY_RECORD] Apply iqk_lok result from efuse = 0x%x, 0x%x, 0x%x"
#define PHY_LOG_015             15      //"[EFUSE_PHY_RECORD] Apply thermal result from efuse = 0x%x, 0x%x"
#define PHY_LOG_016             16      //"BR_max_txgain_index = 0x%x, tx power = %d (unit:0.5dBm)"
#define PHY_LOG_017             17      //"[TX_PWR_TRACK] enable_0p25_comp_for_thermal_tracking = %d, txgain_offset_x1000 = %d, txgain_offset_x1000_floor = %d"
#define PHY_LOG_018             18      //"[TX_PWR_TRACK] tx_power_table_offset = %d"
#define PHY_LOG_019             19      //"IQK/LOK result: lok:0x%x , iqk_x:0x%x , iqk_y:0x%x"
#define PHY_LOG_020             20      //"[TXGAIN_FLATK] larger than 3dB: 0x%x"
#define PHY_LOG_021             21      //"[TXGAIN_FLATK] smaller than -4dB: 0x%x"
#define PHY_LOG_022             22      //"[TXGAIN_FLATK] res_0p25dB_part[%d] = 0x%x"
#define PHY_LOG_023             23      //"[TXGAIN_FLATK] rfc = %d, modem = %d, fw = %d"
#define PHY_LOG_024             24      //"[TXGAIN_FLATK] rfc_comp[%d] = 0x%x"
#define PHY_LOG_025             25      //"[TXGAIN_FLATK] modem_flatk = 0x%x"
#define PHY_LOG_026             26      //"thermal_tracking_register_callback_func: allocate fail"
#define PHY_LOG_027             27      //"packet_parameter is not assigned"
#define PHY_LOG_029             29      //"pouter do not support txgain_index type setting"
#define PHY_LOG_030             30      //"PHY power control = %d"
/*======================================================================================*/

/*=======================================AUDIO=====================================*/
#define AUDIO_START              2000
#define AUD_LOG_IMG_HDR_ERR      2001 //"check image header fail %d scenario 0x%04X"
/*======================================================================================*/

/*=======================================FLASH=====================================*/
#define FLASH_START                                     4000
#define FLASH_BP_LEVEL                                  4001 //"[Flash Nor SPIC0] BP Lv = %d"
#define FTL_INIT_PARAM_ERROR                            4002 //"[FTL] init param error"
#define FTL_INIT_ALREADY                                4003 //"[FTL] module has been initialized already.offset:0x%x"
#define FTL_MALLOC_FAIL                                 4004 //"[FTL][%d] malloc fail"
#define FTL_MODULE_INFO                                 4005 //"[FTL][IOCTL][Cmd=%d] Addr: 0x%x, PAGE num: %d, element: %d, free: %d Curr: %d, FreeCell_idx: %d"
#define FTL_MODULE_PAGE_INFO                            4006 //"[FTL][IOCTL] PAGE HEAD(%x, %x, %x, %x), PAGE TAIL(%x, %x, %x, %x)"
#define FTL_PAGE_FORMAT                                 4007 //"[FTL][%d] page format 0x%x seq: %d"
#define FTL_PAGE_FORMAT_FAIL                            4008 //"[FTL] module in 0x%08x init fail"
#define FTL_PAGE_ALREADY_ERASED                         4009 //"[FTL][%d] already erased page: 0x%x"
#define FTL_READ_ADDR_PARSE_ERROR                       4010 //"[FTL][%d] logical_addr parse error!"
#define FTL_READ_CRC_CHECK_ERROR                        4011 //"[FTL][%d] crc check error!"
#define FTL_READ_ERROR                                  4012 //"[FTL][%d] flash read addr 0x%x error!"
#define FTL_WRITE_SKIP                                  4013 //"[FTL][%d] skip write"
#define FTL_WRITE_IN_ISR                                4014 //"[FTL] can't write in interrupt 0x%x!"
#define FTL_CMD_ERASE_PAGE                              4015 //"[FTL][IOCTL][Cmd=%d] free_page:0x%x"
#define FTL_GC_INFO                                     4016 //"[FTL][%d] GC - free (page_cnt, cell_idx)=(%d, %d)"
#define FTL_RECOVER_FAIL                                4017 //"[FTL] recover fail %d! clean all"
#define FLASH_CLK_SWITCH_INFO                           4018 //"flash io clock larger than 33MHz, cannot roll back to 1 bit mode"
#define FLASH_GET_BIT_MODE_ERROR                        4019 //"get bit mode failed: %d rd_ch: %d, wr_ch: %d"
#define FLASH_DMA_CHANNEL_ERROR                         4020 //"[Flash Nor] dma handler cannot find corresponding channel vector_num: %d"
#define FLASH_DMA_CMD_NODE_NOT_FOUND                    4021 //"[Flash Nor] DMA handler cmd_node not found error"
#define FLASH_DMA_ADDR_INVAILD                          4022 //"[Flash Nor] DMA transaction addr invalid - src = 0x%08x, dst = 0x%08x
#define FLASH_CS_SUSPEND_INFO                           4023 //"[Flash Nor] Context switch suspend cmd node - owner = 0x%08x"
#define FLASH_CS_RESUME_INFO                            4024 //"[Flash Nor] Context switch resume cmd node - owner = 0x%08x"
#define FLASH_DMA_LOCK_SUCCESS                          4025 //"[Flash Nor] Flash dma lock successfully cmd_node = %x"
#define FLASH_DMA_LOCK_INFO                             4026 //"[Flash Nor] Flash dma lock - FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x"
#define FLASH_DMA_CHANNEL_SUCCESS                       4027 //"[Flash Nor] Flash dma finish - vector_num = %x, dma ch = %x"
#define FLASH_DMA_SUC_CMD_NODE                          4028 //"[Flash Nor] Flash dma command node = %x"
#define FLASH_READ_WRITE_BLOCK                          4029 //"[Flash Nor] Flash rw block command node = %x, FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x, status = %d, owner = %x"
#define FLASH_RW_LOCK_INFO                              4030 //"[Flash Nor] Flash rw lock info - FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x"
#define FLASH_DMA_SUSPEND_INFO                          4031 //"[Flash Nor] Suspend DMA remain retry time = %d, src = %x, dst = %x"
#define FLASH_RW_LOCK_ISR_DMA                           4032 //"[Flash Nor] RW lock in ISR!"
#define FLASH_RW_LOCK_TASK_DMA                          4033 //"[Flash Nor] RW lock in task -- %x"
#define FLASH_RW_LOCK_TASK_SUSPEND                      4034 //"[Flash Nor] RW lock suspend dma"
#define FLASH_SUSPEND_DMA_INFO                          4035 //"[Flash Nor] dma ctrl low = %x, ctrl high = %x, sar = %x, dar = %x, CFG l = %x, CFG h = %x"
#define FLASH_DMA_BLOCK_NODE                            4036 //"[Flash Nor] Flash dma block command node = %x, FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x, status = %d, owner = %x"
#define FLASH_UNLOCK_RESUME_NODE                        4037 //"[Flash Nor] Flash unlock resume node = %x, FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x, status = %d, owner = %x"
#define FLASH_UNLOCK_SUCCESS                            4038 //"[Flash Nor] Flash unlock success FLASH_NOR_REQ_TYPE = %x"
#define FLASH_RESUME_HIGHEST                            4039 //"[Flash Nor] Flash resume highest priority"
#define FLASH_CS_RESUME_DMA                             4040 //"[Flash Nor] Flash resume highest priority in cs"
#define FLASH_RESUME_HIGHEST_IN_ERASE                   4041 //"[Flash Nor] Flash resume highest priority in erase lock"
#define FLASH_RESUME_HIGHEST_IN_DMA                     4042 //"[Flash Nor] Flash resume highest priority in dma lock"
#define FLASH_SUS_ALL_BUSY_NODE                         4043 //"[Flash Nor] flash_nor_suspend_all_busy_nodes"
#define FLASH_DMA_RESUME_HIGHEST_NODE                   4044 //"[Flash Nor] Flash resume highest dma command node = %x, FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x, status = %d, owner = %x"
#define FLASH_DMA_GET_BLOCK_IN_RES_HIGH                 4045 //"[Flash Nor] block node in highest dma resume node = %x, FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x, status = %d, owner = %x"
#define FLASH_ERASE_START                               4046 //"[Flash Nor] Flash erase lock"
#define FLASH_RESUME_NODE_IN_CS                         4047 //"[Flash Nor] Flash resume node in cs = %x, FLASH_NOR_REQ_TYPE = %x, addr = %x, len = %x, status = %d, owner = %x"
#define FLASH_DMA_GET_OFFSET_INVALID                    4048 //"[Flash Nor] DMA transaction get offset failed - src = 0x%08x, dst = 0x%08x"
#define FLASH_CAL_OUT_OF_MALLOC_SPACE                   4049 //"[Flash Nor] Avaliable setting number is larger than the malloc space!"
#define FLASH_CAL_FAILED_ERROR_LOG                      4050 //"[Flash Nor] Calibration failed! ret = %x, line = %d"
#define FLASH_ERR_RESUME_HIGHEST_DMA_ERASE_NODE_INFO    4051 //"[Flash Nor] Should not exist a erase blocking node here. erase node req = %x, addr = %x, len = %x, status = %x, owner = %x"
#define FLASH_DMA_SUSPEND_ERROR                         4052 //"[Flash Nor] Suspend DMA failed!"
#define FLASH_ERR_RESUME_HIGHEST_DMA_NODE_INFO          4053 //"[Flash Nor] dma node req = %x, addr = %x, len = %x, status = %x, owner = %x"
#define FLASH_STATUS_IN_ERASE_SUSPEND                   4054 //"[Flash Nor] sus progress - wip = %d"

/*======================================================================================*/

/*========================================CHARGER=======================================*/
#define CHG_LOG_START            13000
#define CHG_LOG_001              13001   //"charger_init: MBIAS_POW_BIAS_500nA %d"
#define CHG_LOG_002              13002   //"charger_init: CHG_POW_M1 %d, CHG_POW_M2 %d"
#define CHG_LOG_003              13003   //"charger_adc_init: Timer Start Fail"
#define CHG_LOG_004              13004   //"charger_adc_init: ADC Register Request Fail"
#define CHG_LOG_005              13005   //"charger_adc_init: xTimerPendFunctionCallFromISR fail."
#define CHG_LOG_006              13006   //"charger_adc_deinit: Timer Stop Fail"
#define CHG_LOG_007              13007   //"charger_adc_deinit: Timer Delete Fail"
#define CHG_LOG_008              13008   //"[CHARGER_STATE] --> STATE_CHARGER_START"
#define CHG_LOG_009              13009   //"[CHARGER_STATE] --> STATE_CHARGER_PRE_CHARGE"
#define CHG_LOG_010              13010   //"[CHARGER_STATE] --> STATE_CHARGER_FAST_CHARGE"
#define CHG_LOG_011              13011   //"[CHARGER_STATE] --> STATE_CHARGER_FINISH"
#define CHG_LOG_012              13012   //"[CHARGER_STATE] --> STATE_CHARGER_ERROR (ErrorCode: %d)"
#define CHG_LOG_013              13013   //"[CHARGER_STATE] --> STATE_CHARGER_END"
#define CHG_LOG_014              13014   //"[CHARGER_STATE] Total Time Stayed: %d ms"
#define CHG_LOG_015              13015   //"[CHARGER_STATE] STATE_CHARGER_START"
#define CHG_LOG_016              13016   //"[CHARGER_STATE] STATE_CHARGER_PRE_CHARGE"
#define CHG_LOG_017              13017   //"[CHARGER_STATE] STATE_CHARGER_FAST_CHARGE"
#define CHG_LOG_018              13018   //"[CHARGER_STATE] STATE_CHARGER_FINISH"
#define CHG_LOG_019              13019   //"[CHARGER_STATE] STATE_CHARGER_ERROR (ErrorCode: %d)"
#define CHG_LOG_020              13020   //"[CHARGER_STATE] STATE_CHARGER_END"
#define CHG_LOG_021              13021   //"[CURRENT_AVERAGE] %d"
#define CHG_LOG_022              13022   //"[SELF_CALIBRATION] %d"
#define CHG_LOG_023              13023   //"[BATTERY_STATE] %d %d %d %d %d"
#define CHG_LOG_024              13024   //"[STATE_OF_CHARGE] %d %%"
#define CHG_LOG_025              13025   //"charger_timer_handler: Charger FSM Update Fail"
#define CHG_LOG_026              13026   //"discharger_init: MBIAS_POW_BIAS_500nA %d"
#define CHG_LOG_027              13027   //"discharger_adc_init: Timer Start Fail"
#define CHG_LOG_028              13028   //"discharger_adc_init: ADC Register Request Fail"
#define CHG_LOG_029              13029   //"discharger_adc_init: xTimerPendFunctionCallFromISR fail."
#define CHG_LOG_030              13030   //"discharger_adc_deinit: Timer Stop Fail"
#define CHG_LOG_031              13031   //"discharger_adc_deinit: Timer Delete Fail"
#define CHG_LOG_032              13032   //"[BATTERY_STATE] %d %d"
#define CHG_LOG_033              13033   //"[STATE_OF_CHARGE] %d %%"
#define CHG_LOG_034              13034   //"discharger_timer_handler: Timer Start Fail"
#define CHG_LOG_035              13035   //"discharger_timer_handler: Discharger SOC Update Fail"
#define CHG_LOG_036              13036   //"[CHARGER_API] Set Adapter Current: 0x%x"
#define CHG_LOG_037              13037   //"[CHARGER_API] Set Full Current: 0x%x"
#define CHG_LOG_038              13038   //"[BATTERY_STATE COMPENSATION] %d, %d"
/*======================================================================================*/

/*=======================================PM=============================================*/
#define PM_START                 14000
#define PM_LOG_STATISTICS        14001      //"platform #%d: [%x] Wakeup Time: %d us Sleep Time: %d us (%d %d) Wakeup at: 0x%x Sleep at: 0x%x Tickcount: 0x%x -> 0x%x"
#define PM_LOG_STAGE_TIME        14002      //"platform stage time: Store: %d, Enter: %d, Exit: %d, Restore: %d"
#define PM_LOG_ENTER_DBG         14003      //"[PM] !**platform enter: addr: 0x%x, time cost: %d"
#define PM_LOG_EXIT_DBG          14004      //"platform exit: addr: 0x%x, time cost: %d
#define PM_LOG_CHECK_DBG2        14005      //"[PM] !**platform check: pof_delay: %d, pon_delay: %d, unit_wakeup_time_diff: %d"
#define PATCH_DATA_UART_CHECK_IN_DLPS_LOG_0 14008 //"[DU] GDMA Tx sem take. data uart tx done %d"
#define PATCH_DATA_UART_CHECK_IN_DLPS_LOG_1 14009 //"[DU] GDMA Tx sem give. data uart tx done %d"
#define PATCH_DATA_UART_CHECK_IN_DLPS_LOG_2 14010 //"data uart tx done %d, cannot enter dlps"

/*======================================================================================*/

/*=======================================USB============================================*/
#define USB_START               15000
#define USB_LOG_01              15001       //"disable_usb_power_seq: power down sequence to disable usb"
#define USB_LOG_02              15002       //"usb powwr on seq fail"
#define USB_LOG_03              15003       //"enable_usb_power_seq: power on sequence finished"
#define USB_LOG_04              15004       //"start test,  addon lbk reg = %x"
#define USB_LOG_05              15005       //" speed[%d], inner[%d]: usb slb fail"
#define USB_LOG_06              15006       //" speed[%d], inner[%d]: usb slb pass"
#define USB_LOG_07              15007       //" speed[%d], inner[%d]: usb slb not ready "
#define USB_LOG_08              15008       //" speed[%d], inner[%d]: usb slb timeout fail"
#define USB_LOG_09              15009       //"usb_self_loopback: Test Result: total(%d), success(%d)"
#define USB_LOG_10              15010       //"usb_poweron_bc12_by_project: before bc12 power: reg1550 = %x "
#define USB_LOG_11              15011       //"usb_poweron_bc12_by_project: after bc12 power: reg1550 = %x "
#define USB_LOG_12              15012       //"usb c-cut phy updated : %x, %x, %x", x, y, z
/*======================================================================================*/

/*==============================PLATFORM===============================*/
#define PLATFORM_START               16000
#define PLATFORM_APPCFG_PARSE        16001 //"parse sys_cfg in app cfg 0x%x error"
#define PLATFORM_SYSCFG_PARSE        16002 //">>>>>> CFG_Header:0x%x, Parsing Error"
#define PLATFORM_OS_TIMER_RESTAR     16003 //"restart timer address:%x, name:%s"
#define PLATFORM_OS_ALLOC_FAIL_LOG   16004 //!!!ALLOC FAIL!RAM type:%d,wanted sz:%d,remain sz:%d
#define PLATFORM_LOG_01              16005 //"free heap data %d, buffer %d, dsp share %d"
#define PLATFORM_LOG_02              16006 //"MinimumEverFreeHeapSize data %d, buffer %d, dsp share %d"
#define PLATFORM_LOG_03              16007 //"Interrupt Enable: 0x%x"
#define PLATFORM_LOG_04              16008 //"Interrupt Priority: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x"

#define PLATFORM_LOG_05              16009 //"CFG image Module 0x%x format checking fail!"
#define PLATFORM_LOG_06              16010 //"cfg_write_to_flash fail with 0x%x,0x%x"
#define PLATFORM_LOG_07              16011 //"alloc new cfg backup buffer fail!"
#define PLATFORM_LOG_08              16012 //"read cfg on flash fail!"
#define PLATFORM_LOG_09              16013 //"CFG image Module format error: skip len 0x%x,total cfg len 0x%x!"
#define PLATFORM_LOG_10              16014 //"cfg_search_and_update_item fail return %d"
#define PLATFORM_LOG_11              16015 //"cfg size will be out of range 0x%x + 0x%x > 0x%x!"
#define PLATFORM_LOG_12              16016 //"Ready to update xtal to config!"
#define PLATFORM_LOG_13              16017 //"update xtal return %d, operation step %d"
#define PLATFORM_LOG_14              16018 //"patch_hw_aes_operate_imp: error code: 0x%x"
#define PLATFORM_LOG_15              16019 //"cfg update ModuleID: %x, Offset: %x fail"
#define PLATFORM_LOG_16              16020 //"Ready to update cfg_items!"
#define PLATFORM_LOG_17              16021 //"update cfg_items status: %d"
/*======================================================================================*/

/*===================================DBG=================================*/
#define DBG_LOG_START        `  3000
#define DBG_LOG_001             3001 //"Remained free timer num: %d,timerMaxNumber: %d, uxTimerCreateCount: %d, uxTimerDeleteCount: %d"
#define DBG_LOG_002             3002 //"%s starts failed"
#define DBG_LOG_003             3003 //"parameter length too long"
#define DBG_LOG_004             3004 //"platform stack syscall error: 0x%x, opcode = 0x%x"
/*======================================================================================*/


/*=======================================UPPERSTACK=====================================*/
#define UPPERSTACK_START         5000
#define UPPER_LOG_INIT_01        5001    //"Upperstack Patch Init"

#define UPPER_LOG_HCI_01         5101    //"imp_patch_upperstack_hci_send_init_cmd: hci.le_isoc_buf_size %d, hci.le_isoc_xmit_window %d"
#define UPPER_LOG_HCI_02         5102    //"sm_check_bond_storage_pending return true"
#define UPPER_LOG_HCI_03         5103    //"hci_disconn_cmpl_evt: no desc found for handle 0x%04x, dup addr handle 0x%04x, disconn_handle_pending %d"
#define UPPER_LOG_HCI_04         5104    //"patch_hci_le_conn_cmpl_evt: handle 0x%04x, peer_addr %b connection already exist"
#define UPPER_LOG_HCI_05         5105    //"hci_cmd_status_evt: discard opcode 0x%04x before init"
#define UPPER_LOG_HCI_06         5106    //"hci_cmd_cmpl_evt: discard opcode 0x%04x before init"
#define UPPER_LOG_HCI_07         5107    //"hci_handle_encrypt_ind: bd_addr %s, status 0x%x, hci desc %p"
#define UPPER_LOG_HCI_08         5108    //"hci_handle_iso_ind: discard ISO data before init"
#define UPPER_LOG_HCI_09         5109    //"hci_handle_iso_ind: unknown conn handle 0x%x"
#define UPPER_LOG_HCI_10         5110    //"hci_handle_iso_ind: conn handle 0x%x, start pkt among continue pkts"
#define UPPER_LOG_HCI_11         5111    //"hci_handle_iso_ind: conn handle 0x%x, no start pkt found"
#define UPPER_LOG_HCI_12         5112    //"hci_handle_iso_ind: conn handle 0x%x, max expected %u, total received %u"
#define UPPER_LOG_HCI_13         5113    //"hci_le_acl_switch_set_acl_info_req: fail, p_data->le_acl_switch_version 0x%x, le_acl_switch_version 0x%x"

#define UPPER_LOG_L2C_01         5201    //"l2c_create_acl_conn: acl state %d"
#define UPPER_LOG_L2C_02         5202    //"l2cu_send_conn_req: bd_addr %s, psm %u, mtu %u, mode_bits 0x%02x, flush_tout 0x%04x"
#define UPPER_LOG_l2C_03         5203    //"l2c_handle_timeout: timer_id 0x%02x, timer_chann 0x%04x"
#define UPPER_LOG_L2C_04         5204    //"l2c_entry: unknown msg cmd 0x%04x"
#define UPPER_LOG_L2C_05         5205    //"l2c_handle_data_ind: unknown ACL handle 0x%04x"
#define UPPER_LOG_L2C_06         5206    //"l2c_handle_data_ind: start pkt amongst continue pkts"
#define UPPER_LOG_L2C_07         5207    //"l2c_handle_data_ind: discard short start pkt"
#define UPPER_LOG_L2C_08         5208    //"l2c_handle_data_ind: invalid cid 0x%04x"
#define UPPER_LOG_L2C_09         5209    //"l2c_handle_data_ind: pdu len %u exceeds br mtu"
#define UPPER_LOG_L2C_10         5210    //"l2c_handle_data_ind: expected %u, received %u, accumulated %u"
#define UPPER_LOG_L2C_11         5211    //"l2c_close_chann: free temp cid 0x%x "
#define UPPER_LOG_L2C_12         5212    //"l2c_close_chann: free cid 0x%x "

#define UPPER_LOG_SMP_01         5301    //"sm_le_update_own_addr_info: p_adv_set_rand_addr is NULL"
#define UPPER_LOG_SMP_02         5302    //"sm_le_update_own_addr_info: p_adv_set_rand_addr 2 is NULL"
#define UPPER_LOG_SMP_03         5303    //"sm_authen_cmpl: remote addr type 0x%02x, mode 0x%08x, state 0x%02x, cause 0x%04x, type %d"
#define UPPER_LOG_SMP_04         5304    //"sm_handle_security_req: addr %s, auth req 0x%02x, role %d, state 0x%08x"
#define UPPER_LOG_SMP_05         5305    //"sm_change_sec_status: state 0x%02x, cause 0x%04x, link sec state 0x%02x"
#define UPPER_LOG_SMP_06         5306    //"sm_handle_le_ltk_req_evt: p_link %p, handle 0x%04x, ediv 0x%04x, rand %b"
#define UPPER_LOG_SMP_07         5307    //"sm_handle_le_ltk_req_evt: as slave, in pairing process, get ltk from memory"
#define UPPER_LOG_SMP_08         5308    //"sm_handle_le_ltk_req_evt: as slave, get ltk from flash"
#define UPPER_LOG_SMP_09         5309    //"sm_cfg_dev_security: policy %d"
#define UPPER_LOG_SMP_10         5310    //"sm_handle_pairing_req: state 0x%08x, io_cap %d, oob_flag %d, remote_auth_req %d, max_key_size %d, init_key_dist 0x%02x, rsp_key_dist 0x%02x"
#define UPPER_LOG_SMP_11         5311    //"sm_handle_pairing_req: sc %d, local_io %d, remote_io %d, ssp_method %d"
#define UPPER_LOG_SMP_12         5312    //"sm_handle_pairing_req: init key dist %d, rsp key dist %d, local key dist %d"
#define UPPER_LOG_SMP_13         5313    //"sm_handle_pairing_req: send PAIRING_RESPONSE iocap %d, oob %d, auth req 0x%02x, max key size %d, key dist 0x%02x|0x%02x"
#define UPPER_LOG_SMP_14         5314    //"sm_handle_smp_message: can not find link by handle 0x%04x"
#define UPPER_LOG_SMP_15         5315    //"sm_handle_smp_message: handle 0x%04x, cmd 0x%02x"
#define UPPER_LOG_SMP_16         5316    //"sm_handle_smp_message: sm_alloc_smp_data failed"
#define UPPER_LOG_SMP_17         5317    //"sm_handle_smp_message: unhandle command"
#define UPPER_LOG_SMP_18         5318    //"sm_handle_smp_message: invalid cmd , role %d, smp state 0x%08x"
#define UPPER_LOG_SMP_19         5319    //"sm_link_key_to_ltk: failed to alloc smp data for handle 0x%04x"
#define UPPER_LOG_SMP_20         5320    //"sm_link_key_to_ltk: addr %s, key type 0x%02x, ltk %b, link state 0x%2x, link mode 0x%08x"
#define UPPER_LOG_SMP_21         5321    //"sm_handle_link_key_notif_evt: bd_addr %s, link %p, key type 0x%02x, key %b"
#define UPPER_LOG_SMP_22         5322    //"sm_handle_io_cap_req_evt: addr %s, incoming pair %u, br paiable mode %u"
#define UPPER_LOG_SMP_23         5323    //"sm_pair_queue_out: cmd 0x%04x, addr %s"
#define UPPER_LOG_SMP_24         5324    //"sm_ltk_to_link_key: remote addr %s, key type 0x%02x, link_key %b"
#define UPPER_LOG_SMP_25         5325    //"sm_handle_pairing_rsp: state 0x%08x, sc_policy %d, io_cap %d, oob_flag %d, auth_req %d, max_key_size %d, init_key_dist 0x%02x, rsp_key_dist 0x%02x"
#define UPPER_LOG_SMP_26         5326    //"sm_handle_pairing_rsp: bad state"
#define UPPER_LOG_SMP_27         5327    //"sm_handle_le_rand_cmd_cmpl: as slave, check and send confirm"
#define UPPER_LOG_SMP_28         5328    //"sm_handle_dhkey_check: local pubublic addr %s, local random addr %s, local addr type %d, remote addr type %d"
#define UPPER_LOG_SMP_29         5329    //"sm_handle_dhkey_check: caculated e %b not match remote e %b, n_remote %b, n_local %b, r_local %b"
#define UPPER_LOG_SMP_30         5330    //"sm_le_key_exchange: smp state 0x%08x, dist 0x%02x/0x%02x, store 0x%02x/0x%02x"
#define UPPER_LOG_SMP_31         5331    //"sm_handle_encrypt_ind: bd_addr %s, encrypted %d, status 0x%04x, mode 0x%08x, state 0x%02x"
#define UPPER_LOG_SMP_32         5332    //"sm_br_send_pairing_req: failed to alloc smp data for handle 0x%x"
#define UPPER_LOG_SMP_33         5333    //"sm_br_trigger_link_key_to_ltk_proc: addr %s, link state 0x%x, link mode 0x%08x"
#define UPPER_LOG_SMP_34         5334    //"sm_handle_le_authen_req: p_link->state %d"
#define UPPER_LOG_SMP_35         5335    //"sm_handle_le_authen_req: failed to alloc smp data"
#define UPPER_LOG_SMP_36         5336    //"sm_handle_message: unknown message 0x%04x"
#define UPPER_LOG_SMP_37         5337    //"sm_handle_dev_data_get_cfm: bd_addr %s, key type 0x%02x, status 0x%04x, count %d"
#define UPPER_LOG_SMP_38         5338    //"sm_handle_dev_data_get_cfm p_link = NULL"
#define UPPER_LOG_SMP_39         5339    //"sm_handle_dev_data_get_cfm: mode 0x%08x, state %d, key size %d, min key size %d, key type %d"
#define UPPER_LOG_SMP_40         5340    //"sm_handle_dev_data_get_cfm: fail to alloc smp data for handle 0x%04x"
#define UPPER_LOG_SMP_41         5341    //"sm_handle_dev_data_get_cfm: BTIF_KEY_LE_REMOTE_LTK, key type %d"
#define UPPER_LOG_SMP_42         5342    //"sm_get_smp_key_dist: authen 0x%04x"
#define UPPER_LOG_SMP_43         5343    //"sm_handle_smp_message: can not find link by handle, or link deactivated 0x%04x"
#define UPPER_LOG_SMP_44         5344    //"sm_handle_timeout: timer id 0x%02x, timer chann 0x%04x, link %p"
#define UPPER_LOG_SMP_45         5345    //"sm_alloc_link_common: could not allocate link for addr %s"
#define UPPER_LOG_SMP_46         5346    //"sm_handle_le_disconn_cmpl_common: p_link %p, state %d"
#define UPPER_LOG_SMP_47         5347    //"sm_crypt_handle_link_check_null: can not find link or p_smp_data for index 0x%04x, link %p"

#define UPPER_LOG_SDP_01         5401    //"sdp_handle_l2c_data: pdu len %u, len %u, left len %u"
#define UPPER_LOG_SDP_02         5402    //"sdp_handle_l2c_data: unhandled code 0x%02x"
#define UPPER_LOG_SDP_03         5403    //"sdp_handle_l2c_data: pdu len %u, len %u, left len %u"
#define UPPER_LOG_SDP_04         5404    //"sdp_handle_l2c_data: unhandled code 0x%02x"
#define UPPER_LOG_SDP_05         5405    //"sdp_attr_req: cid 0x%04x, handle 0x%04x, max cnt %d, p_chann %p"
#define UPPER_LOG_SDP_06         5406    //"sdp_handle_srv_search_rsp: wrong id 0x%04x, chann id 0x%04x, next code 0x%02x"
#define UPPER_LOG_SDP_07         5407    //"sdp_handle_srv_search_rsp: syntax error in response"
#define UPPER_LOG_SDP_08         5408    //"sdp_handle_srv_attr_rsp: syntax error"
#define UPPER_LOG_SDP_09         5409    //"sdp_handle_srv_attr_rsp: rsp byte cnt %d more than allow %d""
#define UPPER_LOG_SDP_10         5410    //"sdp_handle_srv_search_attr_rsp: syntax error"
#define UPPER_LOG_SDP_11         5411    //"sdp_handle_srv_search_attr_rsp: rsp byte cnt %d more than allow %d"
#define UPPER_LOG_SDP_12         5412    //"sdp_search_req: cid 0x%04x, max handle %d, p_chann %p"

#define UPPER_LOG_BTIF_01        5501    //"btif_handle_vendor_le_set_host_feature_req: vendor_le_host_feature 0x%x, bit_number %d, bit_value 0x%x"
#define UPPER_LOG_BTIF_02        5502    //"btif_handle_le_connect_cfm: can not find link by addr %s"
#define UPPER_LOG_BTIF_03        5503    //"btif_handle_le_connect_cfm: link_state 0x%02x"
#define UPPER_LOG_BTIF_04        5504    //"btif_handle_le_connect_cfm: accept connection on bd_addr %s"
#define UPPER_LOG_BTIF_05        5505    //"btif_handle_le_connect_cfm: reject connection on bd_addr %s"
#define UPPER_LOG_BTIF_06        5506    //"btif_handle_le_connect_cfm: invalid link state 0x%02x"
#define UPPER_LOG_BTIF_07        5507    //"btif_radio_mode_set_req: limitted discovery mode did not timeout"
#define UPPER_LOG_BTIF_08        5508    //"btif_sdp_get_next_srv: link state 0x%02x, total num %d, hdl idx %d"
#define UPPER_LOG_BTIF_09        5509    //"btif_sdp_search_rsp: bd %s, p_link %p, p_rsp %p"
#define UPPER_LOG_BTIF_10        5510    //"btif_sdp_conn_cmpl: bd %s, cid 0x%04x, status 0x%04x, p_link %p"
#define UPPER_LOG_BTIF_11        5511    //"btif_find_link_by_handle_common: failed, handle 0x%04x, link type 0x%02x"
#define UPPER_LOG_BTIF_12        5512    //"btif_alloc_link_by_conn_handle: alloc fail"
#define UPPER_LOG_BTIF_13        5513    //"btif_gatt_conn_cmpl: bd %s, handle 0x%04x, p_link %p, link_state %d"
#define UPPER_LOG_BTIF_14        5514    //"btif_gatt_conn_cmpl: invalid link_state 0x%02x"

#define UPPER_LOG_BUILTIN_SVC_01 5601    //"gatt_add_builtin_services: Add gatt service failed"
#define UPPER_LOG_BUILTIN_SVC_02 5602    //"gatt_add_builtin_services: Add gap service failed"

#define UPPER_LOG_INIT_MTU       5603    //"gatt_ll_connected_patch: p_gatt_acl->outgoing %d, master_init_mtu %d, slave_init_mtu %d"

#define UPPER_LOG_GATT_01        5701    //"att_gatt_caching_handle_rx_data: deferred msg, conn handle 0x%x, opcode 0x%x"
#define UPPER_LOG_GATT_02        5702    //"att_gatt_caching_handle_rx_data: ignore command, conn handle 0x%x, opcode 0x%x, service_change_state 0x%x"
#define UPPER_LOG_GATT_03        5703    //"att_gatt_caching_handle_rx_data: unknown opcode 0x%x, conn handle 0x%x, service_change_state 0x%x"
#define UPPER_LOG_GATT_04        5704    //"att_handle_rx_data: opcode 0x%02x"
#define UPPER_LOG_GATT_05        5705    //"att_handle_rx_data: opcode 0x%02x is undefined, command flag is 0x%x"
#define UPPER_LOG_GATT_06        5706    //"gatt_handle_data_ind: deferred msg att opcode: 0x%02x"
#define UPPER_LOG_GATT_07        5707    //"gatt_handle_data_ind: discard LE_DATA"
#define UPPER_LOG_GATT_08        5708    //"gatt_handle_data_ind: pChann is NULL"
#define UPPER_LOG_GATT_09        5709    //"gatt_handle_le_conn_cmpl_evt: p_gatt_acl is NULL and status is 0"
#define UPPER_LOG_GATT_10        5710    //"gatt_handle_le_conn_cmpl_evt: p_chann is NULL and status is 0"
#define UPPER_LOG_GATT_11        5711    //"gatt_alloc_acl_common: already exist"
#define UPPER_LOG_GATT_12        5712    //"gatt_alloc_acl_common: allocate fail"
#define UPPER_LOG_GATT_13        5713    //"gatt_chann_get_cid_by_conn_handle: handle 0x%04x, p_cid_num %d"

/*======================================================================================*/

/*===========================================MP=========================================*/
#define PLATFORM_MP_START       7000
#define PLATFORM_MP_LOG_001     7001 //"[ADC]: mode = 0x%x, index = %d, average_times = %d"
#define PLATFORM_MP_LOG_002     7002 //"[ADC]: ADC Register Request Fail."
#define PLATFORM_MP_LOG_003     7003 //"[ADC]: sar_adc_result = %d"
#define PLATFORM_MP_LOG_004     7004 //"[CHARGER]: test_type = 0x%x, full_current = %d"
#define PLATFORM_MP_LOG_005     7005 //"[CHARGER]: Charger Test Timer Start Fail."
#define PLATFORM_MP_LOG_006     7006 //"[GPIO]: PAD_num = %d is not support"
#define PLATFORM_MP_LOG_007     7007 //"[GPIO]: sub_opcode = 0x%x, PAD_num = %d, direct_level = %d"
#define PLATFORM_MP_LOG_008     7008 //"[XTAL_AUTO_K] xtal_value = 0x%x"
#define PLATFORM_MP_LOG_009     7009 //"[XTAL_AUTO_K] freq_drift = %d Hz"
#define PLATFORM_MP_LOG_010     7010 //"[XTAL_AUTO_K] index = %d, dft_out = %d"
#define PLATFORM_MP_LOG_011     7011 //"[XTAL_AUTO_K] peak_index = %d"
#define PLATFORM_MP_LOG_012     7012 //"[PHY] pouter: packet_type not support"
#define PLATFORM_MP_LOG_013     7013 //"[PHY] DFT: IF_val = %d, dft_out = %d"
#define PLATFORM_MP_LOG_014     7014 //"[PHY] thermal_log = %d degree"
/*======================================================================================*/

#define LOG_MAX_INDEX   0xFFFFFF

#define PATCH_PRINT_INFO9(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)   \
    DBG_INDEX(TYPE_BB2, SUBTYPE_INDEX, MODULE_PATCH, LEVEL_INFO, fmt, 9, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
#define PATCH_PRINT_INFO10(fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)   \
    DBG_INDEX(TYPE_BB2, SUBTYPE_INDEX, MODULE_PATCH, LEVEL_INFO, fmt, 10, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)

#endif//_PATCH_LOG_H__
