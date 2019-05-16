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

#if defined HERE_TRACKING_TEST_DEVICE_ID && defined HERE_TRACKING_TEST_DEVICE_SECRET

#include <stdio.h>

#include <check.h>

#include "here_tracking.h"
#include "here_tracking_time.h"
#include "here_tracking_version.h"

#define TEST_NAME "here_tracking_prod"

#define TEST_DATA_BUFFER_SIZE 2048

#define INIT_RECV_CB_DATA \
    test_here_tracking_prod_recv_data_cb_called = 0; \
    test_here_tracking_prod_recv_data_cb_size = -1; \
    test_here_tracking_prod_recv_data_cb_status = HERE_TRACKING_ERROR

/**************************************************************************************************/

static const char* base_url = "tracking.api.here.com";
static const char* user_agent = "here-tracking-c-test/"HERE_TRACKING_VERSION_STRING;

/**************************************************************************************************/

static const char* basic_telemetry = \
    "[{\"payload\":{\"prodTestFor\":\"here-tracking-c\"},\"timestamp\":%llu}]";

static const char* basic_telemetry_chunk1 = "[{\"payload\":{\"prodTestFor\":\"here-tracking-c\"},";

static const char* basic_telemetry_chunk2 = "\"timestamp\":%llu}]";

/* Manufacturer string is invalid in this (should be at least 2 chars) */
static const char* invalid_telemetry = \
    "[{\"payload\":{\"prodTestFor\":\"here-tracking-c\"},\"timestamp\":%llu,"\
    "\"system\":{\"client\":{\"manufacturer\":\"a\"}}}]";

/**************************************************************************************************/

static void test_here_tracking_prod_create_basic_telemetry(char* buffer, uint32_t buffer_size)
{
    uint32_t ts;
    here_tracking_get_unixtime(&ts);
    memset(buffer, 0x00, buffer_size);
    snprintf(buffer, buffer_size, basic_telemetry, ((unsigned long long)ts) * 1000);
}

/**************************************************************************************************/

static void test_here_tracking_prod_create_invalid_telemetry(char* buffer, uint32_t buffer_size)
{
    uint32_t ts;
    here_tracking_get_unixtime(&ts);
    memset(buffer, 0x00, buffer_size);
    snprintf(buffer, buffer_size, invalid_telemetry, ((unsigned long long)ts) * 1000);
}

/**************************************************************************************************/

static uint32_t test_here_tracking_prod_recv_data_cb_called = 0;
static int32_t test_here_tracking_prod_recv_data_cb_size = -1;
static here_tracking_error test_here_tracking_prod_recv_data_cb_status = HERE_TRACKING_ERROR;

