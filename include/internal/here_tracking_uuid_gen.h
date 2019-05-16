/**************************************************************************************************
 * Copyright (C) 2018 HERE Europe B.V.                                                            *
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

#ifndef HERE_TRACKING_UUID_GEN_H
#define HERE_TRACKING_UUID_GEN_H

#include <stddef.h>

#include "here_tracking_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of UUIDv4 string in bytes, including the 0-terminator.
 */
#define HERE_TRACKING_UUID_SIZE 37

/**
 * @brief Generates a new UUIDv4 string.
 *
 * @param[out] buf Buffer for the generated UUID.
 * @param[in] buf_size Size of the provided buffer in bytes.
 *                     Must be at least HERE_TRACKING_UUID_SIZE.
 * @return ::HERE_TRACKING_OK UUID created successfully.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT Provided buffer is NULL.
 * @return ::HERE_TRACKING_ERROR_BUFFER_TOO_SMALL Provided buffer is too small.
 * @return ::HERE_TRACKING_ERROR Unknown error.
 */
here_tracking_error here_tracking_uuid_gen_new(char* buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_UUID_GEN_H */
