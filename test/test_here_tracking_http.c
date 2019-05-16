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

#include <stdbool.h>
#include <stdlib.h>

#include <check.h>
#include <fff.h>

#include "here_tracking_http.h"
#include "here_tracking_http_defs.h"
#include "here_tracking_test.h"

#include "mock_here_tracking_data_buffer.h"
#include "mock_here_tracking_log.h"
#include "mock_here_tracking_time.h"
#include "mock_here_tracking_tls.h"
#include "mock_here_tracking_tls_writer.h"
#include "mock_here_tracking_uuid_gen.h"

#define TEST_NAME "here_tracking_http"

/**************************************************************************************************/

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC6(here_tracking_error,
                 here_tracking_oauth_create_header,
                 const char*,
                 const char*,
                 const char*,
                 int32_t,
                 char*,
                 uint32_t*);

#define TEST_HERE_TRACKING_HTTP_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_DATA_BUFFER_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_LOG_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TIME_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TLS_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TLS_WRITER_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_UUID_GEN_FAKE_LIST(FAKE) \
    FAKE(here_tracking_oauth_create_header)

#define TEST_HERE_TRACKING_HTTP_TLS_READ_CHUNK_SIZE 256

/**************************************************************************************************/

static const char* fake_device_id = "1b25138b-c795-4b20-a724-59a40162d8fd";
static const char* fake_device_secret = "Ohkai3eF-im5UGai4J-bIPizRburaiLohr4DQNE6cvM";
static const char* fake_base_url = "tracking.api.here.com";
static const char* fake_access_token = \
    "h1.4KV1tI80nK18kF0uS41AGA.OcEbdmNvOsVAhiimFXySlSbVcenfu2RifECrgyNFx6lzo988s_G5AIsfcmkVTMbr1N"\
    "4QujwGJXmUni9tOzkQzJRLcmq7EM8ppUTukhet-_nxavCgNbI-R0ggOtrsxpF5lNy_f2xjN-QISJ91-os6j-nDy52TQJ"\
    "pIUmVZ10ghUoFLn1QwzlePWdLQwS9Pccdsc_ZSV78ueMf8ij6UHXJBeZHYUjrMMVhxYx8hBcJ2wa0tuG50T7GOvdTS5U"\
    "6KSMjF3HZLrg1-IC_dvFGm-f-J3n5tUVUTxWi8pVvBca8jlhS4GnA_ErKJQ8iwS6M2zsKnYtRxGS-F4cSnYZ06EG99_6"\
    "pyOh7pfpxPpJKjpRLqT3iAisuFWWRoPRuy72kgaHI-xbTsy6LFLZ-qC6O7548mRnQzlj0wSWYPb0LxR521CUWtyRFj2T"\
    "gZQHVCMvJ02FWPCyeFT5_svI8HQUYQ5Q135A.wMAt2juL0dnwqbs1Urb1Tnf4iJKEuCxIvz79hhb5aLXuTayUh4zKTYr"\
    "JJxSMouCIv9dohn0hRZ9yvk3zUKfQUTj6fbz6NCaDjzyXX5DqZMUXHELrfEuWwaaCqumwxaNTuZCPyjhKKHkkgExlSCF"\
    "00LnLnKE1dBrnGZ8q3YpZD2aBmR1-0k746QeXmTRF_F5fm46e-J7Q4QVmCr6OMhSPsEucjKHyneF2Ky1UxTG0art1_J5"\
    "MUrRHQZMoPx9u8lTJqh0r84PGb-mcXd8BtvgHVlSJ7bfxNRUobIewOma3eB7-3GuDd5DnaZHueZOF4_IylpKJmATgnaZ"\
    "u3kdt7Mmhrg";
