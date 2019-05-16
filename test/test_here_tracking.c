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

#include <stdlib.h>

#include <check.h>
#include <fff.h>

#include "here_tracking.h"
#include "here_tracking_test.h"

#include "mock_here_tracking_http.h"
#include "mock_here_tracking_time.h"
#include "mock_here_tracking_tls.h"

#define TEST_NAME "here_tracking"

/**************************************************************************************************/

DEFINE_FFF_GLOBALS;

#define TEST_HERE_TRACKING_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_HTTP_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TIME_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TLS_FAKE_LIST(FAKE)

/**************************************************************************************************/

static const char* device_id = "1b25138b-c795-4b20-a724-59a40162d8fd";
static const char* device_secret = "Ohkai3eF-im5UGai4J-bIPizRburaiLohr4DQNE6cvM";
static const char* base_url = "tracking.api.here.com";
static const char* mock_access_token = \
    "h1.9I7RD07L16pZ1SHE0cBi8A.NDYszKzzm4roHiFOfcW9LOYRgRriSGSyqRTP-oKDJjW1FvnL0yIV_7AHfZdfJXR2Gr6"\
    "TsULwqmUQEjlnBoY3O8X1deWLefb2ZC9542DmOb5Nig2I1Fvnpzrfvo0N0wZBHfTDRu4jz2FjgetRTxtXtzWrrepfmTvv"\
    "k-W6GZqVmBsNJbAprIbUs6mkniKLUneXkdrRZuYkEaFWdkUt6EpRCzwNJnbXq-_GFwxSwwh95AvLjcw2VAQYGuN5Cg0UK"\
    "cKq9rLAF0qXvZHXhWOGwUd97kQFU6_s9CSbevCrIxnP8Lvb1pTWUPNUWnrWZa6-9NGEigrkn_H8zKxDjJ6TtXNSUDWmow"\
    "J6MJnPeL6Hh0HF3rod4otLW_ll50JFmiZticlyh7SBg0rE0GWVcgwStsh7_S_cvbujiqjZnwo7vm46ormD_74Aun006mW"\
    "c1vNqKvSC5ZxcVRCUczYyISJ0HaeO_A.ap8zMGJNqfijzE1d3P_oTEv4A_AdOoSdDDLWZpKC0gz-467RVED3kEuVabJS2"\
    "MAUGD8mCRiMMA8Mv-5jcdjJC14vevIPLptswR9F8IUTQIpJF8Exxqh1l7mqF5gzEjrGatFNcOD1MuEY6r5LqRf_cfeesX"\
    "U4soTzg_0sytRwT1URfKE70lunmgeSGAtzXUHxXWakXQBFAdD1vHN22pNHCubn8nUSVHFSuW2XcU6v5gROf2_usvO_MtJ"\
    "mogC2xG8Kqyn-MDD38fZ4MoFg2u8grW__NBmqlkC3QFvpp6ukGoywo9SlHxhYiD0UNHZNq9b2CAVzAtA4lp-Uzx05XQYt"\
    "PA";
static const char* mock_recv_data = "MOCK_RECV_DATA";

/**************************************************************************************************/

static uint32_t recv_data_cb_called = 0;
static here_tracking_error recv_data_cb_status = HERE_TRACKING_OK;

/**************************************************************************************************/

static void test_here_tracking_tc_setup()
{
    TEST_HERE_TRACKING_FAKE_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    here_tracking_http_auth_fake.return_val = HERE_TRACKING_OK;
    here_tracking_http_auth_fake.custom_fake = mock_here_tracking_http_auth_custom;
    here_tracking_http_send_fake.return_val = HERE_TRACKING_OK;
    here_tracking_http_send_fake.custom_fake = mock_here_tracking_http_send_custom;
    here_tracking_http_send_stream_fake.return_val = HERE_TRACKING_OK;
    here_tracking_http_send_stream_fake.custom_fake = mock_here_tracking_http_send_stream_custom;
    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_OK;
    here_tracking_get_unixtime_fake.custom_fake = mock_here_tracking_get_unixtime_custom;
    here_tracking_tls_free_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_free_fake.custom_fake = mock_here_tracking_tls_free_custom;
    mock_here_tracking_http_auth_set_result_token(mock_access_token);
    mock_here_tracking_http_send_set_result_data(NULL, 0);
    mock_here_tracking_http_set_recv_cb_status(HERE_TRACKING_OK);
    recv_data_cb_called = 0;
    recv_data_cb_status = HERE_TRACKING_OK;
}

/**************************************************************************************************/

void test_here_tracking_tc_teardown(void)
{
}

/**************************************************************************************************/

static void test_here_tracking_recv_data_cb_nop(here_tracking_error err,
                                                const char* data,
                                                uint32_t data_size,
                                                void* user_data)
{
    recv_data_cb_called++;
    return;
}

