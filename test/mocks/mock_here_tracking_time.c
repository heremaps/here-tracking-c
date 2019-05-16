/**************************************************************************************************
* Copyright (C) 2017-2019 HERE Europe B.V.                                                        *
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


#include "mock_here_tracking_time.h"

/**************************************************************************************************/

static uint32_t mock_here_tracking_get_unixtime_result = 0;

/**************************************************************************************************/

DEFINE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_get_unixtime, uint32_t*);

/**************************************************************************************************/

void mock_here_tracking_get_unixtime_set_result(uint32_t result)
{
    mock_here_tracking_get_unixtime_result = result;
}

/**************************************************************************************************/

here_tracking_error mock_here_tracking_get_unixtime_custom(uint32_t* ts)
{
    here_tracking_error err;
    here_tracking_get_unixtime_Fake* the_fake = &here_tracking_get_unixtime_fake;

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
        *ts = mock_here_tracking_get_unixtime_result;
    }

    return err;
}
