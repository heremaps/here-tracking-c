/**************************************************************************************************
 * Copyright (C) 2017-2019 HERE Europe B.V.                                                       *
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

#include <stdio.h>
#include <string.h>

#include "mock_here_tracking_http.h"

/**************************************************************************************************/

static const char* mock_here_tracking_http_auth_result_token = NULL;

static const char* mock_here_tracking_http_send_result_data = NULL;

static uint32_t mock_here_tracking_http_send_result_data_size = 0;

static here_tracking_error mock_here_tracking_http_recv_cb_status = HERE_TRACKING_OK;

/**************************************************************************************************/

DEFINE_FAKE_VALUE_FUNC1(here_tracking_error, here_tracking_http_auth, here_tracking_client*);

DEFINE_FAKE_VALUE_FUNC4(here_tracking_error,
                        here_tracking_http_send,
                        here_tracking_client*,
                        char*,
                        uint32_t,
                        uint32_t);

DEFINE_FAKE_VALUE_FUNC6(here_tracking_error,
                        here_tracking_http_send_stream,
                        here_tracking_client*,
                        here_tracking_send_cb,
                        here_tracking_recv_cb,
                        here_tracking_req_type,
                        here_tracking_resp_type,
                        void*);

/**************************************************************************************************/

void mock_here_tracking_http_auth_set_result_token(const char* token)
{
    mock_here_tracking_http_auth_result_token = token;
}

/**************************************************************************************************/

here_tracking_error mock_here_tracking_http_auth_custom(here_tracking_client* client)
{
    here_tracking_error err;
    here_tracking_http_auth_Fake* the_fake = &here_tracking_http_auth_fake;

    if(the_fake->return_val_seq_len > 0)
    {
        if(the_fake->return_val_seq_idx < the_fake->return_val_seq_len)
        {
            err = the_fake->return_val_seq[the_fake->return_val_seq_idx++];
        }
        else
        {
            err = the_fake->return_val_seq[the_fake->return_val_seq_len - 1];
        }
    }
    else
    {
        err = the_fake->return_val;
    }

    if(err == HERE_TRACKING_OK)
    {
        if(mock_here_tracking_http_auth_result_token != NULL)
        {
            strcpy(client->access_token, mock_here_tracking_http_auth_result_token);
        }
    }

    return err;
}

/**************************************************************************************************/

void mock_here_tracking_http_send_set_result_data(const char* data, uint32_t data_size)
{
    mock_here_tracking_http_send_result_data = data;
    mock_here_tracking_http_send_result_data_size = data_size;
}

/**************************************************************************************************/

void mock_here_tracking_http_set_recv_cb_status(here_tracking_error status)
{
    mock_here_tracking_http_recv_cb_status = status;
}

/**************************************************************************************************/

here_tracking_error mock_here_tracking_http_send_custom(here_tracking_client* client,
                                                        char* data,
                                                        uint32_t send_size,
                                                        uint32_t recv_size)
{
    if(here_tracking_http_send_fake.return_val == HERE_TRACKING_OK)
    {
        if(client->data_cb != NULL)
        {
            if(mock_here_tracking_http_send_result_data != NULL &&
               mock_here_tracking_http_send_result_data_size > 0)
            {
                memcpy(data,
                       mock_here_tracking_http_send_result_data,
                       mock_here_tracking_http_send_result_data_size);

                client->data_cb(HERE_TRACKING_OK,
                                data,
                                mock_here_tracking_http_send_result_data_size,
                                client->data_cb_user_data);
            }
        }
    }

    return here_tracking_http_send_fake.return_val;
}

/**************************************************************************************************/

here_tracking_error mock_here_tracking_http_send_stream_custom(here_tracking_client* client,
                                                               here_tracking_send_cb send_cb,
                                                               here_tracking_recv_cb recv_cb,
                                                               here_tracking_req_type req_type,
                                                               here_tracking_resp_type resp_type,
                                                               void* user_data)
{
    if(here_tracking_http_send_stream_fake.return_val == HERE_TRACKING_OK)
    {
        if(mock_here_tracking_http_send_result_data != NULL &&
           mock_here_tracking_http_send_result_data_size > 0)
        {
            here_tracking_recv_data data;

            data.err = mock_here_tracking_http_recv_cb_status;
            data.evt = HERE_TRACKING_RECV_EVT_RESP_SIZE;
            data.data_size = mock_here_tracking_http_send_result_data_size;
            recv_cb(&data, user_data);
            data.evt = HERE_TRACKING_RECV_EVT_RESP_DATA;
            data.data = (uint8_t*)mock_here_tracking_http_send_result_data;
            recv_cb(&data, user_data);
            data.evt = HERE_TRACKING_RECV_EVT_RESP_COMPLETE;
            data.data = NULL;
            data.data_size = 0;
            recv_cb(&data, user_data);
        }
    }

    return here_tracking_http_send_stream_fake.return_val;
}
