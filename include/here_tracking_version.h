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

/**
 * @file here_tracking_version.h
 * @brief HERE Tracking library version information.
 */

#ifndef HERE_TRACKING_VERSION_H
#define HERE_TRACKING_VERSION_H

#include <stdint.h>

#define HERE_TRACKING_VERSION_MAJOR 1 /**< @brief Major version number */

#define HERE_TRACKING_VERSION_MINOR 1 /**< @brief Minor version number */

#define HERE_TRACKING_VERSION_PATCH 1 /**< @brief Patch version number */

/**
 * @brief Version string in format "Major.Minor.Patch".
 */
#define HERE_TRACKING_VERSION_STRING "1.1.1"

/**
 * @brief Get major version number
 *
 * @return Major version number
 */
uint16_t here_tracking_get_version_major(void);

/**
 * @brief Get minor version number
 *
 * @return Minor version number
 */
uint16_t here_tracking_get_version_minor(void);

/**
 * @brief Get patch version number
 *
 * @return Patch version number
 */
uint16_t here_tracking_get_version_patch(void);

/**
 * @brief Get version number
 *
 * @param[out] major Major version number
 * @param[out] minor Minor version number
 * @param[out] patch Patch version number
 */
void here_tracking_get_version(uint16_t* major, uint16_t* minor, uint16_t* patch);

/**
 * @brief Get version string
 *
 * @return Version string
 */
const char* here_tracking_get_version_string(void);

#endif /* HERE_TRACKING_VERSION_H */
