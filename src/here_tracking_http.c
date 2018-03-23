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

#include <stdlib.h>
#include <string.h>

#include "here_tracking_data_buffer.h"
#include "here_tracking_http.h"
#include "here_tracking_http_defs.h"
#include "here_tracking_http_parser.h"
#include "here_tracking_oauth.h"
#include "here_tracking_time.h"
#include "here_tracking_tls_writer.h"
#include "here_tracking_utils.h"

/**************************************************************************************************/

#define HERE_TRACKING_HTTP_PORT_HTTPS 443

/**************************************************************************************************/

static const char* here_tracking_http_query_async = "?async=true";

/**************************************************************************************************/

static const char* here_tracking_http_header_bearer =           "Bearer";
static const char* here_tracking_http_header_content_type =     "Content-Type";
static const char* here_tracking_http_header_host =             "Host";
static const char* here_tracking_http_header_x_here_timestamp = "x-here-timestamp";

/**************************************************************************************************/

static const char* here_tracking_http_version =          "HTTP/1.1";
static const char* here_tracking_http_access_token =     "\"accessToken\"";
static const char* here_tracking_http_expires_in =       "\"expiresIn\"";
static const char* here_tracking_http_application_json = "application/json";

/**************************************************************************************************/

#define TRY(OP) if((err = (OP)) != HERE_TRACKING_OK) { goto here_tracking_http_error; }

#define HERE_TRACKING_HTTP_WRITE_HEADER(WRITER, KEY, VAL) \
    TRY((here_tracking_tls_writer_write_string((WRITER), KEY))); \
    TRY((here_tracking_tls_writer_write_char((WRITER), ':'))); \
    TRY((here_tracking_tls_writer_write_string((WRITER), VAL))); \
    TRY((here_tracking_tls_writer_write_string((WRITER), here_tracking_http_crlf)))

/**************************************************************************************************/

#define HERE_TRACKING_HTTP_TLS_BUFFER_SIZE 256

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
    uint8_t* send_data;
    size_t send_data_size;
    size_t body_size;
    here_tracking_data_buffer resp_buffer;
    here_tracking_error status_code;
} here_tracking_http_send_recv_ctx;

typedef struct
{
    here_tracking_client* client;
    here_tracking_error status_code;
    here_tracking_recv_cb recv_cb;
    void* user_data;
} here_tracking_http_recv_ctx;

/**************************************************************************************************/

static bool here_tracking_http_auth_resp_cb(const here_tracking_http_parser_evt* evt,
                                            bool last,
                                            void* cb_data);

static bool here_tracking_http_send_resp_cb(const here_tracking_http_parser_evt* evt,
                                            bool last,
                                            void* cb_data);

static here_tracking_error here_tracking_http_connect(here_tracking_client* client,
                                                      const char* host,
                                                      uint16_t port);

static here_tracking_error here_tracking_http_recv_resp(here_tracking_client* client,
                                                        uint8_t* recv_buffer,
                                                        size_t recv_buffer_size,
                                                        here_tracking_http_parser_evt_cb resp_cb,
                                                        void* resp_cb_data);

static void here_tracking_http_auth_data_init(here_tracking_http_auth_data* auth_data,
                                              here_tracking_client* client);

static bool here_tracking_http_auth_done(here_tracking_http_auth_data* auth_data);

static here_tracking_error here_tracking_http_status_code_to_err(uint16_t http_status_code);

static here_tracking_error here_tracking_http_send_chunk(here_tracking_tls_writer* tls_writer,
                                                         const uint8_t* chunk_data,
                                                         size_t chunk_size);

static here_tracking_error here_tracking_http_send_cb(uint8_t** data,
                                                      size_t* data_size,
                                                      void* user_data);

static here_tracking_error here_tracking_http_recv_cb(const here_tracking_recv_data* data,
                                                      void* user_data);

static here_tracking_error \
    here_tracking_http_get_write_auth_header(here_tracking_tls_writer* tls_writer,
                                             const here_tracking_http_header* auth_header);

/**************************************************************************************************/