static const char* fake_auth_resp = \
    "HTTP/1.1 200 OK\r\n"\
    "Content-Length: 875\r\n"\
    "\r\n"\
    "{\"accessToken\":\"h1.4KV1tI80nK18kF0uS41AGA.OcEbdmNvOsVAhiimFXySlSbVcenfu2RifECrgyNFx6lzo98"\
    "8s_G5AIsfcmkVTMbr1N4QujwGJXmUni9tOzkQzJRLcmq7EM8ppUTukhet-_nxavCgNbI-R0ggOtrsxpF5lNy_f2xjN-Q"\
    "ISJ91-os6j-nDy52TQJpIUmVZ10ghUoFLn1QwzlePWdLQwS9Pccdsc_ZSV78ueMf8ij6UHXJBeZHYUjrMMVhxYx8hBcJ"\
    "2wa0tuG50T7GOvdTS5U6KSMjF3HZLrg1-IC_dvFGm-f-J3n5tUVUTxWi8pVvBca8jlhS4GnA_ErKJQ8iwS6M2zsKnYtR"\
    "xGS-F4cSnYZ06EG99_6pyOh7pfpxPpJKjpRLqT3iAisuFWWRoPRuy72kgaHI-xbTsy6LFLZ-qC6O7548mRnQzlj0wSWY"\
    "Pb0LxR521CUWtyRFj2TgZQHVCMvJ02FWPCyeFT5_svI8HQUYQ5Q135A.wMAt2juL0dnwqbs1Urb1Tnf4iJKEuCxIvz79"\
    "hhb5aLXuTayUh4zKTYrJJxSMouCIv9dohn0hRZ9yvk3zUKfQUTj6fbz6NCaDjzyXX5DqZMUXHELrfEuWwaaCqumwxaNT"\
    "uZCPyjhKKHkkgExlSCF00LnLnKE1dBrnGZ8q3YpZD2aBmR1-0k746QeXmTRF_F5fm46e-J7Q4QVmCr6OMhSPsEucjKHy"\
    "neF2Ky1UxTG0art1_J5MUrRHQZMoPx9u8lTJqh0r84PGb-mcXd8BtvgHVlSJ7bfxNRUobIewOma3eB7-3GuDd5DnaZHu"\
    "eZOF4_IylpKJmATgnaZu3kdt7Mmhrg\",\"expiresIn\":86399}";
static const char* fake_send_resp = \
    "HTTP/1.1 200 OK\r\n"\
    "Content-Length: 21\r\n"\
    "\r\n"
    "THIS IS SEND RESPONSE";
static const char* fake_no_content_resp = \
    "HTTP/1.1 204 No Content\r\n"\
    "\r\n";
static const char* fake_bad_request_resp = \
    "HTTP/1.1 400 Bad Request\r\n"\
    "Content-Length: 0\r\n"\
    "\r\n";
static const char* fake_unauthorized_resp = \
    "HTTP/1.1 401 Unauthorized\r\n"\
    "Content-Length: 0\r\n"\
    "\r\n";
static const char* fake_forbidden_resp = \
    "HTTP/1.1 403 Forbidden\r\n"\
    "Content-Length: 0\r\n"\
    "\r\n";
static const char* fake_not_found_resp = \
    "HTTP/1.1 404 Not Found\r\n"\
    "Content-Length: 0\r\n"\
    "\r\n";
static const char* fake_precondition_failed_resp = \
    "HTTP/1.1 412 Precondition Failed\r\n"\
    "Content-Length: 0\r\n"\
    "\r\n";
static const char* fake_too_many_requests_resp = \
    "HTTP/1.1 429 Too Many Requests\r\n"\
    "Content-Length: 0\r\n"\
    "Retry-After: 3600\r\n"\
    "\r\n";
static const char* fake_too_many_requests_resp_no_retry_after = \
    "HTTP/1.1 429 Too Many Requests\r\n"\
    "Content-Length: 0\r\n"\
    "\r\n";
static const char* fake_unknown_resp = \
    "HTTP/1.1 999 I Don't Know This Code\r\n"\
    "Content-Length: 0\r\n"\
    "\r\n";
static const char* fake_x_here_ts_resp_unauthorized = \
    "HTTP/1.1 401 Unauthorized\r\n"\
    "Content-Length:0\r\n"\
    "x-here-timestamp: 3000\r\n"\
    "\r\n";
static const char* fake_x_here_ts_resp_ok = \
    "HTTP/1.1 200 OK\r\n"\
    "Content-Length: 875\r\n"\
    "x-here-timestamp: 3000\r\n"\
    "\r\n"\
    "{\"expiresIn\":86399,\"accessToken\":\"h1.4KV1tI80nK18kF0uS41AGA.OcEbdmNvOsVAhiimFXySlSbVcen"\
    "fu2RifECrgyNFx6lzo988s_G5AIsfcmkVTMbr1N4QujwGJXmUni9tOzkQzJRLcmq7EM8ppUTukhet-_nxavCgNbI-R0g"\
    "gOtrsxpF5lNy_f2xjN-QISJ91-os6j-nDy52TQJpIUmVZ10ghUoFLn1QwzlePWdLQwS9Pccdsc_ZSV78ueMf8ij6UHXJ"\
    "BeZHYUjrMMVhxYx8hBcJ2wa0tuG50T7GOvdTS5U6KSMjF3HZLrg1-IC_dvFGm-f-J3n5tUVUTxWi8pVvBca8jlhS4GnA"\
    "_ErKJQ8iwS6M2zsKnYtRxGS-F4cSnYZ06EG99_6pyOh7pfpxPpJKjpRLqT3iAisuFWWRoPRuy72kgaHI-xbTsy6LFLZ-"\
    "qC6O7548mRnQzlj0wSWYPb0LxR521CUWtyRFj2TgZQHVCMvJ02FWPCyeFT5_svI8HQUYQ5Q135A.wMAt2juL0dnwqbs1"\
    "Urb1Tnf4iJKEuCxIvz79hhb5aLXuTayUh4zKTYrJJxSMouCIv9dohn0hRZ9yvk3zUKfQUTj6fbz6NCaDjzyXX5DqZMUX"\
    "HELrfEuWwaaCqumwxaNTuZCPyjhKKHkkgExlSCF00LnLnKE1dBrnGZ8q3YpZD2aBmR1-0k746QeXmTRF_F5fm46e-J7Q"\
    "4QVmCr6OMhSPsEucjKHyneF2Ky1UxTG0art1_J5MUrRHQZMoPx9u8lTJqh0r84PGb-mcXd8BtvgHVlSJ7bfxNRUobIew"\
    "Oma3eB7-3GuDd5DnaZHueZOF4_IylpKJmATgnaZu3kdt7Mmhrg\"}";

