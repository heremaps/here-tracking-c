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


#ifndef HERE_TRACKING_HTTP_PARSER_H
#define HERE_TRACKING_HTTP_PARSER_H

#include <stdbool.h>
#include <stdint.h>

#include "here_tracking_error.h"
#include "here_tracking_data_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HERE_TRACKING_HTTP_PARSER_CONTENT_LENGTH_UNKNOWN -1

/**
 * Parser event identifiers
 */
typedef enum
{
    /** HTTP version info */
    HERE_TRACKING_HTTP_PARSER_EVT_VERSION     = 0,
    /** HTTP status code */
    HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE = 1,
    /** HTTP reason phrase */
    HERE_TRACKING_HTTP_PARSER_EVT_REASON      = 2,
    /** HTTP header */
    HERE_TRACKING_HTTP_PARSER_EVT_HDR         = 3,
    /** HTTP response body */
    HERE_TRACKING_HTTP_PARSER_EVT_BODY        = 4,
    /** HTTP response body size */
    HERE_TRACKING_HTTP_PARSER_EVT_BODY_SIZE   = 5
} here_tracking_http_parser_evt_id;

/**
 * Parser version event data
 */
typedef struct
{
    /** HTTP major version */
    uint16_t major;
    /** HTTP minor version */
    uint16_t minor;
} here_tracking_http_parser_evt_version;

/**
 * Parser status code event data
 */
typedef uint16_t here_tracking_http_parser_evt_status_code;

/**
 * Parser reason phrase event data
 */
typedef here_tracking_data_buffer here_tracking_http_parser_evt_reason;

/** Parser header event data */
typedef struct
{
    uint16_t hdr_key_size;
    uint16_t hdr_val_size;
    const char* hdr_key;
    const char* hdr_val;
} here_tracking_http_parser_evt_hdr;

/**
 * Parser body event data
 */
typedef here_tracking_data_buffer here_tracking_http_parser_evt_body;

/**
 * Parser body size event data
 */
typedef uint32_t here_tracking_http_parser_evt_body_size;

typedef union
{
    here_tracking_http_parser_evt_version version;
    here_tracking_http_parser_evt_status_code status_code;
    here_tracking_http_parser_evt_reason reason;
    here_tracking_http_parser_evt_hdr hdr;
    here_tracking_http_parser_evt_body body;
    here_tracking_http_parser_evt_body_size body_size;
} here_tracking_http_parser_evt_data;

/**
 * Parser callback event
 */
typedef struct
{
    /** Event identifier */
    here_tracking_http_parser_evt_id id;
    /** Event specific data */
    here_tracking_http_parser_evt_data data;
} here_tracking_http_parser_evt;

/**
 * Parser event callback
 *
 * @param evt Event data
 * @param last Is this last part of the event
 * @param cb_data Callback data
 * @return true if client wants to stop parsing after the cb, false to continue parsing.
 */
typedef bool (*here_tracking_http_parser_evt_cb)(const here_tracking_http_parser_evt* evt,
                                                 bool last,
                                                 void* cb_data);

typedef struct
{
    /** Current event parser is in */
    here_tracking_http_parser_evt_id evt_state;
    /** Expected content size */
    int32_t content_size;
    /** Event callback */
    here_tracking_http_parser_evt_cb cb;
    /** Data pointer passed in event callback */
    void* cb_data;
} here_tracking_http_parser;

/**
 *  Initialize HTTP response parser
 *
 *  @param parser Pointer to HTTP parser
 *  @param cb Event callback function
 *  @param cb_data Event callback data pointer
 *  @return Code defining operation success
 */
here_tracking_error here_tracking_http_parser_init(here_tracking_http_parser* parser,
                                                   here_tracking_http_parser_evt_cb cb,
                                                   void* cb_data);

/**
 *  Parse HTTP response data.
 *
 *  @param parser Pointer to initialized HTTP parser
 *  @param data Response data buffer.
 *  @param[in,out] data_size Size of data buffer in bytes. Set to number of bytes parsed in return.
 *  @return Code defining operation success
 *          HERE_TRACKING_OK - parsing completed successfully
 *          HERE_TRACKING_ERROR_CLIENT_INTERRUPT - client requested parsing to stop
 *          HERE_TRACKING_ERROR_NEED_MORE_DATA - parsing not complete, more data required to proceed
 *          Others - parsing error
 */
here_tracking_error here_tracking_http_parser_parse(here_tracking_http_parser* parser,
                                                    const char* data,
                                                    uint32_t* data_size);

#ifdef __cplusplus
}
#endif

#endif /* HERE_TRACKING_HTTP_PARSER_H */
