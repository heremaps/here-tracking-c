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

#include <string.h>

#include "here_tracking_data_buffer.h"

/**************************************************************************************************/

#define HERE_TRACKING_DATA_BUFFER_UINT32_MAX_SIZE 10

/**************************************************************************************************/

here_tracking_error here_tracking_data_buffer_init(here_tracking_data_buffer* data_buffer,
                                                   char* buffer,
                                                   uint32_t buffer_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(data_buffer != NULL && buffer != NULL && buffer_size > 0)
    {
        data_buffer->buffer = buffer;
        data_buffer->buffer_capacity = buffer_size;
        data_buffer->buffer_size = 0;
        err = HERE_TRACKING_OK;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_data_buffer_add_char(here_tracking_data_buffer* data_buffer,
                                                       char c)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(data_buffer != NULL)
    {
        if(HERE_TRACKING_DATA_BUFFER_BYTES_FREE(data_buffer) >= 1)
        {
            data_buffer->buffer[data_buffer->buffer_size++] = c;
            err = HERE_TRACKING_OK;
        }
        else
        {
            err = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
        }
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_data_buffer_add_string(here_tracking_data_buffer* data_buffer,
                                                         const char* s)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(data_buffer != NULL && s != NULL)
    {
        uint32_t len = (uint32_t)strlen(s);

        if(HERE_TRACKING_DATA_BUFFER_BYTES_FREE(data_buffer) >= len)
        {
            memcpy(data_buffer->buffer + data_buffer->buffer_size, s, len);
            data_buffer->buffer_size += len;
            err = HERE_TRACKING_OK;
        }
        else
        {
            err = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
        }
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_data_buffer_add_data(here_tracking_data_buffer* data_buffer,
                                                       const char* d,
                                                       uint32_t d_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(data_buffer != NULL && d != NULL && d_size > 0)
    {
        if(HERE_TRACKING_DATA_BUFFER_BYTES_FREE(data_buffer) >= d_size)
        {
            memcpy(data_buffer->buffer + data_buffer->buffer_size, d, d_size);
            data_buffer->buffer_size += d_size;
            err = HERE_TRACKING_OK;
        }
        else
        {
            err = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
        }
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_data_buffer_add_utoa(here_tracking_data_buffer* data_buffer,
                                                       uint32_t u,
                                                       uint8_t base)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(data_buffer != NULL && (base == 10 || base == 16))
    {
        uint32_t size = 1, divisor = base;

        while(divisor > 0 &&
              (size < HERE_TRACKING_DATA_BUFFER_UINT32_MAX_SIZE && (u / divisor) > 0))
        {
            size++;
            divisor *= base;
        }

        if(HERE_TRACKING_DATA_BUFFER_BYTES_FREE(data_buffer) >= size)
        {
            char* buffer = data_buffer->buffer + data_buffer->buffer_size;
            data_buffer->buffer_size += size;

            while(size > 0)
            {
                uint8_t val = u % base;

                if(val < 0xA)
                {
                    buffer[size - 1] = val + '0';
                }
                else
                {
                    buffer[size -1] = (val - 0xA) + 'A';
                }

                u /= base;
                size--;
            }

            err = HERE_TRACKING_OK;
        }
        else
        {
            err = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
        }
    }

    return err;
}
