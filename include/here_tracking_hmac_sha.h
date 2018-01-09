/**************************************************************************************************
* Copyright (C) 2017 HERE Europe B.V.                                                             *
* All rights reserved.                                                                            *
*                                                                                                 *
* MIT License                                                                                     *
* Permission is hereby granted, free of charge, to any person obtaining a copy                    *
* of this software and associated documentation files (the "Software"), to deal                   *
* in the Software without restriction, including without limitation the rights                    *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                       *
* copies of the Software, and to permit persons to whom the Software is                           *
* furnished to do so, subject to the following conditions:                                        *
*                                                                                                 *
* The above copyright notice and this permission notice shall be included in all                  *
* copies or substantial portions of the Software.                                                 *
*                                                                                                 *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                      *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                        *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE                     *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                          *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,                   *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE                   *
* SOFTWARE.                                                                                       *
**************************************************************************************************/


/**
 * @file here_tracking_hmac_sha.h
 *
 * @brief Interface definition for the HMAC-SHA interface.
 *
 * The HERE Tracking library defines several interfaces that must be implemented to use the library.
 * HMAC-SHA calculation is required for authenticating with HERE Tracking server and you must ensure
 * that the target platform provides an implementation for the defined interface methods.
 *
 * @defgroup hmac_sha_if HMAC-SHA interface
 * @{
 *
 * @brief Interface definition for the HMAC-SHA interface.
 *
 * The HERE Tracking library defines several interfaces that must be implemented to use the library.
 * HMAC-SHA calculation is required for authenticating with HERE Tracking server and you must ensure
 * that the target platform provides an implementation for the defined interface methods.
 */

#ifndef HERE_TRACKING_HMAC_SHA_H
#define HERE_TRACKING_HMAC_SHA_H

#include <stdint.h>

#include "here_tracking_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HERE_TRACKING_HMAC_SHA256_OUT_SIZE 32 /**< @brief Size of HMAC-SHA256 code in bytes */

/**
 * @brief Computes the HMAC of the data using the SHA256 hash function.
 *
 * @param[in] msg The message data. A null-terminator is not required.
 * @param[in] msg_size The message size in bytes.
 * @param[in] secret The message secret. A null-terminator is not required.
 * @param[in] secret_size The secret size in bytes.
 * @param[out] out A buffer to write the calculated HMAC to. A null-terminator is not required.
 * @param[in,out] out_size On input this parameter specifies the size of the output buffer in bytes.
 *                         If computation is successful, the parameter is set to the exact number of
 *                         bytes written to the buffer, not including the null-terminator.
 *                         In case computation fails, the value of the parameter is unspecified.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR_BUFFER_TOO_SMALL The HMAC doesn't fit the data buffer provided
 *         in @p out. Content of the output buffer is unspecified after this error.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_hmac_sha256(const char* msg,
                                              uint32_t msg_size,
                                              const char* secret,
                                              uint32_t secret_size,
                                              char* out,
                                              uint32_t* out_size);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_HMAC_SHA_H */

/** @} */
