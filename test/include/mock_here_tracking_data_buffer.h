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


#ifndef MOCK_HERE_TRACKING_DATA_BUFFER_H
#define MOCK_HERE_TRACKING_DATA_BUFFER_H

#include <fff.h>

#include "here_tracking_data_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC3(here_tracking_error,
                         here_tracking_data_buffer_init,
                         here_tracking_data_buffer*,
                         char*,
                         uint32_t);

DECLARE_FAKE_VALUE_FUNC2(here_tracking_error,
                         here_tracking_data_buffer_add_char,
                         here_tracking_data_buffer*,
                         char);

DECLARE_FAKE_VALUE_FUNC2(here_tracking_error,
                         here_tracking_data_buffer_add_string,
                         here_tracking_data_buffer*,
                         const char*);

DECLARE_FAKE_VALUE_FUNC3(here_tracking_error,
                         here_tracking_data_buffer_add_data,
                         here_tracking_data_buffer*,
                         const char*,
                         uint32_t);

DECLARE_FAKE_VALUE_FUNC2(here_tracking_error,
                         here_tracking_data_buffer_add_utoa,
                         here_tracking_data_buffer*,
                         uint32_t);

#define MOCK_HERE_TRACKING_DATA_BUFFER_FAKE_LIST(FAKE) \
    FAKE(here_tracking_data_buffer_init) \
    FAKE(here_tracking_data_buffer_add_char) \
    FAKE(here_tracking_data_buffer_add_string) \
    FAKE(here_tracking_data_buffer_add_data) \
    FAKE(here_tracking_data_buffer_add_utoa)

here_tracking_error mock_here_tracking_data_buffer_init_custom(here_tracking_data_buffer* data_buf,
                                                               char* buf,
                                                               uint32_t buf_size);

here_tracking_error \
    mock_here_tracking_data_buffer_add_char_custom(here_tracking_data_buffer* data_buf,
                                                   char c);

here_tracking_error \
    mock_here_tracking_data_buffer_add_string_custom(here_tracking_data_buffer* data_buf,
                                                     const char* s);

here_tracking_error \
    mock_here_tracking_data_buffer_add_data_custom(here_tracking_data_buffer* data_buf,
                                                   const char* d,
                                                   uint32_t d_size);

here_tracking_error \
    mock_here_tracking_data_buffer_add_utoa_custom(here_tracking_data_buffer* data_buf,
                                                   uint32_t u);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_HERE_TRACKING_DATA_BUFFER_H */
