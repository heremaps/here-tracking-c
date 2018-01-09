/**************************************************************************************************
 * Copyright (C) 2017 HERE Europe B.V.                                                            *
 * All rights reserved.                                                                           *
 *                                                                                                *
 * MIT License                                                                                    *
 * Permission is hereby granted, free of charge, to any person obtaining a copy                   *
 * of this software and associated documentation files (the "Software"), to deal                  *
 * in the Software without restriction, including without limitation the rights                   *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                      *
 * copies of the Software, and to permit persons to whom the Software is                          *
 * furnished to do so, subject to the following conditions:                                       *
 *                                                                                                *
 * The above copyright notice and this permission notice shall be included in all                 *
 * copies or substantial portions of the Software.                                                *
 *                                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                     *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                       *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE                    *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                         *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,                  *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE                  *
 * SOFTWARE.                                                                                      *
 **************************************************************************************************/

/**
 * @file here_tracking_base64.h
 *
 * @brief Interface for Base64 encoding.
 *
 * The HERE Tracking library defines several interfaces that must be implemented to use the library.
 * Base64 encoding is required for authenticating with HERE Tracking server and you must ensure that
 * the target platform provides an implementation for the defined interface methods.
 *
 * @defgroup base64_if Base64 encoder interface
 * @{
 *
 * @brief Interface for Base64 encoding.
 *
 * The HERE Tracking library defines several interfaces that must be implemented to use the library.
 * Base64 encoding is required for authenticating with HERE Tracking server and you must ensure that
 * the target platform provides an implementation for the defined interface methods.
 */

#ifndef HERE_TRACKING_BASE64_H
#define HERE_TRACKING_BASE64_H

#include <stdint.h>

#include "here_tracking_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Encodes the data to Base64 format.
 *
 * @param[in] in The data to encode. A null-terminator is not required.
 * @param[in] in_size The size of input data in bytes.
 * @param[out] out The buffer to write the encoded data to. A null-terminator is not required.
 * @param[in,out] out_size On input this parameter specifies the size of the output buffer in bytes.
 *                         If encoding is successful, the parameter is set to the exact number of
 *                         bytes written to the buffer, not including the null-terminator.
 *                         In case encoding fails, the value of the parameter is unspecified.
 * @return ::HERE_TRACKING_OK The data was successfully encoded.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR_BUFFER_TOO_SMALL The encoded data doesn't fit the data buffer
 *         provided in @p out. Content of the output buffer is unspecified after this error.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_base64_enc(const char* in,
                                             uint32_t in_size,
                                             char* out,
                                             uint32_t* out_size);

/**
 * @brief Calculates number of bytes required for Base64 encoding.
 *
 * @param[in] size The number of bytes to encode.
 * @return The number of bytes required for Base64 encoding.
 */
#if (defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L) || defined __cplusplus
static inline uint32_t here_tracking_base64_enc_size(uint32_t size)
#else
static uint32_t here_tracking_base64_enc_size(uint32_t size)
#endif
{
    /* Adds one byte because some base64 implementations require space for a null-terminator. */
    return (((4 * size / 3) + 3) & ~3) + 1;
}

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_BASE64_H */

/** @} */
