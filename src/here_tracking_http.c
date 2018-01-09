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

#include "here_tracking_data_buffer.h"
#include "here_tracking_http.h"
#include "here_tracking_http_parser.h"
#include "here_tracking_oauth.h"
#include "here_tracking_time.h"
#include "here_tracking_utils.h"

/**************************************************************************************************/

#define HERE_TRACKING_HTTP_STATUS_OK                  200
#define HERE_TRACKING_HTTP_STATUS_BAD_REQUEST         400
#define HERE_TRACKING_HTTP_STATUS_UNAUTHORIZED        401
#define HERE_TRACKING_HTTP_STATUS_FORBIDDEN           403
#define HERE_TRACKING_HTTP_STATUS_PRECONDITION_FAILED 412

/**************************************************************************************************/

#define HERE_TRACKING_HTTP_PORT_HTTPS 443

/**************************************************************************************************/

const char* here_tracking_http_method_post = "POST";

/**************************************************************************************************/

const char* here_tracking_http_protocol_https = "https://";

/**************************************************************************************************/

const char* here_tracking_http_device_http_token        = "/token";
const char* here_tracking_http_device_http_version      = "/v2";

/**************************************************************************************************/

static const char* here_tracking_http_header_authorization =    "Authorization";
static const char* here_tracking_http_header_bearer =           "Bearer";
static const char* here_tracking_http_header_connection =       "Connection";
const char* here_tracking_http_header_content_length =          "Content-Length";
static const char* here_tracking_http_header_content_type =     "Content-Type";
static const char* here_tracking_http_header_host =             "Host";
static const char* here_tracking_http_header_x_here_timestamp = "x-here-timestamp";

/**************************************************************************************************/

static const char* here_tracking_http_connection_close = "close";
const char* here_tracking_http_crlf =                    "\r\n";
static const char* here_tracking_http_version =          "HTTP/1.1";
static const char* here_tracking_http_access_token =     "\"accessToken\"";
static const char* here_tracking_http_expires_in =       "\"expiresIn\"";
static const char* here_tracking_http_application_json = "application/json";

/**************************************************************************************************/

#define TRY(OP) if((err = (OP)) != HERE_TRACKING_OK) { goto here_tracking_http_error; }

#define HERE_TRACKING_HTTP_ADD_HEADER(BUF, KEY, VAL) \
    TRY((here_tracking_data_buffer_add_string((BUF), KEY))); \
    TRY((here_tracking_data_buffer_add_char((BUF), ':'))); \
    TRY((here_tracking_data_buffer_add_string((BUF), VAL))); \
    TRY((here_tracking_data_buffer_add_string((BUF), here_tracking_http_crlf)))

/**************************************************************************************************/

#define HERE_TRACKING_HTTP_WORK_BUF_SIZE 2048

static char here_tracking_http_work_buf[HERE_TRACKING_HTTP_WORK_BUF_SIZE];

/**************************************************************************************************/

#define HERE_TRACKING_HTTP_AUTH_SEARCH_KEY_COUNT 2

#define HERE_TRACKING_HTTP_AUTH_FIND_KEY          0
#define HERE_TRACKING_HTTP_AUTH_TOKEN_FIND_START  1
#define HERE_TRACKING_HTTP_AUTH_TOKEN_WRITE       2
#define HERE_TRACKING_HTTP_AUTH_EXPIRY_FIND_START 3
#define HERE_TRACKING_HTTP_AUTH_EXPIRY_WRITE      4

typedef struct
{
    const char* key; /**< Key to search for */
    bool found; /**< Has key been found */
    uint8_t chars; /**< Number of chars matched for the key */
    uint8_t next_state; /**< State to move to after key has been found */
} here_tracking_http_search_key;

typedef struct
{
    here_tracking_client* client;
    uint8_t state;
    uint16_t chars;
    here_tracking_error status_code;
    here_tracking_http_search_key search_keys[HERE_TRACKING_HTTP_AUTH_SEARCH_KEY_COUNT];
} here_tracking_http_auth_data;