/**************************************************************************************************/

static const char* fake_oauth_header = "9UOXxjR28bVrPv%2Fvn7YEwflTNtC9UOQndD8npf4xLJc%3D";

static here_tracking_error fake_here_tracking_oauth_create_header(const char* device_id,
                                                                  const char* device_secret,
                                                                  const char* base_url,
                                                                  int32_t srv_time_diff,
                                                                  char* out,
                                                                  uint32_t* out_size)
{
    if(here_tracking_oauth_create_header_fake.return_val == HERE_TRACKING_OK)
    {
        memcpy(out, fake_oauth_header, strlen(fake_oauth_header));
        (*out_size) = strlen(fake_oauth_header);
    }

    return here_tracking_oauth_create_header_fake.return_val;
}

/**************************************************************************************************/

static uint32_t test_here_tracking_http_recv_data_cb_called = 0;
static here_tracking_error test_here_tracking_http_recv_data_cb_status = HERE_TRACKING_ERROR;
static const char** mock_tls_read_data = NULL;
static uint32_t* mock_tls_read_data_size = NULL;
static uint8_t** test_here_tracking_http_send_chunks = NULL;
static size_t* test_here_tracking_http_send_chunk_sizes = NULL;
static uint8_t test_here_tracking_http_send_chunk_index = 0;
static const char* test_here_tracking_http_user_agent = "test-here-tracking-http";

/**************************************************************************************************/

void test_here_tracking_http_tc_setup(void)
{
    TEST_HERE_TRACKING_HTTP_FAKE_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    here_tracking_data_buffer_init_fake.return_val = HERE_TRACKING_OK;
    here_tracking_data_buffer_init_fake.custom_fake = mock_here_tracking_data_buffer_init_custom;
    here_tracking_data_buffer_add_char_fake.return_val = HERE_TRACKING_OK;
    here_tracking_data_buffer_add_char_fake.custom_fake = \
        mock_here_tracking_data_buffer_add_char_custom;
    here_tracking_data_buffer_add_string_fake.return_val = HERE_TRACKING_OK;
    here_tracking_data_buffer_add_string_fake.custom_fake = \
        mock_here_tracking_data_buffer_add_string_custom;
    here_tracking_data_buffer_add_data_fake.return_val = HERE_TRACKING_OK;
    here_tracking_data_buffer_add_data_fake.custom_fake = \
        mock_here_tracking_data_buffer_add_data_custom;
    here_tracking_tls_writer_init_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_writer_write_char_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_writer_write_data_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_writer_write_string_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_writer_write_utoa_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_writer_flush_fake.return_val = HERE_TRACKING_OK;
    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_OK;
    here_tracking_get_unixtime_fake.custom_fake = mock_here_tracking_get_unixtime_custom;
    here_tracking_tls_init_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_init_fake.custom_fake = mock_here_tracking_tls_init_custom;
    here_tracking_tls_connect_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_read_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_read_fake.custom_fake = mock_here_tracking_tls_read_custom;
    mock_here_tracking_tls_read_set_result_data(NULL, NULL, 0);
    here_tracking_uuid_gen_new_fake.return_val = HERE_TRACKING_OK;
    here_tracking_uuid_gen_new_fake.custom_fake = mock_here_tracking_uuid_gen_new_custom;
    here_tracking_oauth_create_header_fake.return_val = HERE_TRACKING_OK;
    here_tracking_oauth_create_header_fake.custom_fake = fake_here_tracking_oauth_create_header;
    test_here_tracking_http_recv_data_cb_called = 0;
    test_here_tracking_http_recv_data_cb_status = HERE_TRACKING_ERROR;
    test_here_tracking_http_send_chunks = NULL;
    test_here_tracking_http_send_chunk_sizes = NULL;
    test_here_tracking_http_send_chunk_index = 0;
}

/**************************************************************************************************/

void test_here_tracking_http_tc_teardown(void)
{
    if(mock_tls_read_data != NULL)
    {
        free(mock_tls_read_data);
        mock_tls_read_data = NULL;
    }

    if(mock_tls_read_data_size != NULL)
    {
        free(mock_tls_read_data_size);
        mock_tls_read_data_size = NULL;
    }
}

