/**************************************************************************************************
* Copyright (C) 2018 HERE Europe B.V.                                                             *
* All rights reserved.                                                                            *
*                                                                                                 *
* MIT License                                                                                     *
* Permission is hereby granted, free of charge, to any person obtaining a copy                    *
* of this software and associated documentation files (the "Software"), to deal                   *
* in the Software without restriction, including without limitation the rights                    *
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell                       *
* copies of the Software, and to permit persons to whom the Software is                           *
* furnished to do so, subject to the following conditions:                                        *
*                                                                                                 *
* The above copyright notice and this permission notice shall be included in all                  *
* copies or substantial portions of the Software.                                                 *
*                                                                                                 *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR                      *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,                        *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE                     *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER                          *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,                   *
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE                   *
* SOFTWARE.                                                                                       *
**************************************************************************************************/

#include <stdlib.h>

#include <check.h>

#include "internal/here_tracking_http.h"
#include "internal/here_tracking_http_defs.h"
#include "here_tracking_test.h"

#define TEST_NAME "here_tracking_http_online"

/**************************************************************************************************/

typedef struct
{
    here_tracking_client client;
    uint32_t recv_cb_called;
    uint32_t recv_data_size;
    uint32_t recv_evt_resp_size_count;
    uint32_t recv_evt_resp_data_count;
    uint32_t recv_evt_resp_complete_count;
    here_tracking_error resp_complete_status;
} test_here_tracking_http_online_ctx;

/**************************************************************************************************/

static const char* device_id = "1b25138b-c795-4b20-a724-59a40162d8fd";
static const char* device_secret = "Ohkai3eF-im5UGai4J-bIPizRburaiLohr4DQNE6cvM";
static const char* base_url = "tracking.api.here.com";

/**************************************************************************************************/

static here_tracking_error \
    test_here_tracking_http_online_recv_ok_cb(const here_tracking_recv_data* data,
                                              void* user_data)
{
    test_here_tracking_http_online_ctx* test_ctx;

    ck_assert_ptr_ne(user_data, NULL);
    test_ctx = (test_here_tracking_http_online_ctx*)user_data;

    if(data->evt == HERE_TRACKING_RECV_EVT_RESP_SIZE)
    {
        ck_assert_int_eq(data->err, HERE_TRACKING_OK);
        ck_assert_uint_gt(data->data_size, 0);
        test_ctx->recv_evt_resp_size_count++;
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_DATA)
    {
        ck_assert_int_eq(data->err, HERE_TRACKING_OK);
        ck_assert_uint_gt(data->data_size, 0);
        ck_assert_ptr_ne(data->data, NULL);
        test_ctx->recv_data_size += data->data_size;
        test_ctx->recv_evt_resp_data_count++;
    }
    else if(data->evt == HERE_TRACKING_RECV_EVT_RESP_COMPLETE)
    {
        test_ctx->recv_evt_resp_complete_count++;
        test_ctx->resp_complete_status = data->err;
    }

    test_ctx->recv_cb_called++;
    return HERE_TRACKING_OK;
}

/**************************************************************************************************/

START_TEST(test_here_tracking_http_online_get_ok)
{
    test_here_tracking_http_online_ctx test_ctx = {0};
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.here.com";
    char* path = "/index.html";
    char* header_accept_name = "Accept";
    char* header_accept_value = "*/*";
    here_tracking_http_header header[2];
    uint16_t port = 443;

    err = here_tracking_init(&test_ctx.client, device_id, device_secret, base_url);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    header[0].name = header_accept_name;
    header[0].value = header_accept_value;
    header[1].name = (char*)here_tracking_http_header_connection;
    header[1].value = (char*)here_tracking_http_connection_close;
    request.host = host;
    request.path = path;
    request.port = port;
    request.header_count = 2;
    request.headers = &(header[0]);
    err = here_tracking_http_get(&test_ctx.client,
                                 &request,
                                 test_here_tracking_http_online_recv_ok_cb,
                                 &test_ctx);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_size_count, 1);
    ck_assert_uint_ge(test_ctx.recv_evt_resp_data_count, 1);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_complete_count, 1);
    ck_assert_uint_ge(test_ctx.recv_cb_called, 3);
    ck_assert_int_eq(test_ctx.resp_complete_status, HERE_TRACKING_OK);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_online_get_not_found)
{
    test_here_tracking_http_online_ctx test_ctx = {0};
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.api.here.com";
    char* path = "/";
    char* header_accept_name = "Accept";
    char* header_accept_value = "*/*";
    here_tracking_http_header header[2];
    uint16_t port = 443;

    err = here_tracking_init(&test_ctx.client, device_id, device_secret, base_url);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    header[0].name = header_accept_name;
    header[0].value = header_accept_value;
    header[1].name = (char*)here_tracking_http_header_connection;
    header[1].value = (char*)here_tracking_http_connection_close;
    request.host = host;
    request.path = path;
    request.port = port;
    request.header_count = 2;
    request.headers = &(header[0]);
    err = here_tracking_http_get(&test_ctx.client,
                                 &request,
                                 test_here_tracking_http_online_recv_ok_cb,
                                 &test_ctx);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_size_count, 1);
    ck_assert_uint_ge(test_ctx.recv_evt_resp_data_count, 1);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_complete_count, 1);
    ck_assert_uint_ge(test_ctx.recv_cb_called, 3);
    ck_assert_int_eq(test_ctx.resp_complete_status, HERE_TRACKING_ERROR_NOT_FOUND);
}
END_TEST

