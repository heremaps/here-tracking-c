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

#include <string.h>

#include "here_tracking.h"
#include "here_tracking_http.h"
#include "here_tracking_time.h"

/**************************************************************************************************/

here_tracking_error here_tracking_init(here_tracking_client* client,
                                       const char* device_id,
                                       const char* device_secret,
                                       const char* base_url)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(client != NULL &&
       device_id != NULL &&
       device_secret != NULL &&
       base_url != NULL &&
       strlen(base_url) < HERE_TRACKING_BASE_URL_SIZE)
    {
        memcpy(client->device_id, device_id, HERE_TRACKING_DEVICE_ID_SIZE);
        memcpy(client->device_secret, device_secret, HERE_TRACKING_DEVICE_SECRET_SIZE);
        strcpy(client->base_url, base_url);
        client->access_token[0] = '\0';
        client->srv_time_diff = 0;
        client->token_expiry = 0;
        client->tls = NULL;
        client->data_cb = NULL;
        client->data_cb_user_data = NULL;
        err = HERE_TRACKING_OK;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_free(here_tracking_client* client)
{
    here_tracking_error err = HERE_TRACKING_OK;

    if(client != NULL)
    {
        if(client->tls != NULL)
        {
            err = here_tracking_tls_free(&client->tls);
            client->tls = NULL;
        }
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_set_recv_data_cb(here_tracking_client* client,
                                                   here_tracking_recv_data_cb cb,
                                                   void* user_data)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(client != NULL)
    {
        client->data_cb = cb;
        client->data_cb_user_data = user_data;
        err = HERE_TRACKING_OK;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_auth(here_tracking_client* client)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(client != NULL)
    {
        client->access_token[0] = '\0';
        client->token_expiry = 0;
        err = here_tracking_http_auth(client);

        if(err == HERE_TRACKING_ERROR_TIME_MISMATCH)
        {
            err = here_tracking_http_auth(client);
        }
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error here_tracking_send(here_tracking_client* client,
                                       char* data,
                                       uint32_t send_size,
                                       uint32_t recv_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR_INVALID_INPUT;

    if(client != NULL && data != NULL && send_size > 0 && recv_size > 0)
    {
        uint32_t ts;

        err = here_tracking_get_unixtime(&ts);

        if(err == HERE_TRACKING_OK &&
           (strlen(client->access_token) == 0 || client->token_expiry < ts))
        {
            err = here_tracking_auth(client);
        }

        if(err == HERE_TRACKING_OK)
        {
            err = here_tracking_http_send(client, data, send_size, recv_size);
        }
    }

    return err;
}
