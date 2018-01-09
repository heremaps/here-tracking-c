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
 * @file here_tracking_time.h
 *
 * @brief Interface definition for the time interface.
 *
 * @defgroup time_if Time interface
 * @{
 *
 * @brief Interface definition for the time interface.
 *
 * You must ensure that the target platform provides an implementation for the time interface
 * methods.
 */

#ifndef HERE_TRACKING_TIME_H
#define HERE_TRACKING_TIME_H

#include <stdint.h>

#include "here_tracking_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the current Unix timestamp in seconds.
 *
 * @param[out] ts The Unix timestamp in seconds.
 * @return ::HERE_TRACKING_OK The Unix timestamp was successfully received.
 * @return ::HERE_TRACKING_ERROR Could not get the Unix timestamp.
 */
here_tracking_error here_tracking_get_unixtime(uint32_t* ts);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_TIME_H */

/** @} */
