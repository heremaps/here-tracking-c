/**************************************************************************************************
 * Copyright (C) 2017 HERE Europe B.V.                                                            *
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

#include <ctype.h>

#include "here_tracking_utils.h"

/**************************************************************************************************/

bool here_tracking_utils_isalnum(const char c)
{
    return (here_tracking_utils_isalpha(c) || here_tracking_utils_isdigit(c)) ? true : false;
}

/**************************************************************************************************/

bool here_tracking_utils_isalpha(const char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) ? true : false;
}

/**************************************************************************************************/

bool here_tracking_utils_isdigit(const char c)
{
    return (c >= '0' && c <= '9') ? true : false;
}

/**************************************************************************************************/

int32_t here_tracking_utils_memcasecmp(const uint8_t* b1, const uint8_t* b2, uint32_t n)
{
    uint32_t i;

    for(i = 0; tolower(*b1) == tolower(*b2) && i < n; ++i, ++b1, ++b2) { /* NOP */ }

    if(i == n)
    {
        return 0;
    }
    else
    {
        return (*b1 < *b2) ? -1 : 1;
    }
}