/**************************************************************************************************/

#if defined HERE_TRACKING_TEST_DEVICE_ID && defined HERE_TRACKING_TEST_DEVICE_SECRET

START_TEST(test_here_tracking_http_online_get_with_auth_forbidden)
{
    test_here_tracking_http_online_ctx test_ctx = {0};
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.api.here.com";
    char path[100] = {0};
    char* path_base = "/registry/v2/devices/";
    here_tracking_http_header header[2];
    uint16_t port = 443;

    err = here_tracking_init(&test_ctx.client,
                             HERE_TRACKING_TEST_DEVICE_ID,
                             HERE_TRACKING_TEST_DEVICE_SECRET,
                             base_url);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    err = here_tracking_auth(&test_ctx.client);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    strcat(path, path_base);
    strncat(path, HERE_TRACKING_TEST_DEVICE_ID, HERE_TRACKING_DEVICE_ID_SIZE);
    header[0].name = (char*)here_tracking_http_header_authorization;
    header[0].value = test_ctx.client.access_token;
    header[1].name = (char*)here_tracking_http_header_connection;
    header[1].value = (char*)here_tracking_http_connection_close;
    request.host = host;
    request.path = path;
    request.port = port;
    request.header_count = 2;
    request.headers = &(header[0]);
    err = here_tracking_http_get(&test_ctx.client,
                                 &request,
                                 test_here_tracking_http_online_recv_ok_cb,
                                 &test_ctx);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_size_count, 1);
    ck_assert_uint_ge(test_ctx.recv_evt_resp_data_count, 1);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_complete_count, 1);
    ck_assert_uint_ge(test_ctx.recv_cb_called, 3);
    ck_assert_int_eq(test_ctx.resp_complete_status, HERE_TRACKING_ERROR_FORBIDDEN);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_online_get_with_auth_bearer_forbidden)
{
    test_here_tracking_http_online_ctx test_ctx = {0};
    here_tracking_error err;
    here_tracking_http_request request;
    char* host = "tracking.api.here.com";
    char path[100] = {0};
    char auth_header_value[1000] = {0};
    char* path_base = "/registry/v2/devices/";
    here_tracking_http_header header[2];
    uint16_t port = 443;

    err = here_tracking_init(&test_ctx.client,
                             HERE_TRACKING_TEST_DEVICE_ID,
                             HERE_TRACKING_TEST_DEVICE_SECRET,
                             base_url);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    err = here_tracking_auth(&test_ctx.client);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    strcat(path, path_base);
    strncat(path, HERE_TRACKING_TEST_DEVICE_ID, HERE_TRACKING_DEVICE_ID_SIZE);
    strcat(auth_header_value, "Bearer ");
    strcat(auth_header_value, test_ctx.client.access_token);
    header[0].name = (char*)here_tracking_http_header_authorization;
    header[0].value = auth_header_value;
    header[1].name = (char*)here_tracking_http_header_connection;
    header[1].value = (char*)here_tracking_http_connection_close;
    request.host = host;
    request.path = path;
    request.port = port;
    request.header_count = 2;
    request.headers = &(header[0]);
    err = here_tracking_http_get(&test_ctx.client,
                                 &request,
                                 test_here_tracking_http_online_recv_ok_cb,
                                 &test_ctx);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_size_count, 1);
    ck_assert_uint_ge(test_ctx.recv_evt_resp_data_count, 1);
    ck_assert_uint_eq(test_ctx.recv_evt_resp_complete_count, 1);
    ck_assert_uint_ge(test_ctx.recv_cb_called, 3);
    ck_assert_int_eq(test_ctx.resp_complete_status, HERE_TRACKING_ERROR_FORBIDDEN);
}
END_TEST

#endif

/**************************************************************************************************/

TEST_SUITE_BEGIN(TEST_NAME)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_online_get_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_online_get_not_found)
#if defined HERE_TRACKING_TEST_DEVICE_ID && defined HERE_TRACKING_TEST_DEVICE_SECRET
    TEST_SUITE_ADD_TEST(test_here_tracking_http_online_get_with_auth_forbidden)
    TEST_SUITE_ADD_TEST(test_here_tracking_http_online_get_with_auth_bearer_forbidden)
#endif
TEST_SUITE_END

/**************************************************************************************************/

TEST_MAIN(TEST_NAME)
