/**************************************************************************************************
 * Copyright (C) 2018 HERE Europe B.V.                                                            *
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

#include <stdlib.h>

#include "here_tracking_time.h"
#include "here_tracking_uuid_gen.h"

/**************************************************************************************************/

static const char HERE_TRACKING_UUID_GEN_V4  = '4';

static const char HERE_TRACKING_UUID_GEN_V4_VARIANT_1 = 'a';

static void here_tracking_uuid_gen_add_bytes(char* buf, size_t n);

/**************************************************************************************************/

here_tracking_error here_tracking_uuid_gen_new(char* buf, size_t buf_size)
{
    here_tracking_error err = HERE_TRACKING_OK;

    if(buf != NULL)
    {
        uint32_t ts;

        if(buf_size < HERE_TRACKING_UUID_SIZE)
        {
            err = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
        }

        if(err == HERE_TRACKING_OK)
        {
            err = here_tracking_get_unixtime(&ts);
        }

        if(err == HERE_TRACKING_OK)
        {
            static uint32_t prev_ts = 0;
            size_t pos = 0;

            /* Make sure the seed is unique */
            if(prev_ts >= ts)
            {
                ts = ++prev_ts;
            }
            else
            {
                prev_ts = ts;
            }

            srand(ts);
            here_tracking_uuid_gen_add_bytes(buf + pos, 8); pos += 8;
            buf[pos++] = '-';
            here_tracking_uuid_gen_add_bytes(buf + pos, 4); pos += 4;
            buf[pos++] = '-';
            buf[pos++] = HERE_TRACKING_UUID_GEN_V4;
            here_tracking_uuid_gen_add_bytes(buf + pos, 3); pos += 3;
            buf[pos++] = '-';
            buf[pos++] = HERE_TRACKING_UUID_GEN_V4_VARIANT_1;
            here_tracking_uuid_gen_add_bytes(buf + pos, 3); pos += 3;
            buf[pos++] = '-';
            here_tracking_uuid_gen_add_bytes(buf + pos, 12); pos += 12;
            buf[pos] = '\0';
        }
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}

/**************************************************************************************************/

static void here_tracking_uuid_gen_add_bytes(char* buf, size_t n)
{
    int i, r;

    for(i = 0; i < n; i++)
    {
        r = rand() % 0x10;

        if(r < 10)
        {
            buf[i] = r + '0';
        }
        else
        {
            buf[i] = r + 'a' - 10;
        }
    }
}
