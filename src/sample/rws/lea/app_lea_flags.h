
#ifndef _APP_LEA_FLAGS_H_
#define _APP_LEA_FLAGS_H_

#if F_APP_LEA_SUPPORT

#define GATTC_TBL_STORAGE_SUPPORT       1
#define BT_GATT_CLIENT_SUPPORT          1

#define F_APP_LE_AUDIO_CLI_TEST         1

#define F_APP_VCS_SUPPORT               1
#define F_APP_MICS_SUPPORT              0
#define F_APP_CSIS_SUPPORT              0
#define F_APP_EATT_SUPPORT              0
#define F_APP_PBP_SUPPORT               1

#define F_APP_LC3_PLUS_SUPPORT          0
#define F_APP_FRAUNHOFER_SUPPORT        0

// CT and UMR must support ASCS and PACS service
#define F_APP_TMAP_CT_SUPPORT           1
#define F_APP_TMAP_UMR_SUPPORT          1
// If the Broadcast Sink role is supported, the Scan Delegator role shall be supported
#define F_APP_TMAP_BMR_SUPPORT          1
#define F_APP_TMAS_SUPPORT              1

#if F_APP_TMAP_CT_SUPPORT
#undef F_APP_CCP_SUPPORT
#define F_APP_CCP_SUPPORT               1
#endif

#if F_APP_TMAP_UMR_SUPPORT
#undef F_APP_MCP_SUPPORT
#define F_APP_MCP_SUPPORT               1
#endif

#if F_APP_TMAP_BMR_SUPPORT
#undef F_APP_LEA_REALCAST_SUPPORT
#define F_APP_LEA_REALCAST_SUPPORT      0
#undef F_APP_MCP_SUPPORT
#define F_APP_MCP_SUPPORT               1
#endif

#endif

#endif

