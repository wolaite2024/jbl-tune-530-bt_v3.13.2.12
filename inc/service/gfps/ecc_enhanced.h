/*
 * Copyright (c) 2013, Kenneth MacKay
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _ECC_ENHANCED_
#define _ECC_ENHANCED_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ECC_CAUSE_SUCCESS           = 0x00,//ecdh_shared_secret_enhanced() run completed
    ECC_CAUSE_PENDING           = 0x01,//ecdh_shared_secret_enhanced() is running
    ECC_CAUSE_FAIL              = 0x02,//ecdh_shared_secret_enhanced() run fail
} T_ECC_CAUSE;

/**
 *@brief Compute a shared secret use ecdh.
 * Note: Because ECDH is time consuming, the function is piecewise executed.
 * After one phase is completed, an MSG(@ref IO_MSG_TYPE_ECC) will be sent to the app task.
 * After app task receive the msg(@ref IO_MSG_TYPE_ECC), it will execute the next phase.
 *@Input:
 *  public_key  - The public key of the remote party.
 *  private_Key - Your private key.
 *
 *@Output:
 *  secret - Will be filled in with the shared secret value.
 *
 * @Return ECC_CAUSE_SUCCESS if the shared secret was generated successfully, Both input and output parameters are with the
 * LSB first.
 */
T_ECC_CAUSE ecdh_shared_secret_enhanced(const uint8_t public_key[64],
                                        const uint8_t private_key[32],
                                        uint8_t secret[32]);

T_ECC_CAUSE ecc_sub_proc(void);

void ecdh_enter_test_mode(void);
void ecdh_exit_test_mode(void);
#ifdef __cplusplus
}
#endif

#endif /* _ECC_ENHANCED_ */