/**************************************************************************************************/

static void test_here_tracking_http_setup(here_tracking_client* client)
{
    memcpy(client->device_id, fake_device_id, HERE_TRACKING_DEVICE_ID_SIZE);
    memcpy(client->device_secret, fake_device_secret, HERE_TRACKING_DEVICE_SECRET_SIZE);
    strcpy(client->base_url, fake_base_url);
    memset(client->access_token, 0x00, HERE_TRACKING_ACCESS_TOKEN_SIZE);
    client->srv_time_diff = 0;
    client->data_cb = NULL;
    client->data_cb_user_data = NULL;
    client->tls = NULL;
    client->correlation_id = NULL;
    client->user_agent = NULL;
    client->retry_after = 0;
}

/**************************************************************************************************/

static void test_here_tracking_http_tls_read_set_result(const char* data)
{
    uint32_t* mock_tls_read_data_size;
    uint8_t i, chunk_count = strlen(data) / TEST_HERE_TRACKING_HTTP_TLS_READ_CHUNK_SIZE;

    if(strlen(data) % TEST_HERE_TRACKING_HTTP_TLS_READ_CHUNK_SIZE > 0)
    {
        chunk_count++;
    }

    mock_tls_read_data = malloc(chunk_count * sizeof(char*));
    mock_tls_read_data_size = malloc(chunk_count * sizeof(uint32_t));

    for(i = 0; i < chunk_count; ++i)
    {
        mock_tls_read_data[i] = data + (i * TEST_HERE_TRACKING_HTTP_TLS_READ_CHUNK_SIZE);

        if(chunk_count == (i + 1))
        {
            mock_tls_read_data_size[i] = \
                strlen(data + (i * TEST_HERE_TRACKING_HTTP_TLS_READ_CHUNK_SIZE));
        }
        else
        {
            mock_tls_read_data_size[i] = TEST_HERE_TRACKING_HTTP_TLS_READ_CHUNK_SIZE;
        }
    }

    mock_here_tracking_tls_read_set_result_data((const char**)mock_tls_read_data,
                                                mock_tls_read_data_size,
                                                chunk_count);
}

/**************************************************************************************************/

static void test_here_tracking_http_recv_data_cb_send_ok(here_tracking_error err,
                                                         const char* data,
                                                         uint32_t data_size,
                                                         void* cb_data)
{
    test_here_tracking_http_recv_data_cb_called++;
    test_here_tracking_http_recv_data_cb_status = err;
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(data_size == 21);
    ck_assert(memcmp(data, "THIS IS SEND RESPONSE", data_size) == 0);
}

/**************************************************************************************************/

