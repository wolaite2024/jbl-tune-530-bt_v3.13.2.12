/**
 * Copyright (c) 2018, Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef _BUILD_CONFIG_H_
#define _BUILD_CONFIG_H_

//*** <<< Use Configuration Wizard in Context Menu >>> ***

// <o>  Build Bank
// <i>  Indicates which bank the image is built for.
// <0=> Bank 0
// <1=> Bank 1
#define BUILD_BANK                  0

// <o>  Secure Verison
// <i>  Secure Verison of Current App Image, range is 0 to 255.
// <0-255>
#define SECURE_VERSION              0

// <o>  Running Mode
// <i>  Indicates the code is running in RAM or XIP.
// <0=> XIP
// <1=> Run in RAM
#define FEATURE_RAM_CODE            0

// <o>  RAM Code Encryption
// <i>  This option is used to enable image encryption if code is running in RAM.
// <0=> Disable
// <1=> Enable
#define ENCRYPT_RAM_CODE            0

// <o>  Encryption Key Select
// <i>  Select key type for RAM code encryption
// <0=> ENC_KEY_SCEK
// <1=> ENC_KEY_SCEK_WITH_RTKCONST
// <2=> ENC_KEY_OCEK
// <3=> ENC_KEY_OCEK_WITH_OEMCONST
// <4=> ENC_KEY_ON_FLASH
// <5=> Others
#define ENC_KEY_SELECT              2

/**
* @brief: Use Cache RAM to run RAM Code
*/
#define FEATURE_CACHE_RAM_CODE      0

//*** <<< end of configuration section >>> ***

#endif /* _BUILD_CONFIG_H_ */
