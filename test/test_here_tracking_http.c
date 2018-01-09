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

#include <check.h>
#include <fff.h>

#include "here_tracking_http.h"

#include "mock_here_tracking_data_buffer.h"
#include "mock_here_tracking_time.h"
#include "mock_here_tracking_tls.h"

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
    MOCK_HERE_TRACKING_TIME_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TLS_FAKE_LIST(FAKE) \
    FAKE(here_tracking_oauth_create_header)

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
static const char* fake_precondition_failed_resp = \
    "HTTP/1.1 412 Precondition Failed\r\n"\
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

static void test_here_tracking_http_setup(here_tracking_client* client)
{
    TEST_HERE_TRACKING_HTTP_FAKE_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    memcpy(client->device_id, fake_device_id, HERE_TRACKING_DEVICE_ID_SIZE);
    memcpy(client->device_secret, fake_device_secret, HERE_TRACKING_DEVICE_SECRET_SIZE);
    strcpy(client->base_url, fake_base_url);
    memset(client->access_token, 0x00, HERE_TRACKING_ACCESS_TOKEN_SIZE);
    client->srv_time_diff = 0;
    client->data_cb = NULL;
    client->data_cb_user_data = NULL;
    client->tls = NULL;
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
    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_OK;
    here_tracking_get_unixtime_fake.custom_fake = mock_here_tracking_get_unixtime_custom;
    here_tracking_tls_init_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_init_fake.custom_fake = mock_here_tracking_tls_init_custom;
    here_tracking_tls_connect_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_read_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_read_fake.custom_fake = mock_here_tracking_tls_read_custom;
    mock_here_tracking_tls_read_set_result_data(NULL, NULL, 0);
    here_tracking_tls_write_fake.return_val = HERE_TRACKING_OK;
    here_tracking_oauth_create_header_fake.return_val = HERE_TRACKING_OK;
    here_tracking_oauth_create_header_fake.custom_fake = fake_here_tracking_oauth_create_header;
    test_here_tracking_http_recv_data_cb_called = 0;
    test_here_tracking_http_recv_data_cb_status = HERE_TRACKING_ERROR;
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

START_TEST(test_here_tracking_http_auth_ok)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_auth_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_auth_resp) };
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(strcmp(client.access_token, fake_access_token) ==  0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok_tls_initialized)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_auth_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_auth_resp) };
    test_here_tracking_http_setup(&client);
    client.tls = (here_tracking_tls)1;
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 0);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
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
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 0);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 0);
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
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 0);
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
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_tls_write_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_tls_write_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_write_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_bad_request)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_bad_request_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_bad_request_resp) };
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_BAD_REQUEST);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_write_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_unauthorized)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_unauthorized_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_unauthorized_resp) };
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_UNAUTHORIZED);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_write_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_forbidden)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_forbidden_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_forbidden_resp) };
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_FORBIDDEN);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_write_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_fail_precondition)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_precondition_failed_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_precondition_failed_resp) };
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_DEVICE_UNCLAIMED);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_write_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_x_here_ts)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_x_here_ts_resp_unauthorized };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_x_here_ts_resp_unauthorized) };
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    mock_here_tracking_get_unixtime_set_result(1000);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR_TIME_MISMATCH);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(client.srv_time_diff == 2000);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_write_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
    ck_assert(here_tracking_get_unixtime_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_ok_x_here_ts_ignored)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_x_here_ts_resp_ok };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_x_here_ts_resp_ok) };
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    mock_here_tracking_get_unixtime_set_result(1000);
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(strcmp(client.access_token, fake_access_token) ==  0);
    ck_assert(client.srv_time_diff == 0);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 1);
    ck_assert(here_tracking_oauth_create_header_fake.call_count == 1);
    ck_assert(here_tracking_tls_write_fake.call_count == 1);
    ck_assert(here_tracking_tls_read_fake.call_count == 1);
    ck_assert(here_tracking_get_unixtime_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_data_buffer_init_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_data_buffer_init_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(here_tracking_data_buffer_init_fake.call_count == 1);
    ck_assert(here_tracking_data_buffer_add_char_fake.call_count == 0);
    ck_assert(here_tracking_data_buffer_add_string_fake.call_count == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_data_buffer_add_char_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_data_buffer_add_char_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(here_tracking_data_buffer_init_fake.call_count == 1);
    ck_assert(here_tracking_data_buffer_add_char_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_auth_data_buffer_add_string_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    test_here_tracking_http_setup(&client);
    here_tracking_data_buffer_add_string_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_http_auth(&client);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(here_tracking_data_buffer_init_fake.call_count == 1);
    ck_assert(here_tracking_data_buffer_add_string_fake.call_count == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_ok)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_send_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_send_resp) };
    char data[100];
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
    client.data_cb = test_here_tracking_http_recv_data_cb_send_ok;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_ok_multi_chunk)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[2] = { fake_send_resp, fake_send_resp + 20 };
    uint32_t mock_tls_read_data_size[2] = { 20, strlen(fake_send_resp) - 20 };
    char data[100];
    test_here_tracking_http_setup(&client);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 2);
    client.data_cb = test_here_tracking_http_recv_data_cb_send_ok;
    strcpy(client.access_token, fake_access_token);
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
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
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert(err == HERE_TRACKING_ERROR);
    ck_assert(here_tracking_tls_init_fake.call_count == 1);
    ck_assert(here_tracking_tls_connect_fake.call_count == 0);
    ck_assert(here_tracking_tls_read_fake.call_count == 0);
    ck_assert(here_tracking_tls_write_fake.call_count == 0);
    ck_assert(here_tracking_tls_close_fake.call_count == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_too_large_to_parse_fail)
{
    here_tracking_client client;
    here_tracking_error err;
    char data[100];
    static const char* first_chunk = "HTTP/1.1 200 OK\r\nx-large-header:";
    char* second_chunk;
    const char* mock_tls_read_data[2];
    uint32_t mock_tls_read_data_size[2];
    test_here_tracking_http_setup(&client);
    strcpy(client.access_token, fake_access_token);
    second_chunk = malloc(2048);
    memset(second_chunk, 'A', 2048);
    mock_tls_read_data[0] = first_chunk;
    mock_tls_read_data[1] = second_chunk;
    mock_tls_read_data_size[0] = strlen(first_chunk);
    mock_tls_read_data_size[1] = 2048;
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 2);
    err = here_tracking_http_send(&client, data, 100, 100);
    ck_assert(err == HERE_TRACKING_ERROR);
    free(second_chunk);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_send_too_small_resp_buffer)
{
    here_tracking_client client;
    here_tracking_error err;
    const char* mock_tls_read_data[1] = { fake_send_resp };
    uint32_t mock_tls_read_data_size[1] = { strlen(fake_send_resp) };
    here_tracking_error data_buffer_add_data_res[2] =
    {
        HERE_TRACKING_ERROR_BUFFER_TOO_SMALL,
        HERE_TRACKING_OK
    };
    char data[10];
    uint32_t data_size = 10;
    test_here_tracking_http_setup(&client);
    SET_RETURN_SEQ(here_tracking_data_buffer_add_data, data_buffer_add_data_res, 2);
    mock_here_tracking_tls_read_set_result_data(mock_tls_read_data, mock_tls_read_data_size, 1);
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
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR_UNAUTHORIZED);
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
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_http_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_http_recv_data_cb_status == HERE_TRACKING_ERROR_FORBIDDEN);
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

