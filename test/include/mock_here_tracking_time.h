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


#ifndef MOCK_HERE_TRACKING_TIME_H
#define MOCK_HERE_TRACKING_TIME_H

#include <fff.h>

#include "here_tracking_time.h"

#ifdef __cplusplus
extern "C" {
#endif

DECLARE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_get_unixtime, uint32_t*);

#define MOCK_HERE_TRACKING_TIME_FAKE_LIST(FAKE) \
    FAKE(here_tracking_get_unixtime)

void mock_here_tracking_get_unixtime_set_result(uint32_t result);

here_tracking_error mock_here_tracking_get_unixtime_custom(uint32_t* ts);

#ifdef __cplusplus
}
#endif

#endif /* MOCK_HERE_TRACKING_TIME_H */
