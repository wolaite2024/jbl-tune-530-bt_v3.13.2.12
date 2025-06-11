#ifndef _APP_GFPS_PSM_H_
#define _APP_GFPS_PSM_H_

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GFPS_LE_MSG_USE_ECFC 0
/**
 * @brief Initialize parameters of GAP ECFC Module.
 * and Register a callback function to handle L2CAP ECFC related messages for a specific gfps PSM.
 *
 */
void app_gfps_psm_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_GFPS_PSM_H_ */