here_tracking_error here_tracking_http_auth(here_tracking_client* client)
{
    here_tracking_error err = here_tracking_http_connect(client,
                                                         client->base_url,
                                                         HERE_TRACKING_HTTP_PORT_HTTPS);

    if(err == HERE_TRACKING_OK)
    {
        here_tracking_tls_writer tls_writer;
        uint8_t tls_buffer[HERE_TRACKING_HTTP_TLS_BUFFER_SIZE];
        uint8_t oauth_buffer[HERE_TRACKING_OAUTH_MIN_OUT_SIZE];
        uint32_t oauth_size = HERE_TRACKING_OAUTH_MIN_OUT_SIZE;
        here_tracking_http_auth_data auth_data;

        TRY((here_tracking_tls_writer_init(&tls_writer,
                                           client->tls,
                                           tls_buffer,
                                           HERE_TRACKING_HTTP_TLS_BUFFER_SIZE)));

        /* HTTP request line */
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_method_post)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ' ')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer,
                                                   here_tracking_http_path_version)));
        TRY((here_tracking_tls_writer_write_string(&tls_writer,
                                                   here_tracking_http_path_token)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ' ')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_version)));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* Host header */
        HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer,
                                        here_tracking_http_header_host,
                                        client->base_url);

        /* Connection header */
        HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer,
                                        here_tracking_http_header_connection,
                                        here_tracking_http_connection_close);

        /* Content length header */
        HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer, here_tracking_http_header_content_length, "0");

        /* Authorization header */
        TRY((here_tracking_tls_writer_write_string(&tls_writer,
                                                   here_tracking_http_header_authorization)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ':')));
        TRY((here_tracking_oauth_create_header(client->device_id,
                                               client->device_secret,
                                               client->base_url,
                                               client->srv_time_diff,
                                               (char*)oauth_buffer,
                                               &oauth_size)));
        TRY((here_tracking_tls_writer_write_data(&tls_writer, oauth_buffer, oauth_size)));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* Complete header section */
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* Flush remaining data */
        TRY((here_tracking_tls_writer_flush(&tls_writer)));

        /* Finally set up response handler and read the response */
        here_tracking_http_auth_data_init(&auth_data, client);
        err = here_tracking_http_recv_resp(client,
                                           tls_buffer,
                                           HERE_TRACKING_HTTP_TLS_BUFFER_SIZE,
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
    here_tracking_error err;
    here_tracking_http_send_recv_ctx send_recv_ctx;

    send_recv_ctx.body_size = 0;
    send_recv_ctx.send_data = (uint8_t*)data;
    send_recv_ctx.send_data_size = send_size;
    send_recv_ctx.status_code = HERE_TRACKING_OK;
    TRY((here_tracking_data_buffer_init(&send_recv_ctx.resp_buffer, data, recv_size)));
    err = here_tracking_http_send_stream(client,
                                   here_tracking_http_send_cb,
                                   here_tracking_http_recv_cb,
                                   HERE_TRACKING_RESP_WITH_DATA,
                                   &send_recv_ctx);

    if(client->data_cb != NULL)
    {
        if(send_recv_ctx.status_code == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL)
        {
            client->data_cb(HERE_TRACKING_ERROR_BUFFER_TOO_SMALL,
                            send_recv_ctx.resp_buffer.buffer,
                            send_recv_ctx.body_size,
                            client->data_cb_user_data);
        }
        else
        {
            client->data_cb(send_recv_ctx.status_code,
                            send_recv_ctx.resp_buffer.buffer,
                            send_recv_ctx.resp_buffer.buffer_size,
                            client->data_cb_user_data);
        }
    }

here_tracking_http_error:
    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_http_send_stream(here_tracking_client* client,
                                             here_tracking_send_cb send_cb,
                                             here_tracking_recv_cb recv_cb,
                                             here_tracking_resp_type resp_type,
                                             void* user_data)
{
    here_tracking_error err = here_tracking_http_connect(client,
                                                         client->base_url,
                                                         HERE_TRACKING_HTTP_PORT_HTTPS);

    if(err == HERE_TRACKING_OK)
    {
        here_tracking_tls_writer tls_writer;
        uint8_t tls_buffer[HERE_TRACKING_HTTP_TLS_BUFFER_SIZE];
        here_tracking_http_recv_ctx recv_ctx;
        uint8_t* data;
        size_t data_size;

        TRY((here_tracking_tls_writer_init(&tls_writer,
                                           client->tls,
                                           tls_buffer,
                                           HERE_TRACKING_HTTP_TLS_BUFFER_SIZE)));

        /* HTTP request line */
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_method_post)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ' ')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer,
                                                   here_tracking_http_path_version)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, '/')));

        if(resp_type == HERE_TRACKING_RESP_STATUS_ONLY)
        {
            TRY((here_tracking_tls_writer_write_string(&tls_writer,
                                                       here_tracking_http_query_async)));
        }

        TRY((here_tracking_tls_writer_write_char(&tls_writer, ' ')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_version)));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* HTTP headers */
        HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer,
                                        here_tracking_http_header_host,
                                        client->base_url);
        HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer,
                                        here_tracking_http_header_connection,
                                        here_tracking_http_connection_close);
        HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer,
                                        here_tracking_http_header_content_type,
                                        here_tracking_http_application_json);
        HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer,
                                        here_tracking_http_header_transfer_encoding,
                                        here_tracking_http_transfer_encoding_chunked);

        /* Construct authorization header */
        TRY((here_tracking_tls_writer_write_string(&tls_writer,
                                                   here_tracking_http_header_authorization)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ':')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_header_bearer)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ' ')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, client->access_token)));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* Complete header section */
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* Read and send data chunks from io context */
        do
        {
            TRY((send_cb(&data, &data_size, user_data)));
            TRY((here_tracking_http_send_chunk(&tls_writer, data, data_size)));
        } while(data != NULL && data_size > 0);

        /* Flush remaining data */
        TRY((here_tracking_tls_writer_flush(&tls_writer)));

        /* Finally set up response handler and read the response */
        recv_ctx.client = client;
        recv_ctx.status_code = HERE_TRACKING_ERROR;
        recv_ctx.recv_cb = recv_cb;
        recv_ctx.user_data = user_data;

        err = here_tracking_http_recv_resp(client,
                                           tls_buffer,
                                           HERE_TRACKING_HTTP_TLS_BUFFER_SIZE,
                                           here_tracking_http_send_resp_cb,
                                           &recv_ctx);

        if(err == HERE_TRACKING_ERROR_CLIENT_INTERRUPT)
        {
            err = HERE_TRACKING_OK;
        }

        if(recv_ctx.status_code == HERE_TRACKING_ERROR_UNAUTHORIZED ||
           recv_ctx.status_code == HERE_TRACKING_ERROR_FORBIDDEN)
        {
            client->access_token[0] = '\0';
            client->token_expiry = 0;
        }

