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

/**
 * @file here_tracking_log.h
 *
 * @brief Interface definition for the logging interface.
 *
 * @defgroup log_if Logging interface
 * @{
 *
 * @brief Interface definition for the logging interface.
 *
 * If logging is enabled (i.e., ::HERE_TRACKING_LOG_LEVEL != ::HERE_TRACKING_LOG_LEVEL_NONE), you
 * must ensure that the target platform provides an implementation for the logging interface
 * methods.
 *
 * @def HERE_TRACKING_LOG_LEVEL
 * @brief The logging level. The default is ::HERE_TRACKING_LOG_LEVEL_NONE.
 * @hideinitializer
 * @def HERE_TRACKING_LOGF(fmt, ...)
 * @brief Macro for logging a fatal-level message.
 * @hideinitializer
 * @def HERE_TRACKING_LOGE(fmt, ...)
 * @brief Macro for logging an error-level message.
 * @hideinitializer
 * @def HERE_TRACKING_LOGW(fmt, ...)
 * @brief Macro for logging a warning-level message.
 * @hideinitializer
 * @def HERE_TRACKING_LOGI(fmt, ...)
 * @brief Macro for logging an info-level message.
 * @hideinitializer
 */

#ifndef HERE_TRACKING_LOG_H
#define HERE_TRACKING_LOG_H

#include <stdint.h>

#include "here_tracking_log_level.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HERE_TRACKING_LOG_LEVEL
  #define HERE_TRACKING_LOG_LEVEL HERE_TRACKING_LOG_LEVEL_NONE
#endif

#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_FATAL
  #define HERE_TRACKING_LOGF(fmt, ...) \
      here_tracking_log(HERE_TRACKING_LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
  #define HERE_TRACKING_LOGF(fmt, ...)
#endif

#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_ERROR
  #define HERE_TRACKING_LOGE(fmt, ...) \
      here_tracking_log(HERE_TRACKING_LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
  #define HERE_TRACKING_LOGE(fmt, ...)
#endif

#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_WARNING
  #define HERE_TRACKING_LOGW(fmt, ...) \
      here_tracking_log(HERE_TRACKING_LOG_LEVEL_WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
  #define HERE_TRACKING_LOGW(fmt, ...)
#endif

#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_INFO
  #define HERE_TRACKING_LOGI(fmt, ...) \
      here_tracking_log(HERE_TRACKING_LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
  #define HERE_TRACKING_LOGI(fmt, ...)
#endif

/**
 * @brief Log a message.
 *
 * The output stream (file, stdout, ...) depends on the platform implementation. The library code
 * uses the HERE_TRACKING_LOG* macros and doesn't call this function directly, so it's not mandatory
 * for the implementation to check if the log level is enabled in runtime (if this method is called
 * it is).
 *
 * @param level Log level of the message.
 * @param file The file where the message was logged.
 * @param line The line number of the message in the log file.
 * @param fmt A format string in printf format.
 * @param ... Additional message arguments.
 */
void here_tracking_log(uint8_t level, const char* file, int line, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_LOG_H */

/** @} */