static void test_here_tracking_http_recv_data_cb_send_too_small_resp_buffer(here_tracking_error err,
                                                                            const char* data,
                                                                            uint32_t data_size,
                                                                            void* cb_data)
{
    uint32_t buffer_size = *((uint32_t*)cb_data);
    test_here_tracking_http_recv_data_cb_called++;
    test_here_tracking_http_recv_data_cb_status = err;
    ck_assert(err == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
    ck_assert(data_size == 21);
    ck_assert(memcmp(data, "THIS IS SEND RESPONSE", buffer_size) == 0);
}

/**************************************************************************************************/

static void test_here_tracking_http_recv_data_cb_err(here_tracking_error err,
                                                     const char* data,
                                                     uint32_t data_size,
                                                     void* cb_data)
{
    test_here_tracking_http_recv_data_cb_called++;
    test_here_tracking_http_recv_data_cb_status = err;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_http_send_ok_cb(const uint8_t** data,
                                                              size_t* data_size,
                                                              void* user_data)
{
    if(test_here_tracking_http_send_chunks != NULL)
    {
        *data = test_here_tracking_http_send_chunks[test_here_tracking_http_send_chunk_index];
        *data_size = \
            test_here_tracking_http_send_chunk_sizes[test_here_tracking_http_send_chunk_index];
        test_here_tracking_http_send_chunk_index++;
    }
    else
    {
        *data = NULL;
        *data_size = 0;
    }

    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_http_recv_ok_cb(const here_tracking_recv_data* data,
                                                              void* user_data)
{
    ck_assert(data->err == HERE_TRACKING_OK);

    if(data->evt == HERE_TRACKING_RECV_EVT_RESP_SIZE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called == 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_DATA)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
        ck_assert(data->data_size == 21);
        ck_assert(data->data != NULL);
        ck_assert(memcmp(data->data, "THIS IS SEND RESPONSE", data->data_size) == 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_COMPLETE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called == 2);
    }

    test_here_tracking_http_recv_data_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error \
    test_here_tracking_http_recv_ok_no_content_cb(const here_tracking_recv_data* data,
                                                  void* user_data)
{
    ck_assert(data->err == HERE_TRACKING_OK);

    if(data->evt == HERE_TRACKING_RECV_EVT_RESP_SIZE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called == 0);
        ck_assert(data->data_size == 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_DATA)
    {
        ck_assert(true);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_COMPLETE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
    }

    test_here_tracking_http_recv_data_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error \
    test_here_tracking_http_recv_ok_multi_chunk_cb(const here_tracking_recv_data* data,
                                                   void* user_data)
{
    ck_assert(data->err == HERE_TRACKING_OK);

    if(data->evt == HERE_TRACKING_RECV_EVT_RESP_SIZE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called == 0);
        ck_assert(data->data_size > 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_DATA)
    {
        ck_assert(data->data_size > 0);
        ck_assert(data->data != NULL);
        ck_assert(test_here_tracking_http_recv_data_cb_called > 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_COMPLETE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called > 2);
    }

    test_here_tracking_http_recv_data_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_http_recv_err_cb(const here_tracking_recv_data* data,
                                                               void* user_data)
{
    if(data->evt == HERE_TRACKING_RECV_EVT_RESP_SIZE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called == 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_COMPLETE)
    {
        ck_assert(test_here_tracking_http_recv_data_cb_called > 0);
        test_here_tracking_http_recv_data_cb_status = data->err;
    }

    test_here_tracking_http_recv_data_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_auth_resp);
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_str_eq(client.access_token, fake_access_token);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_oauth_create_header_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok_tls_initialized)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_auth_resp);
    client.tls = (here_tracking_tls)1;
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_ge(strlen(client.access_token), 0);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_oauth_create_header_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok_user_agent_set)
{
    here_tracking_client client;
    here_tracking_error err;
    uint32_t i;
    bool user_agent_set = false;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_auth_resp);
    client.user_agent = test_here_tracking_http_user_agent;
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_str_eq(client.access_token, fake_access_token);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_oauth_create_header_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_writer_write_string_fake.arg_histories_dropped, 0);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                  here_tracking_http_header_user_agent) == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   test_here_tracking_http_user_agent) == 0))
        {
            user_agent_set = true;
        }
    }

    ck_assert(user_agent_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok_correlation_id_set)
{
    here_tracking_client client;
    here_tracking_error err;
    uint32_t i;
    bool correlation_id_set = false;
    const char* correlation_id = "my-correlation-id";

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_auth_resp);
    client.correlation_id = correlation_id;
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_str_eq(client.access_token, fake_access_token);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_oauth_create_header_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_writer_write_string_fake.arg_histories_dropped, 0);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                   "x-request-id") == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   correlation_id) == 0))
        {
            correlation_id_set = true;
        }
    }

    ck_assert(correlation_id_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok_user_agent_empty_string)
{
    here_tracking_client client;
    here_tracking_error err;
    uint32_t i;
    bool user_agent_set = false;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_auth_resp);
    client.user_agent = "";
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_str_eq(client.access_token, fake_access_token);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_oauth_create_header_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_writer_write_string_fake.arg_histories_dropped, 0);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count; ++i)
    {
        if(strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                  here_tracking_http_header_user_agent) == 0)
        {
            user_agent_set = true;
        }
    }

    ck_assert(!user_agent_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_tls_init_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_init_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(strlen(client.access_token), 0);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_oauth_create_header_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_tls_connect_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_connect_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(strlen(client.access_token), 0);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_oauth_create_header_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_oauth_create_header_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_oauth_create_header_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(strlen(client.access_token), 0);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_tls_writer_flush_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_writer_flush_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_writer_flush_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_bad_request)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_bad_request_resp);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_BAD_REQUEST);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_unauthorized)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_unauthorized_resp);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_UNAUTHORIZED);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_forbidden)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_forbidden_resp);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_FORBIDDEN);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_precondition)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_precondition_failed_resp);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_DEVICE_UNCLAIMED);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_x_here_ts)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_x_here_ts_resp_unauthorized);
    mock_here_tracking_get_unixtime_set_result(1000);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_TIME_MISMATCH);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(client.srv_time_diff == 2000);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
    ck_assert(here_tracking_get_unixtime_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok_x_here_ts_ignored)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_x_here_ts_resp_ok);
    mock_here_tracking_get_unixtime_set_result(1000);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(strcmp(client.access_token, fake_access_token) ==  0);
    ck_assert(client.srv_time_diff == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count >= 1);
    ck_assert(here_tracking_get_unixtime_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_tls_writer_init_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_writer_init_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(here_tracking_tls_writer_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_writer_write_char_fake.call_count == 0);
    ck_assert(here_tracking_tls_writer_write_data_fake.call_count == 0);
    ck_assert(here_tracking_tls_writer_write_string_fake.call_count == 0);
    ck_assert(here_tracking_tls_writer_flush_fake.call_count == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_tls_writer_write_char_fail)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    here_tracking_tls_writer_write_char_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(here_tracking_tls_writer_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_writer_write_char_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_tls_writer_write_string_fail)
{
    here_tracking_client client;
    here_tracking_error err;

    test_here_tracking_http_setup(&client);
    here_tracking_tls_writer_write_string_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(here_tracking_tls_writer_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_writer_write_string_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_ok)
{
    here_tracking_client client;
    here_tracking_error err;
    char data[100];

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    client.data_cb = test_here_tracking_http_recv_data_cb_send_ok;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_tls_init_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    char data[100];

    test_here_tracking_http_setup(&client);
    here_tracking_tls_init_fake.return_val = HERE_TRACKING_ERROR;
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_read_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_write_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_close_fake.call_count, 0);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 0);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_tls_connect_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    char data[100];

    test_here_tracking_http_setup(&client);
    here_tracking_tls_connect_fake.return_val = HERE_TRACKING_ERROR;
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_read_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_write_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_close_fake.call_count, 0);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_tls_writer_write_string_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    char data[100];

    test_here_tracking_http_setup(&client);
    here_tracking_tls_writer_write_string_fake.return_val = HERE_TRACKING_ERROR;
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_int_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_int_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_int_eq(here_tracking_tls_writer_init_fake.call_count, 1);
    ck_assert_int_eq(here_tracking_tls_writer_write_string_fake.call_count, 1);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_tls_read_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    char data[100];

    test_here_tracking_http_setup(&client);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    here_tracking_tls_read_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_int_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_int_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_int_eq(here_tracking_tls_writer_init_fake.call_count, 1);
    ck_assert_int_eq(here_tracking_tls_read_fake.call_count, 1);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_too_large_to_parse_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    char data[100];
    char mock_resp[2048];

    test_here_tracking_http_setup(&client);
    strcpy(client.access_token, fake_access_token);
    memset(mock_resp, 'A', 2047);
    mock_resp[2047] = '\0';
    memcpy(mock_resp,
           "HTTP/1.1 200 OK\r\nx-large-header:",
           strlen("HTTP/1.1 200 OK\r\nx-large-header:"));
    test_here_tracking_http_tls_read_set_result(mock_resp);
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_too_small_resp_buffer)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_error data_buffer_add_data_res[2] =
    {
        HERE_TRACKING_ERROR_BUFFER_TOO_SMALL,
        HERE_TRACKING_OK
    };
    char data[10];
    uint32_t data_size = 10;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    SET_RETURN_SEQ(here_tracking_data_buffer_add_data, data_buffer_add_data_res, 2);
    client.data_cb = test_here_tracking_http_recv_data_cb_send_too_small_resp_buffer;
    client.data_cb_user_data = (void*)(&data_size);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, data_size, data_size);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_bad_request)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_bad_request_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_bad_request_resp) };
    char data[10];
    uint32_t data_size = 10;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, data_size, data_size);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR_BAD_REQUEST);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_unauthorized)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_unauthorized_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_unauthorized_resp) };
    char data[10];
    uint32_t data_size = 10;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, data_size, data_size);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 1);
    ck_assert_int_eq(test_here_tracking_http_recv_data_cb_status, HERE_TRACKING_ERROR_UNAUTHORIZED);
    ck_assert_str_eq(client.access_token, "");
    ck_assert_uint_eq(client.token_expiry, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_forbidden)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_forbidden_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_forbidden_resp) };
    char data[10];
    uint32_t data_size = 10;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, data_size, data_size);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 1);
    ck_assert_int_eq(test_here_tracking_http_recv_data_cb_status, HERE_TRACKING_ERROR_FORBIDDEN);
    ck_assert_str_eq(client.access_token, "");
    ck_assert_uint_eq(client.token_expiry, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_unknown_error_code)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_unknown_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_unknown_resp) };
    char data[10];
    uint32_t data_size = 10;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, data_size, data_size);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_ok)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_ok_user_agent_set)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";
    uint32_t i;
    bool user_agent_set = false;

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    client.user_agent = test_here_tracking_http_user_agent;
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                   here_tracking_http_header_user_agent) == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   test_here_tracking_http_user_agent) == 0))
        {
            user_agent_set = true;
        }
    }

    ck_assert(user_agent_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_ok_user_agent_empty_string)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";
    uint32_t i;
    bool user_agent_set = false;

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    client.user_agent = "";
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                   here_tracking_http_header_user_agent) == 0))
        {
            user_agent_set = true;
        }
    }

    ck_assert(!user_agent_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_ok_proto_req)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";
    uint32_t i;
    bool proto_content_type_set = false;

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_PROTOBUF,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                   here_tracking_http_header_content_type) == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   here_tracking_http_content_type_octet_stream) == 0))
        {
            proto_content_type_set = true;
        }
    }

    ck_assert(proto_content_type_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_ok_proto_resp)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";
    uint32_t i;
    bool proto_accept_set = false;

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_PROTOBUF,
                                         HERE_TRACKING_RESP_WITH_DATA_PROTOBUF,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 1);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                   here_tracking_http_header_accept) == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   here_tracking_http_content_type_octet_stream) == 0))
        {
            proto_accept_set = true;
        }
    }

    ck_assert(proto_accept_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_tls_init_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_init_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_read_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_write_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_close_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_tls_writer_init_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_writer_init_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_writer_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_read_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_write_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_close_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_tls_writer_write_char_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_writer_write_char_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
    ck_assert_uint_eq(here_tracking_tls_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_connect_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_writer_init_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_read_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_writer_write_char_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_tls_write_fake.call_count, 0);
    ck_assert_uint_eq(here_tracking_tls_close_fake.call_count, 1);
    ck_assert_uint_eq(here_tracking_uuid_gen_new_fake.call_count, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_too_large_to_parse_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";
    char mock_resp[2048];

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    strcpy(client.access_token, fake_access_token);
    memset(mock_resp, 'A', 2047);
    mock_resp[2047] = '\0';
    memcpy(mock_resp,
           "HTTP/1.1 200 OK\r\nx-large-header:",
           strlen("HTTP/1.1 200 OK\r\nx-large-header:"));
    test_here_tracking_http_tls_read_set_result(mock_resp);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_bad_request)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_bad_request_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_bad_request_resp) };
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_err_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 2);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR_BAD_REQUEST);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_unauthorized)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_unauthorized_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_unauthorized_resp) };
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_err_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 2);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR_UNAUTHORIZED);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_forbidden)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_forbidden_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_forbidden_resp) };
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_err_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 2);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR_FORBIDDEN);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_not_found)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_not_found_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_not_found_resp) };
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_err_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 2);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR_NOT_FOUND);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_unknown_error_code)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_unknown_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_unknown_resp) };
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_err_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 2);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_no_content)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_no_content_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_no_content_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_STATUS_ONLY,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 2);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_too_many_requests)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_too_many_requests_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_too_many_requests_resp) };
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";
    uint32_t time_in_test = 1000;

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    mock_here_tracking_get_unixtime_set_result(time_in_test);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_err_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 2);
    ck_assert_int_eq(test_here_tracking_http_recv_data_cb_status,
                     HERE_TRACKING_ERROR_TOO_MANY_REQUESTS);
    ck_assert_uint_eq(client.retry_after, time_in_test + 3600);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_too_many_requests_no_retry_after)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_too_many_requests_resp_no_retry_after };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_too_many_requests_resp_no_retry_after) };
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_err;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_err_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 2);
    ck_assert_int_eq(test_here_tracking_http_recv_data_cb_status,
                     HERE_TRACKING_ERROR_TOO_MANY_REQUESTS);
    ck_assert_uint_eq(client.retry_after, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_ok_send_multi_chunk)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[3];
    size_t chunk_sizes[3];
    char* data = "test_data";
    char* data2 = "more_test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = (uint8_t*)data2;
    chunks[2] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = strlen(data2);
    chunk_sizes[2] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_stream_ok_recv_multi_chunk)
{
    here_tracking_client client;
    here_tracking_error err;
    uint8_t* chunks[2];
    size_t chunk_sizes[2];
    char* data = "test_data";

    chunks[0] = (uint8_t*)data;
    chunks[1] = NULL;
    chunk_sizes[0] = strlen(data);
    chunk_sizes[1] = 0;
    test_here_tracking_http_send_chunks = chunks;
    test_here_tracking_http_send_chunk_sizes = chunk_sizes;
    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_auth_resp);
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send_stream(&client,
                                         test_here_tracking_http_send_ok_cb,
                                         test_here_tracking_http_recv_ok_multi_chunk_cb,
                                         HERE_TRACKING_REQ_DATA_JSON,
                                         HERE_TRACKING_RESP_WITH_DATA_JSON,
                                         NULL);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called > 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_ok)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.here.com";
    char* path = "/index.html";

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = NULL;
    request.header_count = 0;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_ok_user_agent_set)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.here.com";
    char* path = "/index.html";
    uint32_t i;
    bool user_agent_set = false;

    test_here_tracking_http_setup(&client);
    client.user_agent = test_here_tracking_http_user_agent;
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = NULL;
    request.header_count = 0;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                  here_tracking_http_header_user_agent) == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   test_here_tracking_http_user_agent) == 0))
        {
            user_agent_set = true;
        }
    }

    ck_assert(user_agent_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_ok_custom_user_agent_set)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    here_tracking_http_header header;
    char* host = "tracking.here.com";
    char* path = "/index.html";
    char* my_user_agent = "my-very-own-user-agent";
    uint32_t i;
    bool user_agent_set = false;

    test_here_tracking_http_setup(&client);
    client.user_agent = test_here_tracking_http_user_agent;
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    header.name = (char*)here_tracking_http_header_user_agent;
    header.value = my_user_agent;
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = &header;
    request.header_count = 1;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                  here_tracking_http_header_user_agent) == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   my_user_agent) == 0))
        {
            user_agent_set = true;
        }
    }

    ck_assert(user_agent_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_ok_user_agent_empty_string)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.here.com";
    char* path = "/index.html";
    uint32_t i;
    bool user_agent_set = false;

    test_here_tracking_http_setup(&client);
    client.user_agent = "";
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = NULL;
    request.header_count = 0;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                   here_tracking_http_header_user_agent) == 0))
        {
            user_agent_set = true;
        }
    }

    ck_assert(!user_agent_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_ok_custom_correlation_id_set)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    here_tracking_http_header header;
    char* host = "tracking.here.com";
    char* path = "/index.html";
    char* my_correlation_id = "my-very-own-correlation-id";
    uint32_t i;
    bool correlation_id_set = false;

    test_here_tracking_http_setup(&client);
    client.user_agent = test_here_tracking_http_user_agent;
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    header.name = "x-request-id";
    header.value = my_correlation_id;
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = &header;
    request.header_count = 1;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);

    for(i = 0; i < here_tracking_tls_writer_write_string_fake.call_count - 1; ++i)
    {
        if((strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i],
                   "x-request-id") == 0) &&
           (strcmp(here_tracking_tls_writer_write_string_fake.arg1_history[i + 1],
                   my_correlation_id) == 0))
        {
            correlation_id_set = true;
        }
    }

    ck_assert(correlation_id_set);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_ok_with_auth)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.here.com";
    char* path = "/index.html";
    here_tracking_http_header header;

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    header.name = (char*)here_tracking_http_header_authorization;
    header.value = client.access_token;
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = &header;
    request.header_count = 1;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_ok_with_auth_bearer)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.here.com";
    char* path = "/index.html";
    here_tracking_http_header header;
    char auth_header_value[1000] = {0};

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    strcat(auth_header_value, "Bearer ");
    strcat(auth_header_value, client.access_token);
    header.name = (char*)here_tracking_http_header_authorization;
    header.value = auth_header_value;
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = &header;
    request.header_count = 1;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_here_tracking_http_recv_data_cb_called, 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_get_invalid_input)
{
    here_tracking_client client;
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.here.com";
    char* path = "/index.html";

    test_here_tracking_http_setup(&client);
    test_here_tracking_http_tls_read_set_result(fake_send_resp);
    strcpy(client.access_token, fake_access_token);
    request.host = host;
    request.path = path;
    request.port = 443;
    request.headers = NULL;
    request.header_count = 0;
    err = here_tracking_http_get(NULL, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_http_get(&client, NULL, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_http_get(&client, &request, NULL, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR_INVALID_INPUT);
    request.host = NULL;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR_INVALID_INPUT);
    request.host = host;
    request.path = NULL;
    err = here_tracking_http_get(&client, &request, test_here_tracking_http_recv_ok_cb, NULL);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

TEST_SUITE_BEGIN(TEST_NAME)
    TEST_SUITE_ADD_SETUP_TEARDOWN_FN(test_here_tracking_http_tc_setup,
                                     test_here_tracking_http_tc_teardown)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_ok_tls_initialized)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_ok_user_agent_set);
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_ok_correlation_id_set);
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_ok_user_agent_empty_string);
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_tls_init_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_tls_connect_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_oauth_create_header_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_tls_writer_flush_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_fail_bad_request)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_fail_unauthorized)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_fail_forbidden)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_fail_precondition)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_x_here_ts)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_ok_x_here_ts_ignored)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_tls_writer_init_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_tls_writer_write_char_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_auth_tls_writer_write_string_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_tls_init_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_tls_connect_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_tls_writer_write_string_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_tls_read_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_too_large_to_parse_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_too_small_resp_buffer)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_bad_request)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_unauthorized)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_forbidden)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_unknown_error_code)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_ok_user_agent_set)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_ok_user_agent_empty_string)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_ok_proto_req)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_ok_proto_resp)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_tls_init_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_tls_writer_init_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_tls_writer_write_char_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_too_large_to_parse_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_bad_request)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_unauthorized)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_forbidden)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_not_found)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_unknown_error_code)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_no_content)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_too_many_requests)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_too_many_requests_no_retry_after)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_ok_send_multi_chunk)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_send_stream_ok_recv_multi_chunk)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_ok_user_agent_set)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_ok_custom_user_agent_set)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_ok_user_agent_empty_string)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_ok_custom_correlation_id_set)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_ok_with_auth)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_ok_with_auth_bearer)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_get_invalid_input)
TEST_SUITE_END

/**************************************************************************************************/

TEST_MAIN(TEST_NAME)
