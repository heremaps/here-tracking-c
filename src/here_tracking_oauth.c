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

#include "here_tracking_base64.h"
#include "here_tracking_data_buffer.h"
#include "here_tracking_hmac_sha.h"
#include "here_tracking_http.h"
#include "here_tracking_log.h"
#include "here_tracking_oauth.h"
#include "here_tracking_time.h"
#include "here_tracking_utils.h"

/**************************************************************************************************/

static const char* here_tracking_oauth_key =                  "OAuth";
static const char* here_tracking_oauth_consumer_key_key =     "oauth_consumer_key";
static const char* here_tracking_oauth_nonce_key =            "oauth_nonce";
static const char* here_tracking_oauth_realm_key =            "realm";
static const char* here_tracking_oauth_realm_val =            "IoT";
static const char* here_tracking_oauth_signature_key =        "oauth_signature";
static const char* here_tracking_oauth_signature_method_key = "oauth_signature_method";
static const char* here_tracking_oauth_signature_method_val = "HMAC-SHA256";
static const char* here_tracking_oauth_timestamp_key =        "oauth_timestamp";
static const char* here_tracking_oauth_version_key =          "oauth_version";
static const char* here_tracking_oauth_version_val =          "1.0";

/**************************************************************************************************/

typedef struct
{
    char* key;
    char* val;
    uint8_t key_len;
    uint8_t val_len;
} here_tracking_oauth_param_t;

#define HERE_TRACKING_OAUTH_PARAM_COUNT 5

typedef struct
{
    here_tracking_oauth_param_t params[HERE_TRACKING_OAUTH_PARAM_COUNT];
} here_tracking_oauth_params_t;

/**************************************************************************************************/

#define HERE_TRACKING_OAUTH_WORK_BUF_SIZE 384

#define TRY(OP) if((err = (OP)) != HERE_TRACKING_OK) { goto here_tracking_oauth_error; }

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_add_oauth_key(here_tracking_data_buffer* data_buf);

static here_tracking_error here_tracking_oauth_add_realm(here_tracking_data_buffer* data_buf);

static here_tracking_error here_tracking_oauth_add_consumer_key(here_tracking_data_buffer* data_buf,
                                                                const char* device_id,
                                                                here_tracking_oauth_param_t* param);

static here_tracking_error here_tracking_oauth_add_nonce(here_tracking_data_buffer* data_buf,
                                                         here_tracking_oauth_param_t* param);

static here_tracking_error \
    here_tracking_oauth_add_signature_method(here_tracking_data_buffer* data_buf,
                                             here_tracking_oauth_param_t* param);

static here_tracking_error here_tracking_oauth_add_timestamp(here_tracking_data_buffer* data_buf,
                                                             here_tracking_oauth_param_t* param,
                                                             int32_t srv_time_diff);

static here_tracking_error here_tracking_oauth_add_version(here_tracking_data_buffer* data_buf,
                                                           here_tracking_oauth_param_t* param);

static here_tracking_error \
    here_tracking_oauth_add_signature(here_tracking_data_buffer* data_buf,
                                      const here_tracking_oauth_params_t* params,
                                      const char* device_secret,
                                      const char* base_url);

static char here_tracking_oauth_to_hex(char code);

static here_tracking_error here_tracking_oauth_percent_encode(here_tracking_data_buffer* data_buf,
                                                              const char* in,
                                                              uint32_t in_size);

#define HERE_TRACKING_OAUTH_NONCE_VAL_SIZE 10

static here_tracking_error \
    here_tracking_oauth_create_nonce_val(here_tracking_data_buffer* data_buf);

#define HERE_TRACKING_OAUTH_TIMESTAMP_VAL_SIZE 10

static here_tracking_error \
    here_tracking_oauth_create_timestamp_val(here_tracking_data_buffer* data_buf,
                                             int32_t srv_time_diff);

#define HERE_TRACKING_OAUTH_MIN_SIGNATURE_VAL_SIZE 48

static here_tracking_error \
    here_tracking_oauth_create_signature_val(here_tracking_data_buffer* data_buf,
                                             const here_tracking_oauth_params_t* params,
                                             const char* device_secret,
                                             const char* base_url);

