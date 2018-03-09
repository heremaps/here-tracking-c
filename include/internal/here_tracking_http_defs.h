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

#ifndef HERE_TRACKING_HTTP_DEFS_H
#define HERE_TRACKING_HTTP_DEFS_H

#define HERE_TRACKING_HTTP_STATUS_OK                  200
#define HERE_TRACKING_HTTP_STATUS_NO_CONTENT          204
#define HERE_TRACKING_HTTP_STATUS_BAD_REQUEST         400
#define HERE_TRACKING_HTTP_STATUS_UNAUTHORIZED        401
#define HERE_TRACKING_HTTP_STATUS_FORBIDDEN           403
#define HERE_TRACKING_HTTP_STATUS_NOT_FOUND           404
#define HERE_TRACKING_HTTP_STATUS_PRECONDITION_FAILED 412

extern const char* here_tracking_http_connection_close;
extern const char* here_tracking_http_crlf;
extern const char* here_tracking_http_header_authorization;
extern const char* here_tracking_http_header_connection;
extern const char* here_tracking_http_header_content_length;
extern const char* here_tracking_http_header_transfer_encoding;
extern const char* here_tracking_http_method_get;
extern const char* here_tracking_http_method_post;
extern const char* here_tracking_http_path_token;
extern const char* here_tracking_http_path_version;
extern const char* here_tracking_http_protocol_https;
extern const char* here_tracking_http_transfer_encoding_chunked;

#endif /* HERE_TRACKING_HTTP_DEFS_H */
