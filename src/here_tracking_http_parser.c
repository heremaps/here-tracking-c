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

#include <stdlib.h>
#include <string.h>

#include "here_tracking_http.h"
#include "here_tracking_http_parser.h"
#include "here_tracking_utils.h"

/**************************************************************************************************/

static const char* here_tracking_http_parser_http_proto = "HTTP/";

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_http_parser_parse_version(here_tracking_http_parser* parser,
                                            const char* data,
                                            uint32_t* data_size);

static here_tracking_error \
    here_tracking_http_parser_parse_status_code(here_tracking_http_parser* parser,
                                                const char* data,
                                                uint32_t* data_size);

static here_tracking_error here_tracking_http_parser_parse_reason(here_tracking_http_parser* parser,
                                                                  const char* data,
                                                                  uint32_t* data_size);

static here_tracking_error here_tracking_http_parser_hdr_end(here_tracking_http_parser* parser,
                                                             const char* data,
                                                             uint32_t* data_size);

static here_tracking_error \
    here_tracking_http_parser_build_hdr_evt(const char* data,
                                            uint32_t data_size,
                                            here_tracking_http_parser_evt* evt);

static here_tracking_error here_tracking_http_parser_parse_hdr(here_tracking_http_parser* parser,
                                                               const char* data,
                                                               uint32_t* data_size);

static here_tracking_error here_tracking_http_parser_parse_body(here_tracking_http_parser* parser,
                                                                const char* data,
                                                                uint32_t* data_size);

/**************************************************************************************************/

#define HERE_TRACKING_HTTP_PARSER_CMP(DATA, STR) memcmp(DATA, STR, strlen(STR))

/**************************************************************************************************/

