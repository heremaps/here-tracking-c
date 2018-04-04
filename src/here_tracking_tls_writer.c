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

#include "here_tracking_tls_writer.h"

/**************************************************************************************************/

#define TRY(OP) if((err = (OP)) != HERE_TRACKING_OK) { goto here_tracking_tls_writer_error; }

/**************************************************************************************************/

here_tracking_error here_tracking_tls_writer_init(here_tracking_tls_writer* writer,
                                                  here_tracking_tls tls_ctx,
                                                  uint8_t* write_buf,
                                                  size_t write_buf_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(writer != NULL && tls_ctx != NULL && write_buf != NULL && write_buf_size > 0)
    {
        writer->tls_ctx = tls_ctx;
        err = here_tracking_data_buffer_init(&writer->data_buffer,
                                             (char*)write_buf,
                                             (uint32_t)write_buf_size);
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_writer_write_char(here_tracking_tls_writer* writer,
                                                        char c)
{
    return here_tracking_tls_writer_write_data(writer, (const uint8_t*)(&c), sizeof(char));
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_writer_write_data(here_tracking_tls_writer* writer,
                                                        const uint8_t* data,
                                                        size_t data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(writer != NULL && data != NULL && data_size > 0)
    {
        size_t pos = 0;
        err = HERE_TRACKING_OK;

        while(err == HERE_TRACKING_OK && data_size > 0)
        {
            err = here_tracking_data_buffer_add_data(&writer->data_buffer,
                                                     ((const char*)data) + pos,
                                                     (uint32_t)data_size);

            if(err == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL)
            {
                size_t bytes_free = HERE_TRACKING_DATA_BUFFER_BYTES_FREE(&writer->data_buffer);

                err = here_tracking_data_buffer_add_data(&writer->data_buffer,
                                                         ((const char*)data) + pos,
                                                         (uint32_t)bytes_free);

                if(err == HERE_TRACKING_OK)
                {
                    pos += bytes_free;
                    data_size -= bytes_free;
                    err = here_tracking_tls_writer_flush(writer);
                }
            }
            else if(err == HERE_TRACKING_OK)
            {
                pos += data_size;
                data_size = 0;

                if(HERE_TRACKING_DATA_BUFFER_BYTES_FREE(&writer->data_buffer) == 0)
                {
                    err = here_tracking_tls_writer_flush(writer);
                }
            }
        }
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_writer_write_string(here_tracking_tls_writer* writer,
                                                          const char* s)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(s != NULL)
    {
        err = here_tracking_tls_writer_write_data(writer, (const uint8_t*)s, strlen(s));
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_writer_write_utoa(here_tracking_tls_writer* writer,
                                                        uint32_t u,
                                                        uint8_t base)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(writer != NULL)
    {
        here_tracking_data_buffer data_buffer;
        uint8_t buf[10];

        TRY((here_tracking_data_buffer_init(&data_buffer, (char*)buf, 10)));
        TRY((here_tracking_data_buffer_add_utoa(&data_buffer, u, base)));
        TRY((here_tracking_tls_writer_write_data(writer,
                                                 (uint8_t*)data_buffer.buffer,
                                                 data_buffer.buffer_size)));
    }

here_tracking_tls_writer_error:
    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_tls_writer_flush(here_tracking_tls_writer* writer)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(writer != NULL)
    {
        uint32_t write_size = writer->data_buffer.buffer_size, pos = 0;
        err = HERE_TRACKING_OK;

        while(err == HERE_TRACKING_OK && write_size > 0)
        {
            err = here_tracking_tls_write(writer->tls_ctx,
                                          writer->data_buffer.buffer + pos,
                                          &write_size);

            if(err == HERE_TRACKING_OK)
            {
                pos += write_size;
                write_size = writer->data_buffer.buffer_size - pos;
            }
        }

        if(err == HERE_TRACKING_OK)
        {
            writer->data_buffer.buffer_size = 0;
        }
    }

    return err;
}
