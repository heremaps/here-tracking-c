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

#ifndef HERE_TRACKING_HTTP_H
#define HERE_TRACKING_HTTP_H

#include "here_tracking.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HTTP header
 */
typedef struct
{
    /** @brief Header name */
    char* name;

    /** @brief Header value */
    char* value;
} here_tracking_http_header;

/**
 * @brief HTTP request
 */
typedef struct
{
    /** @brief Host address/name */
    char* host;

    /** @brief Request path */
    char* path;

    /** @brief Port number */
    uint16_t port;

    /** HTTP request headers array */
    here_tracking_http_header* headers;

    /** Number of elements in @link here_tracking_http_request::headers @endlink */
    uint8_t header_count;
} here_tracking_http_request;

here_tracking_error here_tracking_http_auth(here_tracking_client* client);

here_tracking_error here_tracking_http_send(here_tracking_client* client,
                                            char* data,
                                            uint32_t send_size,
                                            uint32_t recv_size);

here_tracking_error here_tracking_http_send_stream(here_tracking_client* client,
                                                   here_tracking_send_cb send_cb,
                                                   here_tracking_recv_cb recv_cb,
                                                   here_tracking_req_type req_type,
                                                   here_tracking_resp_type resp_type,
                                                   void* user_data);

/**
 * @brief Make HTTP GET request
 *
 * @param[in] client Pointer to the initialized client structure.
 * @param[in] request Request to make.
 * @param[in] recv_cb Callback method that will be called when response is received.
 * @param[in] user_data User data to pass back as an argument in recv callback.
 */
here_tracking_error here_tracking_http_get(here_tracking_client* client,
                                           const here_tracking_http_request* request,
                                           here_tracking_recv_cb recv_cb,
                                           void* user_data);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_HTTP_H */
