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
 * @file here_tracking_tls.h
 *
 * @brief Interface definition for the TLS interface.
 *
 * @defgroup tls_if TLS interface
 * @{
 *
 * @brief Interface definition for the TLS interface.
 *
 * You must ensure that the target platform provides an implementation for the TLS interface
 * methods.
 */

#ifndef HERE_TRACKING_TLS_H
#define HERE_TRACKING_TLS_H

#include <stdint.h>

#include "here_tracking_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* here_tracking_tls; /**< @brief TLS handle */

/**
 * @brief Initializes the TLS implementation.
 *
 * @param[in] tls Pointer to the uninitialized TLS handle.
 * @return ::HERE_TRACKING_OK The TLS implementation was successfully initialized.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 */
here_tracking_error here_tracking_tls_init(here_tracking_tls* tls);

/**
 * @brief Releases resources that were allocated the TLS implementation.
 *
 * @param[in] tls Pointer to the initialized TLS handle.
 * @return ::HERE_TRACKING_OK The system resources were successfully released. 
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 */
here_tracking_error here_tracking_tls_free(here_tracking_tls* tls);

/**
 * @brief Establishes a TLS connection.
 *
 * @param[in] tls The initialized TLS handle.
 * @param[in] host The address of the host to connect to. You must terminate the string with `\0`.
 * @param[in] port The port number to connect to.
 * @return ::HERE_TRACKING_OK A TLS connection was successfully established.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_tls_connect(here_tracking_tls tls,
                                              const char* host,
                                              uint16_t port);

/**
 * @brief Closes the TLS connection.
 *
 * @param[in] tls The initialized TLS handle.
 * @return ::HERE_TRACKING_OK The TLS connection was successfully closed.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_tls_close(here_tracking_tls tls);

/**
 * @brief Reads data from a connected TLS socket.
 *
 * @param[in] tls The initialized TLS handle.
 * @param[out] data The buffer to read the incoming data to.
 * @param[in,out] data_size On input this parameter specifies the maximum number of bytes to read.
 *                          On output it is set to the actual number of bytes read.
 * @return ::HERE_TRACKING_OK The data was successfully received from the TLS socket.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_tls_read(here_tracking_tls tls,
                                           char* data,
                                           uint32_t* data_size);

/**
 * @brief Writes data to a connected TLS socket.
 *
 * @param[in] tls The initialized TLS handle.
 * @param[in] data The buffer to write the outgoing data from.
 * @param[in,out] data_size On input this parameter specifies maximum number of bytes to write.
 *                          On output it is set to the actual number of bytes written.
 * @return ::HERE_TRACKING_OK The data was successfully written to the TLS socket.
 * @return ::HERE_TRACKING_ERROR_INVALID_INPUT One or more input parameters were invalid.
 * @return ::HERE_TRACKING_ERROR An unknown error occurred.
 */
here_tracking_error here_tracking_tls_write(here_tracking_tls tls,
                                            const char* data,
                                            uint32_t* data_size);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_TLS_H */

/** @} */