Suite* test_here_tracking_http_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_http_auth_ok);
    tcase_add_test(tc, test_here_tracking_http_auth_ok_tls_initialized);
    tcase_add_test(tc, test_here_tracking_http_auth_tls_init_fail);
    tcase_add_test(tc, test_here_tracking_http_auth_tls_connect_fail);
    tcase_add_test(tc, test_here_tracking_http_auth_oauth_create_header_fail);
    tcase_add_test(tc, test_here_tracking_http_auth_tls_write_fail);
    tcase_add_test(tc, test_here_tracking_http_auth_fail_bad_request);
    tcase_add_test(tc, test_here_tracking_http_auth_fail_unauthorized);
    tcase_add_test(tc, test_here_tracking_http_auth_fail_forbidden);
    tcase_add_test(tc, test_here_tracking_http_auth_fail_precondition);
    tcase_add_test(tc, test_here_tracking_http_auth_x_here_ts);
    tcase_add_test(tc, test_here_tracking_http_auth_ok_x_here_ts_ignored);
    tcase_add_test(tc, test_here_tracking_http_auth_data_buffer_init_fail);
    tcase_add_test(tc, test_here_tracking_http_auth_data_buffer_add_char_fail);
    tcase_add_test(tc, test_here_tracking_http_auth_data_buffer_add_string_fail);
    tcase_add_test(tc, test_here_tracking_http_send_ok);
    tcase_add_test(tc, test_here_tracking_http_send_ok_multi_chunk);
    tcase_add_test(tc, test_here_tracking_http_send_tls_init_fail);
    tcase_add_test(tc, test_here_tracking_http_send_too_large_to_parse_fail);
    tcase_add_test(tc, test_here_tracking_http_send_too_small_resp_buffer);
    tcase_add_test(tc, test_here_tracking_http_send_bad_request);
    tcase_add_test(tc, test_here_tracking_http_send_unauthorized);
    tcase_add_test(tc, test_here_tracking_http_send_forbidden);
    tcase_add_test(tc, test_here_tracking_http_send_unknown_error_code);
    suite_add_tcase(s, tc);
    return s;
}

/**************************************************************************************************/

int main()
{
    int failed;
    SRunner* sr = srunner_create(test_here_tracking_http_suite());
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