/**************************************************************************************************/

static void test_here_tracking_recv_data_cb_send_ok(here_tracking_error err,
                                                    const char* data,
                                                    uint32_t data_size,
                                                    void* user_data)
{
    recv_data_cb_called++;
    ck_assert(err == HERE_TRACKING_OK);
    ck_assert(data != NULL);
    ck_assert(memcmp(data, mock_recv_data, strlen(mock_recv_data)) == 0);
    ck_assert(data_size == strlen(mock_recv_data));
    return;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_send_cb(const uint8_t** data,
                                                      size_t* data_size,
                                                      void* user_data)
{
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_recv_cb(const here_tracking_recv_data* data,
                                                      void* user_data)
{
    recv_data_cb_status = data->err;
    recv_data_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

START_TEST(test_here_tracking_init_ok)
{
    here_tracking_client client;
    here_tracking_error res;
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(memcmp(client.device_id, device_id, HERE_TRACKING_DEVICE_ID_SIZE) == 0);
    ck_assert(memcmp(client.device_secret, device_secret, HERE_TRACKING_DEVICE_SECRET_SIZE) == 0);
    ck_assert(strcmp(client.base_url, base_url) == 0);
    ck_assert(strlen(client.access_token) == 0);
    ck_assert(client.data_cb == NULL);
    ck_assert(client.data_cb_user_data == NULL);
    ck_assert(client.correlation_id == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_init_invalid_input)
{
    here_tracking_client client;
    char base_url_too_long[HERE_TRACKING_BASE_URL_SIZE + 1];
    here_tracking_error res;
    res = here_tracking_init(NULL, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_init(&client, NULL, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_init(&client, device_id, NULL, base_url);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_init(&client, device_id, device_secret, NULL);
    memset(base_url_too_long, 'A', HERE_TRACKING_BASE_URL_SIZE);
    base_url_too_long[HERE_TRACKING_BASE_URL_SIZE] = '\0';
    res = here_tracking_init(&client, device_id, device_secret, base_url_too_long);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_auth_ok)
{
    here_tracking_client client;
    here_tracking_error res;
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_auth(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strcmp(client.access_token, mock_access_token) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_auth_time_mismatch)
{
    here_tracking_client client;
    here_tracking_error res;
    here_tracking_error http_auth_res[2] =
    {
        HERE_TRACKING_ERROR_TIME_MISMATCH,
        HERE_TRACKING_OK
    };

    SET_RETURN_SEQ(here_tracking_http_auth, http_auth_res, 2);
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_auth(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strcmp(client.access_token, mock_access_token) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_auth_invalid_input)
{
    here_tracking_error res;
    res = here_tracking_auth(NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_set_recv_data_cb_ok)
{
    here_tracking_client client;
    here_tracking_error res;
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_recv_data_cb_nop, &client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.data_cb == test_here_tracking_recv_data_cb_nop);
    ck_assert(client.data_cb_user_data == &client);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_set_recv_data_cb_invalid_input)
{
    here_tracking_error res;
    res = here_tracking_set_recv_data_cb(NULL, test_here_tracking_recv_data_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_ok)
{
    here_tracking_client client;
    here_tracking_error res;
    char data[100];
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_recv_data_cb_send_ok, &client);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_auth(&client);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send(&client, data, 100, 100);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(recv_data_cb_called == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_invalid_input)
{
    here_tracking_client client;
    here_tracking_error res;
    static char data[100];
    res = here_tracking_send(NULL, data, 100, 100);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_send(&client, NULL, 100, 100);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_send(&client, data, 0, 100);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_send(&client, data, 100, 0);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_no_token_yet)
{
    here_tracking_client client;
    here_tracking_error res;
    char data[100];
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_recv_data_cb_send_ok, &client);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send(&client, data, 100, 100);
    ck_assert(strcmp(client.access_token, mock_access_token) == 0);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(recv_data_cb_called == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_token_expired)
{
    here_tracking_client client;
    here_tracking_error res;
    char data[100];
    mock_here_tracking_get_unixtime_set_result(1000);
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    strcpy(client.access_token, "expired_token");
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_recv_data_cb_send_ok, &client);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send(&client, data, 100, 100);
    ck_assert(strcmp(client.access_token, mock_access_token) == 0);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(recv_data_cb_called == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_time_error)
{
    here_tracking_client client;
    here_tracking_error res;
    char data[100];
    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_ERROR;
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_recv_data_cb_send_ok, &client);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send(&client, data, 100, 100);
    ck_assert(res == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_too_many_requests)
{
    here_tracking_client client;
    here_tracking_error res;
    char data[100];
    uint32_t time_in_test = 1000;

    mock_here_tracking_get_unixtime_set_result(time_in_test);
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_recv_data_cb_send_ok, &client);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    client.retry_after = time_in_test + 100;
    res = here_tracking_send(&client, data, 100, 100);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_TOO_MANY_REQUESTS);
    ck_assert_uint_eq(recv_data_cb_called, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_free_tls_initialized)
{
    here_tracking_client client;
    here_tracking_error res;
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    client.tls = (here_tracking_tls)1;
    res = here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.tls == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_free_tls_null)
{
    here_tracking_client client;
    here_tracking_error res;
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_free_client_null)
{
    here_tracking_error res = here_tracking_free(NULL);
    ck_assert(res == HERE_TRACKING_OK);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_ok)
{
    here_tracking_client client;
    here_tracking_error res;

    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_auth(&client);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(recv_data_cb_called == 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_invalid_input)
{
    here_tracking_client client;
    here_tracking_error res;

    res = here_tracking_send_stream(NULL,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_send_stream(&client,
                                    NULL,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    NULL,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_no_token_yet)
{
    here_tracking_client client;
    here_tracking_error res;

    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert(strcmp(client.access_token, mock_access_token) == 0);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(recv_data_cb_called == 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_token_expired)
{
    here_tracking_client client;
    here_tracking_error res;

    mock_here_tracking_get_unixtime_set_result(1000);
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    strcpy(client.access_token, "expired_token");
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert(strcmp(client.access_token, mock_access_token) == 0);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(recv_data_cb_called == 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_token_expiry_offset)
{
    here_tracking_client client;
    here_tracking_error res;
    uint32_t time_in_test = 1000, token_expires_in = 60;

    mock_here_tracking_get_unixtime_set_result(time_in_test);
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    strcpy(client.access_token, "token_about_to_expire");
    client.token_expiry = time_in_test + token_expires_in;
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert_str_eq(client.access_token, mock_access_token);
    ck_assert_uint_eq(here_tracking_http_auth_fake.call_count, 1);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_uint_eq(recv_data_cb_called, 3);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_time_error)
{
    here_tracking_client client;
    here_tracking_error res;

    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_ERROR;
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert(res == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_time_error_seq)
{
    here_tracking_client client;
    here_tracking_error res;
    here_tracking_error get_unixtime_res[2] =
    {
        HERE_TRACKING_OK,
        HERE_TRACKING_ERROR
    };

    SET_RETURN_SEQ(here_tracking_get_unixtime, get_unixtime_res, 2);
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_too_many_requests)
    here_tracking_client client;
    here_tracking_error res;
    uint32_t time_in_test = 1000;

    mock_here_tracking_get_unixtime_set_result(time_in_test);
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    res = here_tracking_auth(&client);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    client.retry_after = time_in_test + 100;
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_TOO_MANY_REQUESTS);
    ck_assert_uint_eq(recv_data_cb_called, 0);
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_send_stream_too_many_requests_cb)
    here_tracking_client client;
    here_tracking_error res;

    mock_here_tracking_http_set_recv_cb_status(HERE_TRACKING_ERROR_TOO_MANY_REQUESTS);
    mock_here_tracking_http_send_set_result_data(mock_recv_data, strlen(mock_recv_data));
    res = here_tracking_init(&client, device_id, device_secret, base_url);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    res = here_tracking_auth(&client);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    res = here_tracking_send_stream(&client,
                                    test_here_tracking_send_cb,
                                    test_here_tracking_recv_cb,
                                    HERE_TRACKING_REQ_DATA_JSON,
                                    HERE_TRACKING_RESP_WITH_DATA_JSON,
                                    NULL);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_int_eq(recv_data_cb_status, HERE_TRACKING_ERROR_TOO_MANY_REQUESTS);
    ck_assert_uint_eq(recv_data_cb_called, 3);
END_TEST

/**************************************************************************************************/

TEST_SUITE_BEGIN(TEST_NAME)
    TEST_SUITE_ADD_SETUP_TEARDOWN_FN(test_here_tracking_tc_setup,
                                     test_here_tracking_tc_teardown)
    TEST_SUITE_ADD_TEST(test_here_tracking_init_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_init_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_auth_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_auth_time_mismatch)
    TEST_SUITE_ADD_TEST(test_here_tracking_auth_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_set_recv_data_cb_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_set_recv_data_cb_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_no_token_yet)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_token_expired)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_time_error)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_too_many_requests)
    TEST_SUITE_ADD_TEST(test_here_tracking_free_tls_initialized)
    TEST_SUITE_ADD_TEST(test_here_tracking_free_tls_null)
    TEST_SUITE_ADD_TEST(test_here_tracking_free_client_null)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_no_token_yet)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_token_expired)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_token_expiry_offset)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_time_error)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_time_error_seq)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_too_many_requests)
    TEST_SUITE_ADD_TEST(test_here_tracking_send_stream_too_many_requests_cb)
TEST_SUITE_END

/**************************************************************************************************/

TEST_MAIN(TEST_NAME)
