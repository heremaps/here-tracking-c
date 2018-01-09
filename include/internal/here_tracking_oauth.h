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

#ifndef HERE_TRACKING_OAUTH_H
#define HERE_TRACKING_OAUTH_H

#include <stdint.h>

#include "here_tracking_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HERE_TRACKING_OAUTH_MIN_OUT_SIZE 384

/**
 * Create OAuth1 authorization header. This method is not thread safe.
 *
 * @param[in] device_id Device ID. Used as OAuth consumer key.
 *                     0-termination not required, length must be HERE_TRACKING_DEVICE_ID_SIZE.
 * @param[in] device_secret Device Secret. Used as OAuth signature key.
 *                         0-termination not required, length must be
 *                         HERE_TRACKING_DEVICE_SECRET_SIZE.
 * @param[in] base_url The base URL of the HERE Tracking service. 0-termination isnot required.
 * @param[in] srv_time_diff Time difference between platform clock and server clock.
 *                          Used to adjust timestamp in created header.
 * @param[out] out Buffer to return the created authorization header in.
 *                 Result will not be 0-terminated.
 * @param[in,out] out_size Size of the out buffer in bytes.
 *                         Must be at least HERE_TRACKING_OAUTH_MIN_OUT_SIZE.
 *                         Will be set to the size of created authorization header in return.
 * @return Code defining operation success
 */
here_tracking_error here_tracking_oauth_create_header(const char* device_id,
                                                      const char* device_secret,
                                                      const char* base_url,
                                                      int32_t srv_time_diff,
                                                      char* out,
                                                      uint32_t* out_size);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_OAUTH_H */
