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


#include "mock_here_tracking_tls.h"

/**************************************************************************************************/

static const char** mock_tls_read_result_data = NULL;

static uint32_t* mock_tls_read_result_data_size = NULL;

static uint32_t mock_tls_read_result_chunk_count = 0;

static uint32_t mock_tls_read_current_chunk = 0;

static uint32_t mock_tls_read_current_chunk_offset = 0;

/**************************************************************************************************/

DEFINE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_tls_init, here_tracking_tls*);

DEFINE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_tls_free, here_tracking_tls*);

DEFINE_FAKE_VALUE_FUNC3(here_tracking_error,
                        here_tracking_tls_connect,
                        here_tracking_tls,
                        const char*,
                        uint16_t);

DEFINE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_tls_close, here_tracking_tls);

DEFINE_FAKE_VALUE_FUNC3(here_tracking_error,
                        here_tracking_tls_read,
                        here_tracking_tls,
                        char*,
                        uint32_t*);

DEFINE_FAKE_VALUE_FUNC3(here_tracking_error,
                        here_tracking_tls_write,
                        here_tracking_tls,
                        const char*,
                        uint32_t*);

/**************************************************************************************************/

here_tracking_error mock_here_tracking_tls_init_custom(here_tracking_tls* tls)
{
    if(here_tracking_tls_init_fake.return_val == HERE_TRACKING_OK)
    {
        *tls = (here_tracking_tls)1;
    }

    return here_tracking_tls_init_fake.return_val;
}

/**************************************************************************************************/

here_tracking_error mock_here_tracking_tls_free_custom(here_tracking_tls* tls)
{
    if(here_tracking_tls_free_fake.return_val == HERE_TRACKING_OK)
    {
        *tls = NULL;
    }

    return here_tracking_tls_free_fake.return_val;
}

/**************************************************************************************************/

void mock_here_tracking_tls_read_set_result_data(const char** data,
                                                 uint32_t* data_size,
                                                 uint32_t chunks)
{
    mock_tls_read_result_data = data;
    mock_tls_read_result_data_size = data_size;
    mock_tls_read_result_chunk_count = chunks;
    mock_tls_read_current_chunk = 0;
    mock_tls_read_current_chunk_offset = 0;
}

/**************************************************************************************************/

here_tracking_error mock_here_tracking_tls_read_custom(here_tracking_tls tls,
                                                       char* data,
                                                       uint32_t* data_size)
{
    if(here_tracking_tls_read_fake.return_val == HERE_TRACKING_OK)
    {
        if(mock_tls_read_current_chunk < mock_tls_read_result_chunk_count)
        {
            uint32_t data_in_chunk = mock_tls_read_result_data_size[mock_tls_read_current_chunk] -
                                     mock_tls_read_current_chunk_offset;
            const char* data_pos = (mock_tls_read_result_data[mock_tls_read_current_chunk]) +
                                   mock_tls_read_current_chunk_offset;

            if(data_in_chunk > (*data_size))
            {
                memcpy(data, data_pos, (*data_size));
                mock_tls_read_current_chunk_offset += (*data_size);
            }
            else
            {
                memcpy(data, data_pos, data_in_chunk);
                mock_tls_read_current_chunk_offset = 0;
                mock_tls_read_current_chunk++;
                (*data_size) = data_in_chunk;
            }
        }
    }

    return here_tracking_tls_read_fake.return_val;
}