typedef struct
{
    here_tracking_client* client;
    here_tracking_error status_code;
    uint32_t body_size;
    here_tracking_data_buffer resp_buffer;
} here_tracking_http_send_data;

/**************************************************************************************************/

static bool here_tracking_http_auth_resp_cb(const here_tracking_http_parser_evt* evt,
                                            bool last,
                                            void* cb_data);

static bool here_tracking_http_send_resp_cb(const here_tracking_http_parser_evt* evt,
                                            bool last,
                                            void* cb_data);

static here_tracking_error here_tracking_http_connect(here_tracking_client* client);

static here_tracking_error here_tracking_http_send_req(here_tracking_client* client,
                                                       const char* req,
                                                       uint32_t req_size);

static here_tracking_error here_tracking_http_recv_resp(here_tracking_client* client,
                                                        here_tracking_http_parser_evt_cb resp_cb,
                                                        void* resp_cb_data);

static void here_tracking_http_auth_data_init(here_tracking_http_auth_data* auth_data,
                                              here_tracking_client* client);

static bool here_tracking_http_auth_done(here_tracking_http_auth_data* auth_data);

static here_tracking_error here_tracking_http_status_code_to_err(uint16_t http_status_code);

/**************************************************************************************************/

here_tracking_error here_tracking_http_auth(here_tracking_client* client)
{
    here_tracking_error err = here_tracking_http_connect(client);

    if(err == HERE_TRACKING_OK)
    {
        here_tracking_data_buffer data_buf;
        uint32_t size;
        here_tracking_http_auth_data auth_data;

        TRY((here_tracking_data_buffer_init(&data_buf,
                                            here_tracking_http_work_buf,
                                            HERE_TRACKING_HTTP_WORK_BUF_SIZE)));
        TRY((here_tracking_data_buffer_add_string(&data_buf, here_tracking_http_method_post)));
        TRY((here_tracking_data_buffer_add_char(&data_buf, ' ')));
        TRY((here_tracking_data_buffer_add_string(&data_buf,
                                                  here_tracking_http_device_http_version)));
        TRY((here_tracking_data_buffer_add_string(&data_buf,
                                                  here_tracking_http_device_http_token)));
        TRY((here_tracking_data_buffer_add_char(&data_buf, ' ')));
        TRY((here_tracking_data_buffer_add_string(&data_buf, here_tracking_http_version)));
        TRY((here_tracking_data_buffer_add_string(&data_buf, here_tracking_http_crlf)));

        /* HTTP headers */
        HERE_TRACKING_HTTP_ADD_HEADER(&data_buf, here_tracking_http_header_host, client->base_url);
        HERE_TRACKING_HTTP_ADD_HEADER(&data_buf,
                                      here_tracking_http_header_connection,
                                      here_tracking_http_connection_close);
        HERE_TRACKING_HTTP_ADD_HEADER(&data_buf, here_tracking_http_header_content_length, "0");
        TRY((here_tracking_data_buffer_add_string(&data_buf,
                                                  here_tracking_http_header_authorization)));
        TRY((here_tracking_data_buffer_add_char(&data_buf, ':')));
        size = data_buf.buffer_capacity - data_buf.buffer_size;
        TRY((here_tracking_oauth_create_header(client->device_id,
                                               client->device_secret,
                                               client->base_url,
                                               client->srv_time_diff,
                                               (data_buf.buffer + data_buf.buffer_size),
                                               &size)));
        data_buf.buffer_size += size;
        /* Complete header section and send the request */
        TRY((here_tracking_data_buffer_add_string(&data_buf, here_tracking_http_crlf)));
        TRY((here_tracking_data_buffer_add_string(&data_buf, here_tracking_http_crlf)));
        TRY((here_tracking_http_send_req(client, data_buf.buffer, data_buf.buffer_size)));

        /* Finally set up response handler and read the response */
        here_tracking_http_auth_data_init(&auth_data, client);
        err = here_tracking_http_recv_resp(client,
                                           here_tracking_http_auth_resp_cb,
                                           (void*)(&auth_data));

        if(err == HERE_TRACKING_OK || err == HERE_TRACKING_ERROR_CLIENT_INTERRUPT)
        {
            err = auth_data.status_code;
        }

here_tracking_http_error:
        here_tracking_tls_close(client->tls);
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_http_send(here_tracking_client* client,
                                            char* data,
                                            uint32_t send_size,
                                            uint32_t recv_size)
{
    here_tracking_error err = here_tracking_http_connect(client);

    if(err == HERE_TRACKING_OK)
    {
        here_tracking_data_buffer header_buf;
        here_tracking_http_send_data send_data;

        TRY((here_tracking_data_buffer_init(&header_buf,
                                            here_tracking_http_work_buf,
                                            HERE_TRACKING_HTTP_WORK_BUF_SIZE)));

        /* HTTP request line */
        TRY((here_tracking_data_buffer_add_string(&header_buf, here_tracking_http_method_post)));
        TRY((here_tracking_data_buffer_add_char(&header_buf, ' ')));
        TRY((here_tracking_data_buffer_add_string(&header_buf,
                                                  here_tracking_http_device_http_version)));
        TRY((here_tracking_data_buffer_add_char(&header_buf, '/')));
        TRY((here_tracking_data_buffer_add_char(&header_buf, ' ')));
        TRY((here_tracking_data_buffer_add_string(&header_buf, here_tracking_http_version)));
        TRY((here_tracking_data_buffer_add_string(&header_buf, here_tracking_http_crlf)));

        /* HTTP headers */
        HERE_TRACKING_HTTP_ADD_HEADER(&header_buf,
                                      here_tracking_http_header_host,
                                      client->base_url);
        HERE_TRACKING_HTTP_ADD_HEADER(&header_buf,
                                      here_tracking_http_header_connection,
                                      here_tracking_http_connection_close);
        HERE_TRACKING_HTTP_ADD_HEADER(&header_buf,
                                      here_tracking_http_header_content_type,
                                      here_tracking_http_application_json);

        /* Construct authorization header */
        TRY((here_tracking_data_buffer_add_string(&header_buf,
                                                  here_tracking_http_header_authorization)));
        TRY((here_tracking_data_buffer_add_char(&header_buf, ':')));
        TRY((here_tracking_data_buffer_add_string(&header_buf, here_tracking_http_header_bearer)));
        TRY((here_tracking_data_buffer_add_char(&header_buf, ' ')));
        TRY((here_tracking_data_buffer_add_string(&header_buf, client->access_token)));
        TRY((here_tracking_data_buffer_add_string(&header_buf, here_tracking_http_crlf)));

        /* Construct content length header */
        TRY((here_tracking_data_buffer_add_string(&header_buf,
                                                  here_tracking_http_header_content_length)));
        TRY((here_tracking_data_buffer_add_char(&header_buf, ':')));
        TRY((here_tracking_data_buffer_add_utoa(&header_buf, send_size)));
        TRY((here_tracking_data_buffer_add_string(&header_buf, here_tracking_http_crlf)));
        TRY((here_tracking_data_buffer_add_string(&header_buf, here_tracking_http_crlf)));

        /* Send request line + headers */
        TRY((here_tracking_http_send_req(client, header_buf.buffer, header_buf.buffer_size)));

        /* Send request body */
        TRY((here_tracking_http_send_req(client, data, send_size)));

        /* Finally set up response handler and read the response */
        send_data.client = client;
        send_data.status_code = HERE_TRACKING_ERROR;
        send_data.body_size = 0;
        TRY((here_tracking_data_buffer_init(&send_data.resp_buffer, data, recv_size)));
        err = here_tracking_http_recv_resp(client, here_tracking_http_send_resp_cb, &send_data);

        if(err == HERE_TRACKING_ERROR_CLIENT_INTERRUPT)
        {
            err = HERE_TRACKING_OK;
        }

        if(client->data_cb != NULL)
        {
            if(send_data.status_code == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL)
            {
                client->data_cb(HERE_TRACKING_ERROR_BUFFER_TOO_SMALL,
                                send_data.resp_buffer.buffer,
                                send_data.body_size,
                                client->data_cb_user_data);
            }
            else
            {
                client->data_cb(send_data.status_code,
                                send_data.resp_buffer.buffer,
                                send_data.resp_buffer.buffer_size,
                                client->data_cb_user_data);
            }
        }

here_tracking_http_error:
        here_tracking_tls_close(client->tls);
    }

    return err;
}

/**************************************************************************************************/

static bool here_tracking_http_auth_resp_cb(const here_tracking_http_parser_evt* evt,
                                            bool last,
                                            void* cb_data)
{
    here_tracking_http_auth_data* auth_data = (here_tracking_http_auth_data*)cb_data;
    bool res = false;

    switch(evt->id)
    {
        case HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE:
        {
            auth_data->status_code = here_tracking_http_status_code_to_err(evt->data.status_code);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_HDR:
        {
            if(auth_data->status_code == HERE_TRACKING_ERROR_UNAUTHORIZED)
            {
                const here_tracking_http_parser_evt_hdr* hdr_data = &(evt->data.hdr);

                if(hdr_data->hdr_key_size == strlen(here_tracking_http_header_x_here_timestamp) &&
                   here_tracking_utils_memcasecmp((const uint8_t*)hdr_data->hdr_key,
                                        (const uint8_t*)here_tracking_http_header_x_here_timestamp,
                                        hdr_data->hdr_key_size) == 0)
                {
                    uint32_t srv_time, pl_time;

                    if(here_tracking_get_unixtime(&pl_time) == HERE_TRACKING_OK)
                    {
                        srv_time = atoi(hdr_data->hdr_val);
                        auth_data->client->srv_time_diff = srv_time - pl_time;
                        auth_data->status_code = HERE_TRACKING_ERROR_TIME_MISMATCH;
                        res = true;
                    }
                }
            }
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
        {
            if(auth_data->status_code == HERE_TRACKING_OK)
            {
                uint32_t pos = 0, data_size = evt->data.body.buffer_size;
                const char* data = evt->data.body.buffer;

                while(data_size - pos > 0)
                {
                    /*
                     * Auth data states:
                     *
                     *           key_found                          key_handled          yes
                     * FIND_KEY -----------> KEY SPECIFIC STATE(S) -------------> DONE? ----> EXIT
                     *    ^                                                         |
                     *    |                                                         | no
                     *    -----------------------------------------------------------
                     */
                    if(auth_data->state == HERE_TRACKING_HTTP_AUTH_FIND_KEY)
                    {
                        uint8_t i;

                        for(i = 0; i < HERE_TRACKING_HTTP_AUTH_SEARCH_KEY_COUNT; ++i)
                        {
                            here_tracking_http_search_key* key = (auth_data->search_keys + i);

                            if(!key->found)
                            {
                                if(data[pos] == key->key[key->chars])
                                {
                                    /* Char matches key, increment count */
                                    key->chars++;
                                }
                                else
                                {
                                    /* Not a match, reset char count */
                                    key->chars = 0;
                                    continue;
                                }

                                if(key->chars == strlen(key->key))
                                {
                                    /* Matched the key, mark as found and move to key handling
                                       state. */
                                    key->found = true;
                                    auth_data->state = key->next_state;
                                }
                            }
                        }
                    }
                    else if(auth_data->state == HERE_TRACKING_HTTP_AUTH_TOKEN_FIND_START)
                    {
                        if(data[pos] == '\"')
                        {
                            auth_data->state = HERE_TRACKING_HTTP_AUTH_TOKEN_WRITE;
                        }
                    }
                    else if(auth_data->state == HERE_TRACKING_HTTP_AUTH_TOKEN_WRITE)
                    {
                        if(data[pos] == '\"')
                        {
                            auth_data->client->access_token[auth_data->chars] = '\0';

                            if(here_tracking_http_auth_done(auth_data))
                            {
                                res = true;
                                break;
                            }
                            else
                            {
                                auth_data->state = HERE_TRACKING_HTTP_AUTH_FIND_KEY;
                            }
                        }
                        else if(auth_data->chars < HERE_TRACKING_ACCESS_TOKEN_SIZE - 1)
                        {
                            auth_data->client->access_token[auth_data->chars] = data[pos];
                        }
                        else
                        {
                            auth_data->client->access_token[0] = '\0';
                            auth_data->status_code = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
                            res = true;
                            break;
                        }

                        auth_data->chars++;
                    }
                    else if(auth_data->state == HERE_TRACKING_HTTP_AUTH_EXPIRY_FIND_START)
                    {
                        if(here_tracking_utils_isdigit(data[pos]))
                        {
                            auth_data->state = HERE_TRACKING_HTTP_AUTH_EXPIRY_WRITE;
                            continue;
                        }
                    }
                    else if(auth_data->state == HERE_TRACKING_HTTP_AUTH_EXPIRY_WRITE)
                    {
                        if(!here_tracking_utils_isdigit(data[pos]))
                        {
                            uint32_t ts;

                            here_tracking_get_unixtime(&ts);
                            auth_data->client->token_expiry += ts;

                            if(here_tracking_http_auth_done(auth_data))
                            {
                                res = true;
                                break;
                            }
                            else
                            {
                                auth_data->state = HERE_TRACKING_HTTP_AUTH_FIND_KEY;
                            }
                        }
                        else
                        {
                            auth_data->client->token_expiry *= 10;
                            auth_data->client->token_expiry += (data[pos] - '0');
                        }
                    }

                    pos++;
                }
            }
        }
        break;

        default:
            break;
    }

    return res;
}

/**************************************************************************************************/

static bool here_tracking_http_send_resp_cb(const here_tracking_http_parser_evt* evt,
                                            bool last,
                                            void* cb_data)
{
    here_tracking_http_send_data* send_data = (here_tracking_http_send_data*)cb_data;
    bool res = false;

    switch(evt->id)
    {
        case HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE:
        {
            send_data->status_code = here_tracking_http_status_code_to_err(evt->data.status_code);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
        {
            if(here_tracking_data_buffer_add_data(&send_data->resp_buffer,
                                                  evt->data.body.buffer,
                                                  evt->data.body.buffer_size) != HERE_TRACKING_OK)
            {
                here_tracking_data_buffer_add_data(&send_data->resp_buffer,
                                                   evt->data.body.buffer,
                                                   (send_data->resp_buffer.buffer_capacity -
                                                    send_data->resp_buffer.buffer_size));
                send_data->status_code = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
                res = true;
            }
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY_SIZE:
        {
            send_data->body_size = evt->data.body_size;
        }
        break;

        default:
            break;
    }

    return res;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_connect(here_tracking_client* client)
{
    here_tracking_error err = HERE_TRACKING_OK;

    if(client->tls == NULL)
    {
        err = here_tracking_tls_init(&(client->tls));
    }

    if(err == HERE_TRACKING_OK)
    {
        err = here_tracking_tls_connect(client->tls,
                                        client->base_url,
                                        HERE_TRACKING_HTTP_PORT_HTTPS);
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_send_req(here_tracking_client* client,
                                                       const char* req,
                                                       uint32_t req_size)
{
    here_tracking_error err = HERE_TRACKING_OK;
    uint32_t to_send = req_size;

    while(err == HERE_TRACKING_OK && to_send > 0)
    {
        err = here_tracking_tls_write(client->tls, (req + (req_size - to_send)), &to_send);

        if(err == HERE_TRACKING_OK)
        {
            to_send = req_size - to_send;
        }
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_http_recv_resp(here_tracking_client* client,
                                 here_tracking_http_parser_evt_cb resp_cb,
                                 void* resp_cb_data)
{
    here_tracking_error err = HERE_TRACKING_OK;
    uint32_t size = HERE_TRACKING_HTTP_WORK_BUF_SIZE, parse_size, pos = 0;
    here_tracking_http_parser parser;

    TRY((here_tracking_tls_read(client->tls, here_tracking_http_work_buf, &size)));
    here_tracking_http_parser_init(&parser, resp_cb, resp_cb_data);
    parse_size = size;
    err = here_tracking_http_parser_parse(&parser, here_tracking_http_work_buf, &parse_size);

    while(err == HERE_TRACKING_ERROR_NEED_MORE_DATA && size < HERE_TRACKING_HTTP_WORK_BUF_SIZE)
    {
        /* Move data that hasn't been parsed to the beginning of the work buffer */
        memmove(here_tracking_http_work_buf,
                (here_tracking_http_work_buf + parse_size),
                size - parse_size);

        pos = size - parse_size; /* Free space starts in this position */
        size = HERE_TRACKING_HTTP_WORK_BUF_SIZE - pos; /* Free space available */

        /* Read more data to the free space in work buffer */
        TRY((here_tracking_tls_read(client->tls, here_tracking_http_work_buf + pos, &size)));

        size = parse_size = pos + size; /* Size of unparsed data in the work buffer */

        err = here_tracking_http_parser_parse(&parser, here_tracking_http_work_buf, &parse_size);
    }

    /* Parser requires more data to continue but work buffer is already full. */
    if(err == HERE_TRACKING_ERROR_NEED_MORE_DATA)
    {
        err = HERE_TRACKING_ERROR;
    }

here_tracking_http_error:
    return err;
}

/**************************************************************************************************/

static void here_tracking_http_auth_data_init(here_tracking_http_auth_data* auth_data,
                                              here_tracking_client* client)
{
    auth_data->client = client;
    auth_data->state = HERE_TRACKING_HTTP_AUTH_FIND_KEY;
    auth_data->chars = 0;
    auth_data->status_code = HERE_TRACKING_ERROR;
    auth_data->search_keys[0].key = here_tracking_http_access_token;
    auth_data->search_keys[0].found = false;
    auth_data->search_keys[0].chars = 0;
    auth_data->search_keys[0].next_state = HERE_TRACKING_HTTP_AUTH_TOKEN_FIND_START;
    auth_data->search_keys[1].key = here_tracking_http_expires_in;
    auth_data->search_keys[1].found = false;
    auth_data->search_keys[1].chars = 0;
    auth_data->search_keys[1].next_state = HERE_TRACKING_HTTP_AUTH_EXPIRY_FIND_START;
}

/**************************************************************************************************/

static bool here_tracking_http_auth_done(here_tracking_http_auth_data* auth_data)
{
    uint8_t i;
    bool all_found = true;

    /* Done if all search keys have been found. */
    for(i = 0; i < HERE_TRACKING_HTTP_AUTH_SEARCH_KEY_COUNT; ++i)
    {
        if(!auth_data->search_keys[i].found)
        {
            auth_data->search_keys[i].chars = 0;
            all_found = false;
        }
    }

    return all_found;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_status_code_to_err(uint16_t http_status_code)
{
    here_tracking_error err;

    switch(http_status_code)
    {
        case HERE_TRACKING_HTTP_STATUS_OK:
        {
            err = HERE_TRACKING_OK;
        }
        break;

        case HERE_TRACKING_HTTP_STATUS_BAD_REQUEST:
        {
            err = HERE_TRACKING_ERROR_BAD_REQUEST;
        }
        break;

        case HERE_TRACKING_HTTP_STATUS_UNAUTHORIZED:
        {
            err = HERE_TRACKING_ERROR_UNAUTHORIZED;
        }
        break;

        case HERE_TRACKING_HTTP_STATUS_FORBIDDEN:
        {
            err = HERE_TRACKING_ERROR_FORBIDDEN;
        }
        break;

        case HERE_TRACKING_HTTP_STATUS_PRECONDITION_FAILED:
        {
            err = HERE_TRACKING_ERROR_DEVICE_UNCLAIMED;
        }
        break;

        default:
        {
            err = HERE_TRACKING_ERROR;
        }
        break;
    }

    return err;
}
