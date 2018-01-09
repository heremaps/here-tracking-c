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


#ifndef HERE_TRACKING_DATA_BUFFER_H
#define HERE_TRACKING_DATA_BUFFER_H

#include <stdint.h>

#include "here_tracking_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char* buffer;
    uint32_t buffer_capacity;
    uint32_t buffer_size;
} here_tracking_data_buffer;

here_tracking_error here_tracking_data_buffer_init(here_tracking_data_buffer* data_buffer,
                                                   char* buffer,
                                                   uint32_t buffer_size);

here_tracking_error here_tracking_data_buffer_add_char(here_tracking_data_buffer* data_buffer,
                                                       char c);

here_tracking_error here_tracking_data_buffer_add_string(here_tracking_data_buffer* data_buffer,
                                                         const char* s);

here_tracking_error here_tracking_data_buffer_add_data(here_tracking_data_buffer* data_buffer,
                                                       const char* d,
                                                       uint32_t d_size);

here_tracking_error here_tracking_data_buffer_add_utoa(here_tracking_data_buffer* data_buffer,
                                                       uint32_t u);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_DATA_BUFFER_H */