here_tracking_http_error:
        here_tracking_tls_close(client->tls);
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_http_get(here_tracking_client* client,
                                           const here_tracking_http_request* request,
                                           here_tracking_recv_cb recv_cb,
                                           void* user_data)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(client != NULL &&
       request != NULL &&
       request->host != NULL &&
       request->path != NULL &&
       recv_cb != NULL)
    {
        err = here_tracking_http_connect(client, request->host, request->port);
    }

    if(err == HERE_TRACKING_OK)
    {
        here_tracking_tls_writer tls_writer;
        uint8_t tls_buffer[HERE_TRACKING_HTTP_TLS_BUFFER_SIZE];
        here_tracking_http_recv_ctx recv_ctx;
        uint8_t i;

        TRY((here_tracking_tls_writer_init(&tls_writer,
                                           client->tls,
                                           tls_buffer,
                                           HERE_TRACKING_HTTP_TLS_BUFFER_SIZE)));

        /* HTTP request line */
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_method_get)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ' ')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, request->path)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ' ')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_version)));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* HTTP headers */

        /* Construct host header */
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_header_host)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ':')));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, request->host)));
        TRY((here_tracking_tls_writer_write_char(&tls_writer, ':')));
        TRY((here_tracking_tls_writer_write_utoa(&tls_writer, request->port, 10)));
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* User defined headers */
        for(i = 0; i < request->header_count; ++i)
        {
            here_tracking_http_header* header = (request->headers + i);

            /* Special handling for authorization header */
            if(strcmp(header->name, here_tracking_http_header_authorization) == 0)
            {
                here_tracking_http_get_write_auth_header(&tls_writer, header);
            }
            else
            {
                HERE_TRACKING_HTTP_WRITE_HEADER(&tls_writer,
                                                request->headers[i].name,
                                                request->headers[i].value);
            }
        }

        /* Complete header section */
        TRY((here_tracking_tls_writer_write_string(&tls_writer, here_tracking_http_crlf)));

        /* Flush remaining data */
        TRY((here_tracking_tls_writer_flush(&tls_writer)));

        /* Finally set up response handler and read the response */
        recv_ctx.client = client;
        recv_ctx.status_code = HERE_TRACKING_ERROR;
        recv_ctx.recv_cb = recv_cb;
        recv_ctx.user_data = user_data;

        err = here_tracking_http_recv_resp(client,
                                           tls_buffer,
                                           HERE_TRACKING_HTTP_TLS_BUFFER_SIZE,
                                           here_tracking_http_send_resp_cb,
                                           &recv_ctx);

        if(err == HERE_TRACKING_ERROR_CLIENT_INTERRUPT)
        {
            err = HERE_TRACKING_OK;
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
    here_tracking_http_recv_ctx* recv_ctx = (here_tracking_http_recv_ctx*)cb_data;
    bool res = false;

    switch(evt->id)
    {
        case HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE:
        {
            recv_ctx->status_code = here_tracking_http_status_code_to_err(evt->data.status_code);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
        {
            here_tracking_recv_data data;

            data.err = HERE_TRACKING_OK;
            data.evt = HERE_TRACKING_RECV_EVT_RESP_DATA;
            data.data = (uint8_t*)evt->data.body.buffer;
            data.data_size = evt->data.body.buffer_size;

            if(recv_ctx->recv_cb(&data, recv_ctx->user_data) != HERE_TRACKING_OK)
            {
                res = true;
                break;
            }

            if(last)
            {
                data.err = recv_ctx->status_code;
                data.evt = HERE_TRACKING_RECV_EVT_RESP_COMPLETE;
                data.data = NULL;
                data.data_size = 0;
                recv_ctx->recv_cb(&data, recv_ctx->user_data);
            }
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY_SIZE:
        {
            here_tracking_recv_data data;

            data.err = HERE_TRACKING_OK;
            data.evt = HERE_TRACKING_RECV_EVT_RESP_SIZE;
            data.data = NULL;
            data.data_size = evt->data.body_size;
            recv_ctx->recv_cb(&data, recv_ctx->user_data);

            if(evt->data.body_size == 0)
            {
                data.err = recv_ctx->status_code;
                data.evt = HERE_TRACKING_RECV_EVT_RESP_COMPLETE;
                data.data = NULL;
                data.data_size = 0;
                recv_ctx->recv_cb(&data, recv_ctx->user_data);
            }
        }
        break;

        default:
            break;
    }

    return res;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_connect(here_tracking_client* client,
                                                      const char* host,
                                                      uint16_t port)
{
    here_tracking_error err = HERE_TRACKING_OK;

    if(client->tls == NULL)
    {
        err = here_tracking_tls_init(&(client->tls));
    }

    if(err == HERE_TRACKING_OK)
    {
        err = here_tracking_tls_connect(client->tls, host, port);
    }

    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_recv_resp(here_tracking_client* client,
                                                        uint8_t* recv_buffer,
                                                        size_t recv_buffer_size,
                                                        here_tracking_http_parser_evt_cb resp_cb,
                                                        void* resp_cb_data)
{
    here_tracking_error err = HERE_TRACKING_OK;
    uint32_t size = (uint32_t)recv_buffer_size, parse_size, pos = 0;
    here_tracking_http_parser parser;

    TRY((here_tracking_tls_read(client->tls, (char*)recv_buffer, &size)));
    here_tracking_http_parser_init(&parser, resp_cb, resp_cb_data);
    parse_size = size;
    err = here_tracking_http_parser_parse(&parser, (char*)recv_buffer, &parse_size);

    while(err == HERE_TRACKING_ERROR_NEED_MORE_DATA && (size < recv_buffer_size || parse_size > 0))
    {
        /* Move data that hasn't been parsed to the beginning of the work buffer */
        memmove(recv_buffer,
                (recv_buffer + parse_size),
                size - parse_size);

        pos = size - parse_size; /* Free space starts in this position */
        size = recv_buffer_size - pos; /* Free space available */

        /* Read more data to the free space in work buffer */
        TRY((here_tracking_tls_read(client->tls, ((char*)recv_buffer) + pos, &size)));

        size = parse_size = pos + size; /* Size of unparsed data in the work buffer */

        err = here_tracking_http_parser_parse(&parser, ((char*)recv_buffer), &parse_size);
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
            /* fall through */
        case HERE_TRACKING_HTTP_STATUS_NO_CONTENT:
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

        case HERE_TRACKING_HTTP_STATUS_NOT_FOUND:
        {
            err = HERE_TRACKING_ERROR_NOT_FOUND;
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

/**************************************************************************************************/

static here_tracking_error here_tracking_http_send_chunk(here_tracking_tls_writer* tls_writer,
                                                         const uint8_t* chunk_data,
                                                         size_t chunk_size)
{
    here_tracking_error err = HERE_TRACKING_OK;

    TRY((here_tracking_tls_writer_write_utoa(tls_writer, (uint32_t)chunk_size, 16)));
    TRY((here_tracking_tls_writer_write_string(tls_writer, here_tracking_http_crlf)));

    if(chunk_data != NULL && chunk_size > 0)
    {
        TRY((here_tracking_tls_writer_write_data(tls_writer, chunk_data, chunk_size)));
    }

    TRY((here_tracking_tls_writer_write_string(tls_writer, here_tracking_http_crlf)));

here_tracking_http_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_send_cb(uint8_t** data,
                                                      size_t* data_size,
                                                      void* user_data)
{
    here_tracking_http_send_recv_ctx* send_recv_ctx = (here_tracking_http_send_recv_ctx*)user_data;

    if(send_recv_ctx->send_data != NULL && send_recv_ctx->send_data_size > 0)
    {
        *data = send_recv_ctx->send_data;
        *data_size = send_recv_ctx->send_data_size;
        send_recv_ctx->send_data = NULL;
        send_recv_ctx->send_data_size = 0;
    }
    else
    {
        *data = NULL;
        *data_size = 0;
    }

    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_http_recv_cb(const here_tracking_recv_data* data,
                                                      void* user_data)
{
    here_tracking_http_send_recv_ctx* send_recv_ctx = (here_tracking_http_send_recv_ctx*)user_data;

    switch(data->evt)
    {
        case HERE_TRACKING_RECV_EVT_RESP_SIZE:
        {
            send_recv_ctx->body_size = data->data_size;
        }
        break;

        case HERE_TRACKING_RECV_EVT_RESP_DATA:
        {
            if(here_tracking_data_buffer_add_data(&send_recv_ctx->resp_buffer,
                                                  (const char*)data->data,
                                                  (uint32_t)data->data_size) != HERE_TRACKING_OK)
            {
                here_tracking_data_buffer_add_data(&send_recv_ctx->resp_buffer,
                                                   (const char*)data->data,
                                                   (send_recv_ctx->resp_buffer.buffer_capacity -
                                                    send_recv_ctx->resp_buffer.buffer_size));
                send_recv_ctx->status_code = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
            }
        }
        break;

        case HERE_TRACKING_RECV_EVT_RESP_COMPLETE:
        {
            send_recv_ctx->status_code = data->err;
        }
        break;

        default:
            send_recv_ctx->status_code = HERE_TRACKING_ERROR;
            break;
    }

    return send_recv_ctx->status_code;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_http_get_write_auth_header(here_tracking_tls_writer* tls_writer,
                                             const here_tracking_http_header* auth_header)
{
    here_tracking_error err;

    TRY((here_tracking_tls_writer_write_string(tls_writer,
                                               here_tracking_http_header_authorization)));
    TRY((here_tracking_tls_writer_write_char(tls_writer, ':')));

    /* Make sure that 'Bearer' token type is set */
    if(strncmp(auth_header->value,
               here_tracking_http_header_bearer,
               strlen(here_tracking_http_header_bearer)) != 0)
    {
        TRY((here_tracking_tls_writer_write_string(tls_writer, here_tracking_http_header_bearer)));
        TRY((here_tracking_tls_writer_write_char(tls_writer, ' ')));
    }

    TRY((here_tracking_tls_writer_write_string(tls_writer, auth_header->value)));
    TRY((here_tracking_tls_writer_write_string(tls_writer, here_tracking_http_crlf)));

here_tracking_http_error:
    return err;
}