static void test_here_tracking_prod_recv_data_cb(here_tracking_error err,
                                                 const char* data,
                                                 uint32_t data_size,
                                                 void* cb_data)
{
    test_here_tracking_prod_recv_data_cb_called++;
    test_here_tracking_prod_recv_data_cb_status = err;
    test_here_tracking_prod_recv_data_cb_size = data_size;
    return;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_prod_send_ok_cb(const uint8_t** data,
                                                              size_t* data_size,
                                                              void* user_data)
{
    static uint8_t chunk_count = 0;
    char buffer[TEST_DATA_BUFFER_SIZE];

    if(chunk_count == 0)
    {
        *data = (uint8_t*)basic_telemetry_chunk1;
        *data_size = strlen(basic_telemetry_chunk1);
        chunk_count++;
    }
    else if(chunk_count == 1)
    {
        uint32_t ts;

        here_tracking_get_unixtime(&ts);
        memset(buffer, 0x00, TEST_DATA_BUFFER_SIZE);
        snprintf(buffer,
                 TEST_DATA_BUFFER_SIZE,
                 basic_telemetry_chunk2,
                 ((unsigned long long)ts) * 1000);
        *data = (uint8_t*)buffer;
        *data_size = strlen(buffer);
        chunk_count++;
    }
    else
    {
        *data = NULL;
        *data_size = 0;
        chunk_count = 0;
    }

    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_prod_recv_ok_cb(const here_tracking_recv_data* data,
                                                              void* user_data)
{
    ck_assert(data->err == HERE_TRACKING_OK);

    if(data->evt == HERE_TRACKING_RECV_EVT_RESP_SIZE)
    {
        ck_assert(test_here_tracking_prod_recv_data_cb_called == 0);
        ck_assert(data->data_size > 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_DATA)
    {
        ck_assert(test_here_tracking_prod_recv_data_cb_called > 0);
        ck_assert(data->data_size > 0);
        ck_assert(data->data != NULL);
        test_here_tracking_prod_recv_data_cb_size += data->data_size;
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_COMPLETE)
    {
        ck_assert(test_here_tracking_prod_recv_data_cb_called > 1);
        test_here_tracking_prod_recv_data_cb_status = data->err;
    }

    test_here_tracking_prod_recv_data_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

static here_tracking_error test_here_tracking_prod_recv_error_cb(const here_tracking_recv_data* data,
                                                                 void* user_data)
{
    if(data->evt == HERE_TRACKING_RECV_EVT_RESP_SIZE)
    {
        ck_assert_int_eq(data->err, HERE_TRACKING_OK);
        ck_assert_uint_eq(test_here_tracking_prod_recv_data_cb_called, 0);
        ck_assert_uint_gt(data->data_size, 0);
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_DATA)
    {
        ck_assert_int_eq(data->err, HERE_TRACKING_OK);
        ck_assert_uint_gt(test_here_tracking_prod_recv_data_cb_called, 0);
        ck_assert_uint_gt(data->data_size, 0);
        ck_assert_ptr_ne(data->data, NULL);
        test_here_tracking_prod_recv_data_cb_size += data->data_size;
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_COMPLETE)
    {
        ck_assert_int_ne(data->err, HERE_TRACKING_OK);
        ck_assert_uint_ge(test_here_tracking_prod_recv_data_cb_called, 1);
        test_here_tracking_prod_recv_data_cb_status = data->err;
    }

    test_here_tracking_prod_recv_data_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_send_ok)
{
    here_tracking_client client;
    char data[TEST_DATA_BUFFER_SIZE];
    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert(res == HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_prod_recv_data_cb, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_auth(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(client.token_expiry > 0);
    test_here_tracking_prod_create_basic_telemetry(data, TEST_DATA_BUFFER_SIZE);
    INIT_RECV_CB_DATA;
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_prod_recv_data_cb_status == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_size > 0);
    here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.tls == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_adjust_time_diff)
{
    here_tracking_client client;
    char data[TEST_DATA_BUFFER_SIZE];
    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert(res == HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_prod_recv_data_cb, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    client.srv_time_diff = -600; /* Deliberately set a time diff */
    res = here_tracking_auth(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(client.token_expiry > 0);
    ck_assert(client.srv_time_diff <= 5 && client.srv_time_diff >= - 5); /* Allow a small diff */
    test_here_tracking_prod_create_basic_telemetry(data, TEST_DATA_BUFFER_SIZE);
    INIT_RECV_CB_DATA;
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_prod_recv_data_cb_status == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_size > 0);
    here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.tls == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_send_ok_no_auth)
{
    here_tracking_client client;
    char data[TEST_DATA_BUFFER_SIZE];
    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert(res == HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_prod_recv_data_cb, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    test_here_tracking_prod_create_basic_telemetry(data, TEST_DATA_BUFFER_SIZE);
    INIT_RECV_CB_DATA;
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(client.token_expiry > 0);
    ck_assert(test_here_tracking_prod_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_prod_recv_data_cb_status == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_size > 0);
    here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.tls == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_send_ok_expired_token)
{
    here_tracking_client client;
    char data[TEST_DATA_BUFFER_SIZE];
    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert(res == HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_prod_recv_data_cb, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    test_here_tracking_prod_create_basic_telemetry(data, TEST_DATA_BUFFER_SIZE);
    INIT_RECV_CB_DATA;
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(client.token_expiry > 0);
    ck_assert(test_here_tracking_prod_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_prod_recv_data_cb_status == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_size > 0);
    client.token_expiry = 0;
    INIT_RECV_CB_DATA;
    test_here_tracking_prod_create_basic_telemetry(data, TEST_DATA_BUFFER_SIZE);
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(client.token_expiry > 0);
    ck_assert(test_here_tracking_prod_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_prod_recv_data_cb_status == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_size > 0);
    here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.tls == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_send_ok_expiry_offset_token)
{
    here_tracking_client client;
    char data[TEST_DATA_BUFFER_SIZE];
    uint32_t token_expires_in = 60, ts;
    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert_int_eq(res, HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_prod_recv_data_cb, NULL);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    test_here_tracking_prod_create_basic_telemetry(data, TEST_DATA_BUFFER_SIZE);
    INIT_RECV_CB_DATA;
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_uint_gt(strlen(client.access_token), 0);
    ck_assert_uint_gt(client.token_expiry, 0);
    ck_assert_uint_eq(test_here_tracking_prod_recv_data_cb_called, 1);
    ck_assert_int_eq(test_here_tracking_prod_recv_data_cb_status, HERE_TRACKING_OK);
    ck_assert_uint_gt(test_here_tracking_prod_recv_data_cb_size, 0);
    res = here_tracking_get_unixtime(&ts);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    client.token_expiry = ts + token_expires_in;
    INIT_RECV_CB_DATA;
    test_here_tracking_prod_create_basic_telemetry(data, TEST_DATA_BUFFER_SIZE);
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_uint_gt(strlen(client.access_token), 0);
    ck_assert_uint_gt(client.token_expiry, (ts + token_expires_in));
    ck_assert_uint_eq(test_here_tracking_prod_recv_data_cb_called, 1);
    ck_assert_int_eq(test_here_tracking_prod_recv_data_cb_status, HERE_TRACKING_OK);
    ck_assert_uint_gt(test_here_tracking_prod_recv_data_cb_size, 0);
    here_tracking_free(&client);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_ptr_eq(client.tls, NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_send_invalid_data)
{
    here_tracking_client client;
    char data[TEST_DATA_BUFFER_SIZE];
    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert(res == HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_set_recv_data_cb(&client, test_here_tracking_prod_recv_data_cb, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    test_here_tracking_prod_create_invalid_telemetry(data, TEST_DATA_BUFFER_SIZE);
    INIT_RECV_CB_DATA;
    res = here_tracking_send(&client, data, strlen(data), TEST_DATA_BUFFER_SIZE);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(client.token_expiry > 0);
    ck_assert(test_here_tracking_prod_recv_data_cb_called == 1);
    ck_assert(test_here_tracking_prod_recv_data_cb_status == HERE_TRACKING_ERROR_BAD_REQUEST);
    ck_assert(test_here_tracking_prod_recv_data_cb_size > 0);
    here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.tls == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_send_stream_ok)
{
    here_tracking_client client;

    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert(res == HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_auth(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(strlen(client.access_token) > 0);
    ck_assert(client.token_expiry > 0);
    INIT_RECV_CB_DATA;
    res = here_tracking_send_stream(&client,
                              test_here_tracking_prod_send_ok_cb,
                              test_here_tracking_prod_recv_ok_cb,
                              HERE_TRACKING_REQ_DATA_JSON,
                              HERE_TRACKING_RESP_WITH_DATA_JSON,
                              NULL);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_called >= 3);
    ck_assert(test_here_tracking_prod_recv_data_cb_status == HERE_TRACKING_OK);
    ck_assert(test_here_tracking_prod_recv_data_cb_size > 0);
    here_tracking_free(&client);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(client.tls == NULL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_prod_send_stream_invalid_token)
{
    here_tracking_client client;

    here_tracking_error res = here_tracking_init(&client,
                                                 HERE_TRACKING_TEST_DEVICE_ID,
                                                 HERE_TRACKING_TEST_DEVICE_SECRET,
                                                 base_url);

    ck_assert_int_eq(res, HERE_TRACKING_OK);
    client.user_agent = user_agent;
    res = here_tracking_auth(&client);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_uint_gt(strlen(client.access_token), 0);
    ck_assert_uint_gt(client.token_expiry, 0);
    memset(client.access_token, 'A', strlen(client.access_token));
    INIT_RECV_CB_DATA;
    res = here_tracking_send_stream(&client,
                              test_here_tracking_prod_send_ok_cb,
                              test_here_tracking_prod_recv_error_cb,
                              HERE_TRACKING_REQ_DATA_JSON,
                              HERE_TRACKING_RESP_WITH_DATA_JSON,
                              NULL);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_uint_ge(test_here_tracking_prod_recv_data_cb_called, 3);
    ck_assert_int_eq(test_here_tracking_prod_recv_data_cb_status, HERE_TRACKING_ERROR_UNAUTHORIZED);
    ck_assert_uint_gt(test_here_tracking_prod_recv_data_cb_size, 0);
    ck_assert_str_eq(client.access_token, "");
    ck_assert_uint_eq(client.token_expiry, 0);
    here_tracking_free(&client);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_ptr_eq(client.tls, NULL);
}
END_TEST

/**************************************************************************************************/

Suite* test_here_tracking_prod_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_prod_send_ok);
    tcase_add_test(tc, test_here_tracking_prod_adjust_time_diff);
    tcase_add_test(tc, test_here_tracking_prod_send_ok_no_auth);
    tcase_add_test(tc, test_here_tracking_prod_send_ok_expired_token);
    tcase_add_test(tc, test_here_tracking_prod_send_ok_expiry_offset_token);
    tcase_add_test(tc, test_here_tracking_prod_send_invalid_data);
    tcase_add_test(tc, test_here_tracking_prod_send_stream_ok);
    tcase_add_test(tc, test_here_tracking_prod_send_stream_invalid_token);
    suite_add_tcase(s, tc);
    return s;
}

#endif /* defined HERE_TRACKING_TEST_DEVICE_ID && defined HERE_TRACKING_TEST_DEVICE_SECRET */

/**************************************************************************************************/

int main()
{
    int failed = 0;

#if defined HERE_TRACKING_TEST_DEVICE_ID && defined HERE_TRACKING_TEST_DEVICE_SECRET
    SRunner* sr = srunner_create(test_here_tracking_prod_suite());
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
#endif /* defined HERE_TRACKING_TEST_DEVICE_ID && defined HERE_TRACKING_TEST_DEVICE_SECRET */

    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
