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

#include <string.h>

#include "mock_here_tracking_uuid_gen.h"

/**************************************************************************************************/

static const char* mock_here_tracking_uuid_gen_new_result_default = \
    "58be8c2c-704b-4e75-a8a2-15be9d7ee353";
static char* mock_here_tracking_uuid_gen_new_result = NULL;

/**************************************************************************************************/

DEFINE_FAKE_VALUE_FUNC2(here_tracking_error, here_tracking_uuid_gen_new, char*, size_t);

/**************************************************************************************************/

void mock_here_tracking_uuid_gen_new_set_result(char* result)
{
    mock_here_tracking_uuid_gen_new_result = result;
}

/**************************************************************************************************/

here_tracking_error mock_here_tracking_uuid_gen_new_custom(char* buf, size_t buf_size)
{
    if(here_tracking_uuid_gen_new_fake.return_val == HERE_TRACKING_OK)
    {
        if(mock_here_tracking_uuid_gen_new_result == NULL)
        {
            strcpy(buf, mock_here_tracking_uuid_gen_new_result_default);
        }
        else
        {
            strcpy(buf, mock_here_tracking_uuid_gen_new_result);
        }
    }

    return here_tracking_uuid_gen_new_fake.return_val;
}
