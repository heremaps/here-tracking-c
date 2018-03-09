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

#ifndef MOCK_HERE_TRACKING_TLS_WRITER_H
#define MOCK_HERE_TRACKING_TLS_WRITER_H

#include <fff.h>

#include "here_tracking_tls_writer.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC4(here_tracking_error,
                         here_tracking_tls_writer_init,
                         here_tracking_tls_writer*,
                         here_tracking_tls,
                         uint8_t*,
                         size_t);

DECLARE_FAKE_VALUE_FUNC2(here_tracking_error,
                         here_tracking_tls_writer_write_char,
                         here_tracking_tls_writer*,
                         char);

DECLARE_FAKE_VALUE_FUNC3(here_tracking_error,
                         here_tracking_tls_writer_write_data,
                         here_tracking_tls_writer*,
                         const uint8_t*,
                         size_t);

DECLARE_FAKE_VALUE_FUNC2(here_tracking_error,
                         here_tracking_tls_writer_write_string,
                         here_tracking_tls_writer*,
                         const char*);

DECLARE_FAKE_VALUE_FUNC3(here_tracking_error,
                         here_tracking_tls_writer_write_utoa,
                         here_tracking_tls_writer*,
                         uint32_t,
                         uint8_t);

DECLARE_FAKE_VALUE_FUNC1(here_tracking_error,
                         here_tracking_tls_writer_flush,
                         here_tracking_tls_writer*);

#define MOCK_HERE_TRACKING_TLS_WRITER_FAKE_LIST(FAKE) \
    FAKE(here_tracking_tls_writer_init) \
    FAKE(here_tracking_tls_writer_write_char) \
    FAKE(here_tracking_tls_writer_write_string) \
    FAKE(here_tracking_tls_writer_write_data) \
    FAKE(here_tracking_tls_writer_write_utoa) \
    FAKE(here_tracking_tls_writer_flush)

#ifdef __cplusplus
}
#endif

#endif /* MOCK_HERE_TRACKING_TLS_WRITER_H */