here_tracking_error here_tracking_http_parser_init(here_tracking_http_parser* parser,
                                                   here_tracking_http_parser_evt_cb cb,
                                                   void* cb_data)
{
    here_tracking_error err = HERE_TRACKING_OK;

    if(parser != NULL && cb != NULL)
    {
        parser->evt_state = HERE_TRACKING_HTTP_PARSER_EVT_VERSION;
        parser->cb = cb;
        parser->cb_data = cb_data;
        parser->content_size = HERE_TRACKING_HTTP_PARSER_CONTENT_LENGTH_UNKNOWN;
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_http_parser_parse(here_tracking_http_parser* parser,
                                                    const char* data,
                                                    uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_OK;

    if(parser != NULL && data != NULL && data_size != NULL && (*data_size) > 0)
    {
        uint32_t pos = 0, size = (*data_size);

        /* Run as long as all is well and new data is available */
        while(err == HERE_TRACKING_OK && pos < (*data_size))
        {
            switch(parser->evt_state)
            {
                case HERE_TRACKING_HTTP_PARSER_EVT_VERSION:
                {
                    err = here_tracking_http_parser_parse_version(parser, (data + pos), &size);
                }
                break;

                case HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE:
                {
                    err = here_tracking_http_parser_parse_status_code(parser, (data + pos), &size);
                }
                break;

                case HERE_TRACKING_HTTP_PARSER_EVT_REASON:
                {
                    err = here_tracking_http_parser_parse_reason(parser, (data + pos), &size);
                }
                break;

                case HERE_TRACKING_HTTP_PARSER_EVT_HDR:
                {
                    err = here_tracking_http_parser_parse_hdr(parser, (data + pos), &size);
                }
                break;

                case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
                {
                    err = here_tracking_http_parser_parse_body(parser, (data + pos), &size);
                }
                break;

                default:
                {
                    /* Should not be here */
                    err = HERE_TRACKING_ERROR;
                }
                break;
            }

            /* Only return codes that allow re-entry set size info correctly. Increment position. */
            if(err == HERE_TRACKING_OK || err == HERE_TRACKING_ERROR_NEED_MORE_DATA)
            {
                pos += size;
                size = (*data_size) - pos;
            }
        }

        /* It is possible to exit the parsing loop with OK status even if response is not complete.
           This depends on how data is passed in. Make sure final result is OK only when entire
           response has been parsed.*/
        if(err == HERE_TRACKING_OK)
        {
            if(parser->evt_state == HERE_TRACKING_HTTP_PARSER_EVT_BODY)
            {
                /* Content left to parse */
                if(parser->content_size > 0)
                {
                    err = HERE_TRACKING_ERROR_NEED_MORE_DATA;
                }
            }
            else
            {
                /* Not yet in final state (EVT_BODY) */
                err = HERE_TRACKING_ERROR_NEED_MORE_DATA;
            }
        }

        (*data_size) = pos;
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_http_parser_parse_version(here_tracking_http_parser* parser,
                                            const char* data,
                                            uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_NEED_MORE_DATA;

    /* Minimum length is 9 characters. "HTTP/" + MAJOR + '.' + MINOR + SP */
    if((*data_size) >= strlen(here_tracking_http_parser_http_proto) + 4 * sizeof(char))
    {
        /* Must start with "HTTP/" */
        if(HERE_TRACKING_HTTP_PARSER_CMP(data, here_tracking_http_parser_http_proto) == 0)
        {
            uint32_t pos = (uint32_t)strlen(here_tracking_http_parser_http_proto);
            uint32_t major_pos = pos;
            uint32_t minor_pos = pos;

            while(pos < (*data_size))
            {
                if(data[pos] == '.')
                {
                    minor_pos = pos + 1;
                }
                else if(data[pos] == ' ')
                {
                    if((minor_pos - major_pos) < 2 || (pos - minor_pos) < 1)
                    {
                        err = HERE_TRACKING_ERROR;
                        break;
                    }
                    else
                    {
                        here_tracking_http_parser_evt evt;

                        evt.id = HERE_TRACKING_HTTP_PARSER_EVT_VERSION;
                        evt.data.version.major = atoi(data + major_pos);
                        evt.data.version.minor = atoi(data + minor_pos);
                        (*data_size) = pos + 1;
                        err = parser->cb(&evt, true, parser->cb_data) ?
                            HERE_TRACKING_ERROR_CLIENT_INTERRUPT : HERE_TRACKING_OK;
                        parser->evt_state = HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE;
                        break;
                    }
                }
                else
                {
                    if(!here_tracking_utils_isdigit(data[pos]))
                    {
                        err = HERE_TRACKING_ERROR;
                        break;
                    }
                }

                pos++;
            }
        }
        else
        {
            err = HERE_TRACKING_ERROR;
        }
    }

    if(err == HERE_TRACKING_ERROR_NEED_MORE_DATA)
    {
        (*data_size) = 0;
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_http_parser_parse_status_code(here_tracking_http_parser* parser,
                                                const char* data,
                                                uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_NEED_MORE_DATA;

    /* Minimum 4 bytes required to parse. 3 for code, 1 one for whitespace */
    if((*data_size) >= 4)
    {
        /* 3 digits + whitespace is required format */
        if(here_tracking_utils_isdigit(data[0]) &&
           here_tracking_utils_isdigit(data[1]) &&
           here_tracking_utils_isdigit(data[2]) &&
           data[3] == ' ')
        {
            here_tracking_http_parser_evt evt;

            evt.id = HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE;
            evt.data.status_code = atoi(data);
            (*data_size) = 4;
            err = parser->cb(&evt, true, parser->cb_data) ?
                HERE_TRACKING_ERROR_CLIENT_INTERRUPT : HERE_TRACKING_OK;
            parser->evt_state = HERE_TRACKING_HTTP_PARSER_EVT_REASON;
        }
        else
        {
            err = HERE_TRACKING_ERROR;
        }
    }

    if(err == HERE_TRACKING_ERROR_NEED_MORE_DATA)
    {
        (*data_size) = 0;
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_parser_parse_reason(here_tracking_http_parser* parser,
                                                                  const char* data,
                                                                  uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_NEED_MORE_DATA;

    /* Minimum 3 bytes required, 1 to N for reason phrase, 2 for CRLF */
    if((*data_size) >= strlen(here_tracking_http_crlf) + 1)
    {
        uint32_t pos = 1; /* Start from index 1 as must find one character before CRLF */

        while(pos <= (*data_size) - strlen(here_tracking_http_crlf))
        {
            if(HERE_TRACKING_HTTP_PARSER_CMP((data + pos), here_tracking_http_crlf) == 0)
            {
                here_tracking_http_parser_evt evt;

                evt.id = HERE_TRACKING_HTTP_PARSER_EVT_REASON;
                evt.data.reason.buffer = (char*)data;
                evt.data.reason.buffer_capacity = evt.data.reason.buffer_size = pos;
                (*data_size) = pos + (uint32_t)strlen(here_tracking_http_crlf);
                err = parser->cb(&evt, true, parser->cb_data) ?
                    HERE_TRACKING_ERROR_CLIENT_INTERRUPT : HERE_TRACKING_OK;
                parser->evt_state = HERE_TRACKING_HTTP_PARSER_EVT_HDR;
                break;
            }
            else
            {
                pos++;
            }
        }
    }

    if(err == HERE_TRACKING_ERROR_NEED_MORE_DATA)
    {
        (*data_size) = 0;
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_parser_hdr_end(here_tracking_http_parser* parser,
                                                             const char* data,
                                                             uint32_t* data_size)
{
    here_tracking_error err;

    (*data_size) = (uint32_t)strlen(here_tracking_http_crlf);

    if(parser->content_size == HERE_TRACKING_HTTP_PARSER_CONTENT_LENGTH_UNKNOWN)
    {
        /* Content length header wasn't found. Required for this parser. */
        err = HERE_TRACKING_ERROR;
    }
    else
    {
        err = HERE_TRACKING_OK;
        parser->evt_state = HERE_TRACKING_HTTP_PARSER_EVT_BODY;
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_http_parser_build_hdr_evt(const char* data,
                                            uint32_t data_size,
                                            here_tracking_http_parser_evt* evt)
{
    here_tracking_error err = HERE_TRACKING_ERROR;
    char* tmp;

    evt->id = HERE_TRACKING_HTTP_PARSER_EVT_HDR;

    tmp = memchr(data, ':', data_size);

    /* Must have at least one char for both key and value */
    if(tmp != NULL && tmp > data && tmp < (data + data_size - 1))
    {
        evt->data.hdr.hdr_key_size = tmp - data;
        evt->data.hdr.hdr_key = data;

        /* Move to beginning of hdr value */
        tmp++;

        /* Skip leading whitespace for header value */
        while(tmp < (data + data_size) && *tmp == ' ')
        {
            tmp++;
        }

        /* Are there meaningful characters in header value (not empty or only whitespace) */
        if(tmp < (data + data_size))
        {
            evt->data.hdr.hdr_val = tmp;
            evt->data.hdr.hdr_val_size = (data + data_size) - tmp;
            err = HERE_TRACKING_OK;
        }
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_parser_parse_hdr(here_tracking_http_parser* parser,
                                                               const char* data,
                                                               uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_NEED_MORE_DATA;

    /* At least two bytes required. Header key + colon or CRLF */
    if((*data_size) >= 2)
    {
        if(HERE_TRACKING_HTTP_PARSER_CMP(data, here_tracking_http_crlf) == 0)
        {
            err = here_tracking_http_parser_hdr_end(parser, data, data_size);
        }
        else
        {
            uint32_t pos = 1;

            while(pos <= (*data_size) - strlen(here_tracking_http_crlf))
            {
                if(HERE_TRACKING_HTTP_PARSER_CMP((data + pos), here_tracking_http_crlf) == 0)
                {
                    here_tracking_http_parser_evt evt;

                    err = here_tracking_http_parser_build_hdr_evt(data, pos, &evt);

                    if(err == HERE_TRACKING_OK)
                    {
                        here_tracking_http_parser_evt_hdr* hdr = &evt.data.hdr;

                        (*data_size) = pos + (uint32_t)strlen(here_tracking_http_crlf);

                        err = parser->cb(&evt, true, parser->cb_data) ?
                            HERE_TRACKING_ERROR_CLIENT_INTERRUPT : HERE_TRACKING_OK;

                        if(err == HERE_TRACKING_OK &&
                           hdr->hdr_key_size == strlen(here_tracking_http_header_content_length) &&
                           here_tracking_utils_memcasecmp((uint8_t*)hdr->hdr_key,
                                                (uint8_t*)here_tracking_http_header_content_length,
                                                hdr->hdr_key_size) == 0)
                        {
                            parser->content_size = atoi(hdr->hdr_val);
                            evt.id = HERE_TRACKING_HTTP_PARSER_EVT_BODY_SIZE;
                            evt.data.body_size = (uint32_t)parser->content_size;
                            err = parser->cb(&evt, true, parser->cb_data) ?
                                HERE_TRACKING_ERROR_CLIENT_INTERRUPT : HERE_TRACKING_OK;
                        }
                    }

                    break;
                }

                pos++;
            }
        }
    }

    if(err == HERE_TRACKING_ERROR_NEED_MORE_DATA)
    {
        (*data_size) = 0;
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_parser_parse_body(here_tracking_http_parser* parser,
                                                                const char* data,
                                                                uint32_t* data_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_NEED_MORE_DATA;
    here_tracking_http_parser_evt evt;

    evt.id = HERE_TRACKING_HTTP_PARSER_EVT_BODY;
    evt.data.body.buffer = (char*)data;

    if(parser->content_size <= (*data_size))
    {
        /* Last part of the body */
        (*data_size) = parser->content_size;
        parser->content_size = 0;
        err = HERE_TRACKING_OK;
    }
    else
    {
        /* Part of the body */
        parser->content_size -= (*data_size);
    }

    evt.data.body.buffer_capacity = evt.data.body.buffer_size = (*data_size);

    if(parser->cb(&evt, (parser->content_size == 0), parser->cb_data))
    {
        err = HERE_TRACKING_ERROR_CLIENT_INTERRUPT;
    }

    return err;
}