static here_tracking_error \
    here_tracking_oauth_create_base_string(here_tracking_data_buffer* data_buf,
                                           const here_tracking_oauth_params_t* params,
                                           const char* base_url);

/**************************************************************************************************/

here_tracking_error here_tracking_oauth_create_header(const char* device_id,
                                                      const char* device_secret,
                                                      const char* base_url,
                                                      int32_t srv_time_diff,
                                                      char* out,
                                                      uint32_t* out_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR;

    if(device_id != NULL && device_secret != NULL && base_url != NULL && out != NULL &&
       out_size != NULL)
    {
        if((*out_size) >= HERE_TRACKING_OAUTH_MIN_OUT_SIZE)
        {
            here_tracking_data_buffer data_buf;
            here_tracking_oauth_params_t params; /* OAuth parameters for signature calculation */

            TRY((here_tracking_data_buffer_init(&data_buf, out, (*out_size))));
            TRY((here_tracking_oauth_add_oauth_key(&data_buf)));
            TRY((here_tracking_oauth_add_realm(&data_buf)));
            TRY((here_tracking_oauth_add_consumer_key(&data_buf, device_id, &(params.params[0]))));
            TRY((here_tracking_oauth_add_nonce(&data_buf, &(params.params[1]))));
            TRY((here_tracking_oauth_add_signature_method(&data_buf, &(params.params[2]))));
            TRY((here_tracking_oauth_add_timestamp(&data_buf, &(params.params[3]), srv_time_diff)));
            TRY((here_tracking_oauth_add_version(&data_buf, &(params.params[4]))));
            TRY((here_tracking_oauth_add_signature(&data_buf, &params, device_secret, base_url)));
            HERE_TRACKING_LOGI("OAuth authorization header - len: %u, val: %.*s",
                               data_buf.buffer_size,
                               data_buf.buffer_size,
                               data_buf.buffer);
            (*out_size) = data_buf.buffer_size;
        }
        else
        {
            err = HERE_TRACKING_ERROR_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        err = HERE_TRACKING_ERROR_INVALID_INPUT;
    }

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_add_oauth_key(here_tracking_data_buffer* data_buf)
{
    here_tracking_error err;

    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, ' ')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_add_realm(here_tracking_data_buffer* data_buf)
{
    here_tracking_error err;

    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_realm_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '=')));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_realm_val)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_data_buffer_add_char(data_buf, ',')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_add_consumer_key(here_tracking_data_buffer* data_buf,
                                                                const char* device_id,
                                                                here_tracking_oauth_param_t* param)
{
    here_tracking_error err;

    param->key = data_buf->buffer + data_buf->buffer_size;
    param->key_len = strlen(here_tracking_oauth_consumer_key_key);
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_consumer_key_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '=')));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    param->val = data_buf->buffer + data_buf->buffer_size;
    param->val_len = HERE_TRACKING_DEVICE_ID_SIZE;
    TRY((here_tracking_data_buffer_add_data(data_buf, device_id, HERE_TRACKING_DEVICE_ID_SIZE)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_data_buffer_add_char(data_buf, ',')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_add_nonce(here_tracking_data_buffer* data_buf,
                                                         here_tracking_oauth_param_t* param)
{
    here_tracking_error err;

    param->key = data_buf->buffer + data_buf->buffer_size;
    param->key_len = strlen(here_tracking_oauth_nonce_key);
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_nonce_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '=')));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    param->val = data_buf->buffer + data_buf->buffer_size;
    param->val_len = HERE_TRACKING_OAUTH_NONCE_VAL_SIZE;
    TRY((here_tracking_oauth_create_nonce_val(data_buf)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_data_buffer_add_char(data_buf, ',')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_oauth_add_signature_method(here_tracking_data_buffer* data_buf,
                                             here_tracking_oauth_param_t* param)
{
    here_tracking_error err;

    param->key = data_buf->buffer + data_buf->buffer_size;
    param->key_len = strlen(here_tracking_oauth_signature_method_key);
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_signature_method_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '=')));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    param->val = data_buf->buffer + data_buf->buffer_size;
    param->val_len = strlen(here_tracking_oauth_signature_method_val);
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_signature_method_val)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_data_buffer_add_char(data_buf, ',')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_add_timestamp(here_tracking_data_buffer* data_buf,
                                                             here_tracking_oauth_param_t* param,
                                                             int32_t srv_time_diff)
{
    here_tracking_error err;

    param->key = data_buf->buffer + data_buf->buffer_size;
    param->key_len = strlen(here_tracking_oauth_timestamp_key);
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_timestamp_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '=')));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    param->val = data_buf->buffer + data_buf->buffer_size;
    TRY((here_tracking_oauth_create_timestamp_val(data_buf, srv_time_diff)));
    param->val_len = (data_buf->buffer + data_buf->buffer_size) - param->val;
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_data_buffer_add_char(data_buf, ',')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_add_version(here_tracking_data_buffer* data_buf,
                                                           here_tracking_oauth_param_t* param)
{
    here_tracking_error err;

    param->key = data_buf->buffer + data_buf->buffer_size;
    param->key_len = strlen(here_tracking_oauth_version_key);
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_version_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '=')));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    param->val = data_buf->buffer + data_buf->buffer_size;
    param->val_len = strlen(here_tracking_oauth_version_val);
    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_version_val)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_data_buffer_add_char(data_buf, ',')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_oauth_add_signature(here_tracking_data_buffer* data_buf,
                                      const here_tracking_oauth_params_t* params,
                                      const char* device_secret,
                                      const char* base_url)
{
    here_tracking_error err;

    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_oauth_signature_key)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '=')));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));
    TRY((here_tracking_oauth_create_signature_val(data_buf, params, device_secret, base_url)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '\"')));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static char here_tracking_oauth_to_hex(char code)
{
    static char hex[] = "0123456789ABCDEF";
    return hex[code & 15];
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_percent_encode(here_tracking_data_buffer* data_buf,
                                                              const char* in,
                                                              uint32_t in_size)
{
    here_tracking_error err = HERE_TRACKING_OK;
    uint32_t i;

    for(i = 0; i < in_size; ++i)
    {
        char c = in[i];

        if(here_tracking_utils_isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            TRY((here_tracking_data_buffer_add_char(data_buf, c)));
        }
        else
        {
            TRY((here_tracking_data_buffer_add_char(data_buf, '%')));
            TRY((here_tracking_data_buffer_add_char(data_buf, here_tracking_oauth_to_hex(c >> 4))));
            TRY((here_tracking_data_buffer_add_char(data_buf, here_tracking_oauth_to_hex(c & 15))));
        }
    }

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error here_tracking_oauth_create_nonce_val(here_tracking_data_buffer* data_buf)
{
    static uint32_t prev_ts = 0;
    here_tracking_error err;
    uint32_t ts, i;

    TRY((here_tracking_get_unixtime(&ts)));

    /* Do not initialize random generator with same seed as it will then create the same nonce */
    if(prev_ts >= ts)
    {
        ts = ++prev_ts;
    }
    else
    {
        prev_ts = ts;
    }

    srand(ts);

    for(i = 0; i < HERE_TRACKING_OAUTH_NONCE_VAL_SIZE; ++i)
    {
        /* For now using only digits for nonce. */
        TRY((here_tracking_data_buffer_add_char(data_buf, ((rand() % 10) + '0'))));
    }

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_oauth_create_timestamp_val(here_tracking_data_buffer* data_buf,
                                             int32_t srv_time_diff)
{
    here_tracking_error err;
    uint32_t ts;

    TRY((here_tracking_get_unixtime(&ts)));
    ts += srv_time_diff;
    TRY((here_tracking_data_buffer_add_utoa(data_buf, ts)));

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_oauth_create_signature_val(here_tracking_data_buffer* data_buf,
                                             const here_tracking_oauth_params_t* params,
                                             const char* device_secret,
                                             const char* base_url)
{
    here_tracking_error err;
    here_tracking_data_buffer work_buf;
    uint32_t base_string_size, size;
    char work_buf_mem[HERE_TRACKING_OAUTH_WORK_BUF_SIZE];
#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_INFO
    uint32_t tmp_size;
#endif

    TRY((here_tracking_data_buffer_init(&work_buf,
                                        work_buf_mem,
                                        HERE_TRACKING_OAUTH_WORK_BUF_SIZE)));

    /* Create base string to the beginning of work buffer. */
    TRY((here_tracking_oauth_create_base_string(&work_buf, params, base_url)));
    base_string_size = work_buf.buffer_size;
    HERE_TRACKING_LOGI("OAuth base string - len: %u val: %.*s",
                       base_string_size,
                       base_string_size,
                       work_buf.buffer);

    /* As there is no token secret, signature key is device secret + '&'.
       Write it to work buffer after base string. */
    TRY((here_tracking_data_buffer_add_data(&work_buf,
                                            device_secret,
                                            HERE_TRACKING_DEVICE_SECRET_SIZE)));
    TRY((here_tracking_data_buffer_add_char(&work_buf, '&')));

    /* Create HMAC and write it to work buffer after signature key. */
    size = work_buf.buffer_capacity - work_buf.buffer_size;
    TRY((here_tracking_hmac_sha256(work_buf.buffer,
                                   base_string_size,
                                   work_buf.buffer + base_string_size,
                                   HERE_TRACKING_DEVICE_SECRET_SIZE + 1,
                                   work_buf.buffer + work_buf.buffer_size,
                                   &size)));
    HERE_TRACKING_LOGI("Bytes in OAuth work buffer %u", (work_buf.buffer_size + size));
    size = work_buf.buffer_size;
    TRY((here_tracking_base64_enc(work_buf.buffer + size,
                                  HERE_TRACKING_HMAC_SHA256_OUT_SIZE,
                                  work_buf.buffer,
                                  &size)));

#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_INFO
    tmp_size = data_buf->buffer_size;
#endif

    /* Percent encode the signature and write result to the final output buffer. */
    TRY((here_tracking_oauth_percent_encode(data_buf, work_buf.buffer, size)));

#if HERE_TRACKING_LOG_LEVEL <= HERE_TRACKING_LOG_LEVEL_INFO
    tmp_size = data_buf->buffer_size - tmp_size;
    HERE_TRACKING_LOGI("OAuth signature - len: %u val: %.*s",
                       tmp_size,
                       tmp_size,
                       (data_buf->buffer + data_buf->buffer_size - tmp_size));
#endif

here_tracking_oauth_error:
    return err;
}

/**************************************************************************************************/

static here_tracking_error \
    here_tracking_oauth_create_base_string(here_tracking_data_buffer* data_buf,
                                           const here_tracking_oauth_params_t* params,
                                           const char* base_url)
{
    here_tracking_error err;
    uint8_t i;

    TRY((here_tracking_data_buffer_add_string(data_buf, here_tracking_http_method_post)));
    TRY((here_tracking_data_buffer_add_char(data_buf, '&')));
    TRY((here_tracking_oauth_percent_encode(data_buf,
                                            here_tracking_http_protocol_https,
                                            (uint32_t)strlen(here_tracking_http_protocol_https))));
    TRY((here_tracking_data_buffer_add_string(data_buf, base_url)));
    TRY((here_tracking_oauth_percent_encode(data_buf,
                                        here_tracking_http_device_http_version,
                                        (uint32_t)strlen(here_tracking_http_device_http_version))));
    TRY((here_tracking_oauth_percent_encode(data_buf,
                                        here_tracking_http_device_http_token,
                                        (uint32_t)strlen(here_tracking_http_device_http_token))));
    TRY((here_tracking_data_buffer_add_char(data_buf, '&')));

    for(i = 0; i < HERE_TRACKING_OAUTH_PARAM_COUNT; ++i)
    {
        const here_tracking_oauth_param_t* param = &(params->params[i]);

        TRY((here_tracking_data_buffer_add_data(data_buf, param->key, param->key_len)));
        TRY((here_tracking_oauth_percent_encode(data_buf, "=", sizeof(char))));
        TRY((here_tracking_data_buffer_add_data(data_buf, param->val, param->val_len)));

        if(i < (HERE_TRACKING_OAUTH_PARAM_COUNT - 1))
        {
            TRY((here_tracking_oauth_percent_encode(data_buf, "&", sizeof(char))));
        }
    }

here_tracking_oauth_error:
    return err;
}
