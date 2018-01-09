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


#include <stdio.h>
#include <string.h>

#include "mock_here_tracking_data_buffer.h"

/**************************************************************************************************/

DEFINE_FAKE_VALUE_FUNC3(here_tracking_error,
                        here_tracking_data_buffer_init,
                        here_tracking_data_buffer*,
                        char*,
                        uint32_t);

DEFINE_FAKE_VALUE_FUNC2(here_tracking_error,
                        here_tracking_data_buffer_add_char,
                        here_tracking_data_buffer*,
                        char);

DEFINE_FAKE_VALUE_FUNC2(here_tracking_error,
                        here_tracking_data_buffer_add_string,
                        here_tracking_data_buffer*,
                        const char*);

DEFINE_FAKE_VALUE_FUNC3(here_tracking_error,
                        here_tracking_data_buffer_add_data,
                        here_tracking_data_buffer*,
                        const char*,
                        uint32_t);

DEFINE_FAKE_VALUE_FUNC2(here_tracking_error,
                        here_tracking_data_buffer_add_utoa,
                        here_tracking_data_buffer*,
                        uint32_t);

/**************************************************************************************************/

here_tracking_error mock_here_tracking_data_buffer_init_custom(here_tracking_data_buffer* data_buf,
                                                               char* buf,
                                                               uint32_t buf_size)
{
    if(here_tracking_data_buffer_init_fake.return_val == HERE_TRACKING_OK)
    {
        data_buf->buffer = buf;
        data_buf->buffer_capacity = buf_size;
        data_buf->buffer_size = 0;
    }

    return here_tracking_data_buffer_init_fake.return_val;
}

/**************************************************************************************************/

here_tracking_error \
    mock_here_tracking_data_buffer_add_char_custom(here_tracking_data_buffer* data_buf,
                                                   char c)
{
    if(here_tracking_data_buffer_add_char_fake.return_val == HERE_TRACKING_OK)
    {
        data_buf->buffer[data_buf->buffer_size++] = c;
    }

    return here_tracking_data_buffer_add_char_fake.return_val;
}

/**************************************************************************************************/

here_tracking_error \
    mock_here_tracking_data_buffer_add_string_custom(here_tracking_data_buffer* data_buf,
                                                     const char* s)
{
    if(here_tracking_data_buffer_add_string_fake.return_val == HERE_TRACKING_OK)
    {
        memcpy(data_buf->buffer + data_buf->buffer_size, s, strlen(s));
        data_buf->buffer_size += strlen(s);
    }

    return here_tracking_data_buffer_add_string_fake.return_val;
}

/**************************************************************************************************/

here_tracking_error \
    mock_here_tracking_data_buffer_add_data_custom(here_tracking_data_buffer* data_buf,
                                                   const char* d,
                                                   uint32_t d_size)
{
    here_tracking_error err;
    here_tracking_data_buffer_add_data_Fake* the_fake = &here_tracking_data_buffer_add_data_fake;

    if(the_fake->return_val_seq_len > 0)
    {
        if(the_fake->return_val_seq_idx < the_fake->return_val_seq_len)
        {
            err = the_fake->return_val_seq[the_fake->return_val_seq_idx++];
        }
        else
        {
            err = the_fake->return_val_seq[the_fake->return_val_seq_len - 1];
        }
    }
    else
    {
        err = the_fake->return_val;
    }

    if(err == HERE_TRACKING_OK)
    {
        memcpy(data_buf->buffer + data_buf->buffer_size, d, d_size);
        data_buf->buffer_size += d_size;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error \
    mock_here_tracking_data_buffer_add_utoa_custom(here_tracking_data_buffer* data_buf,
                                                   uint32_t u)
{
    if(here_tracking_data_buffer_add_utoa_fake.return_val == HERE_TRACKING_OK)
    {
        sprintf(data_buf->buffer + data_buf->buffer_size, "%u", u);
        data_buf->buffer_size += strlen(data_buf->buffer + data_buf->buffer_size);
    }

    return here_tracking_data_buffer_add_utoa_fake.return_val;
}
