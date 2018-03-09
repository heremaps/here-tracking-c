/**************************************************************************************************
 * Copyright (C) 2017-2018 HERE Europe B.V.                                                       *
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
 * @file here_tracking_error.h
 * @brief HERE Tracking client error codes
 */

#ifndef HERE_TRACKING_ERROR_H
#define HERE_TRACKING_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes
 */
typedef enum
{
    /** @brief OK */
    HERE_TRACKING_OK                      =   0,

    /** @brief Unspecified error */
    HERE_TRACKING_ERROR                   =  -1,

    /** @brief Input is invalid */
    HERE_TRACKING_ERROR_INVALID_INPUT     =  -2,

    /** @brief Provided data buffer is too small */
    HERE_TRACKING_ERROR_BUFFER_TOO_SMALL  =  -3,

    /** @brief More data is required to proceed */
    HERE_TRACKING_ERROR_NEED_MORE_DATA    =  -4,

    /** @brief Operation was interrupted by the client */
    HERE_TRACKING_ERROR_CLIENT_INTERRUPT  =  -5,

    /** @brief Client and server time are not in sync */
    HERE_TRACKING_ERROR_TIME_MISMATCH     =  -6,

    /** @brief Request data was not accepted by the server */
    HERE_TRACKING_ERROR_BAD_REQUEST       =  -7,

    /** @brief Request was unauthorized because of invalid authentication details */
    HERE_TRACKING_ERROR_UNAUTHORIZED      =  -8,

    /** @brief The device doesn't have the necessary priviledges to access HERE Tracking service */
    HERE_TRACKING_ERROR_FORBIDDEN         =  -9,

    /** @brief The device can't receive access token because it hasn't been claimed yet. */
    HERE_TRACKING_ERROR_DEVICE_UNCLAIMED  = -10,

    /** @brief Requested data/resource was not found */
    HERE_TRACKING_ERROR_NOT_FOUND         = -11
} here_tracking_error;

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_ERROR_H */
