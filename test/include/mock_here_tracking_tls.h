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


#ifndef MOCK_HERE_TRACKING_TLS_H
#define MOCK_HERE_TRACKING_TLS_H

#include <fff.h>

#include "here_tracking_tls.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_tls_init, here_tracking_tls*);

DECLARE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_tls_free, here_tracking_tls*);

DECLARE_FAKE_VALUE_FUNC3(here_tracking_error,
                         here_tracking_tls_connect,
                         here_tracking_tls,
                         const char*,
                         uint16_t);

DECLARE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_tls_close, here_tracking_tls);

DECLARE_FAKE_VALUE_FUNC3(here_tracking_error,
                         here_tracking_tls_read,
                         here_tracking_tls,
                         char*,
                         uint32_t*);

DECLARE_FAKE_VALUE_FUNC3(here_tracking_error,
                         here_tracking_tls_write,
                         here_tracking_tls,
                         const char*,
                         uint32_t*);

#define MOCK_HERE_TRACKING_TLS_FAKE_LIST(FAKE) \
    FAKE(here_tracking_tls_init) \
    FAKE(here_tracking_tls_free) \
    FAKE(here_tracking_tls_connect) \
    FAKE(here_tracking_tls_close) \
    FAKE(here_tracking_tls_read) \
    FAKE(here_tracking_tls_write)

here_tracking_error mock_here_tracking_tls_init_custom(here_tracking_tls* tls);

here_tracking_error mock_here_tracking_tls_free_custom(here_tracking_tls* tls);

void mock_here_tracking_tls_read_set_result_data(const char** data,
                                                 uint32_t* data_size,
                                                 uint32_t chunks);

here_tracking_error mock_here_tracking_tls_read_custom(here_tracking_tls tls,
                                                       char* data,
                                                       uint32_t* data_size);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_HERE_TRACKING_TLS_H */
