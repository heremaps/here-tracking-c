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

#include <stdarg.h>
#include <stdio.h>

#include "here_tracking_log.h"

static const char* here_tracking_log_level_fatal = "FATAL";
static const char* here_tracking_log_level_error = "ERROR";
static const char* here_tracking_log_level_warning = "WARNING";
static const char* here_tracking_log_level_info = "INFO";

static void here_tracking_log_msg(FILE* f,
                                  const char* level,
                                  const char* file,
                                  int line,
                                  const char* fmt,
                                  va_list args);

/**************************************************************************************************/

void here_tracking_log(uint8_t level, const char* file, int line, const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    switch(level)
    {
        case HERE_TRACKING_LOG_LEVEL_FATAL:
        {
            here_tracking_log_msg(stdout, here_tracking_log_level_fatal, file, line, fmt, args);
        }
        break;

        case HERE_TRACKING_LOG_LEVEL_ERROR:
        {
            here_tracking_log_msg(stderr, here_tracking_log_level_error, file, line, fmt, args);
        }
        break;

        case HERE_TRACKING_LOG_LEVEL_WARNING:
        {
            here_tracking_log_msg(stdout, here_tracking_log_level_warning, file, line, fmt, args);
        }
        break;

        case HERE_TRACKING_LOG_LEVEL_INFO:
        {
            here_tracking_log_msg(stdout, here_tracking_log_level_info, file, line, fmt, args);
        }
        break;

        default:
            break;
    }

    va_end(args);
}

/**************************************************************************************************/

static void here_tracking_log_msg(FILE* f,
                                  const char* level,
                                  const char* file,
                                  int line,
                                  const char* fmt,
                                  va_list args)
{
    fprintf(f, "[HERE_TRACKING_C/%s][%s:%d]:", level, file, line);
    vfprintf(f, fmt, args);
    fprintf(f, "\n");
}
