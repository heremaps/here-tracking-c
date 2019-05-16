/**************************************************************************************************
 * Copyright (C) 2017-2019 HERE Europe B.V.                                                       *
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

#ifndef MOCK_HERE_TRACKING_HTTP_H
#define MOCK_HERE_TRACKING_HTTP_H

#include <fff.h>

#include "here_tracking_http.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_http_auth, here_tracking_client*);

DECLARE_FAKE_VALUE_FUNC4(here_tracking_error,
                         here_tracking_http_send,
                         here_tracking_client*,
                         char*,
                         uint32_t,
                         uint32_t);

DECLARE_FAKE_VALUE_FUNC6(here_tracking_error,
                         here_tracking_http_send_stream,
                         here_tracking_client*,
                         here_tracking_send_cb,
                         here_tracking_recv_cb,
                         here_tracking_req_type,
                         here_tracking_resp_type,
                         void*);

#define MOCK_HERE_TRACKING_HTTP_FAKE_LIST(FAKE) \
    FAKE(here_tracking_http_auth)  \
    FAKE(here_tracking_http_send)  \
    FAKE(here_tracking_http_send_stream) \

void mock_here_tracking_http_auth_set_result_token(const char* token);

here_tracking_error mock_here_tracking_http_auth_custom(here_tracking_client* client);

void mock_here_tracking_http_send_set_result_data(const char* data, uint32_t data_size);

void mock_here_tracking_http_set_recv_cb_status(here_tracking_error status);

here_tracking_error mock_here_tracking_http_send_custom(here_tracking_client* client,
                                                        char* data,
                                                        uint32_t send_size,
                                                        uint32_t recv_size);

here_tracking_error mock_here_tracking_http_send_stream_custom(here_tracking_client* client,
                                                               here_tracking_send_cb send_cb,
                                                               here_tracking_recv_cb recv_cb,
                                                               here_tracking_req_type req_type,
                                                               here_tracking_resp_type resp_type,
                                                               void* user_data);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_HERE_TRACKING_HTTP_H */
