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
 * @file here_tracking_log_level.h
 * @brief Log level definitions.
 */

#ifndef HERE_TRACKING_LOG_LEVEL_H
#define HERE_TRACKING_LOG_LEVEL_H

/** @brief No log messages. */
#define HERE_TRACKING_LOG_LEVEL_NONE     100

/** @brief Enable log messages up to the level `fatal`.*/
#define HERE_TRACKING_LOG_LEVEL_FATAL     90

/** @brief Enable log messages up to the level `error`. */
#define HERE_TRACKING_LOG_LEVEL_ERROR     70

/** @brief Enable log messages up to the level `warning`. */
#define HERE_TRACKING_LOG_LEVEL_WARNING   40

/** @brief Enable log messages up to the level `info`. */
#define HERE_TRACKING_LOG_LEVEL_INFO      20

#endif /* HERE_TRACKING_LOG_LEVEL_H */
