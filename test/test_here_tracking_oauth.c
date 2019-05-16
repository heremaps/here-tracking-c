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

#include "here_tracking_oauth.h"
#include "here_tracking_test.h"

#include "mock_here_tracking_data_buffer.h"
#include "mock_here_tracking_log.h"
#include "mock_here_tracking_time.h"

#define TEST_NAME "here_tracking_oauth"

/**************************************************************************************************/

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC4(here_tracking_error,
                 here_tracking_base64_enc,
                 const char*,
                 uint32_t,
                 char*,
                 uint32_t*);

FAKE_VALUE_FUNC6(here_tracking_error,
                 here_tracking_hmac_sha256,
                 const char*,
                 uint32_t,
                 const char*,
                 uint32_t,
                 char*,
                 uint32_t*);

#define TEST_HERE_TRACKING_OAUTH_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_DATA_BUFFER_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_LOG_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TIME_FAKE_LIST(FAKE) \
    FAKE(here_tracking_base64_enc) \
    FAKE(here_tracking_hmac_sha256)

/**************************************************************************************************/

static void test_here_tracking_oauth_setup()
{
    TEST_HERE_TRACKING_OAUTH_FAKE_LIST(RESET_FAKE);
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
    here_tracking_data_buffer_add_utoa_fake.return_val = HERE_TRACKING_OK;
    here_tracking_data_buffer_add_utoa_fake.custom_fake = \
        mock_here_tracking_data_buffer_add_utoa_custom;
    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_OK;
    here_tracking_get_unixtime_fake.custom_fake = mock_here_tracking_get_unixtime_custom;
}

/**************************************************************************************************/

here_tracking_error return_base64(const char* in, uint32_t in_size, char* out, uint32_t* out_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR;
    static const char* res = "9UOXxjR28bVrPv/vn7YEwflTNtC9UOQndD8npf4xLJc=";

    if(out != NULL && out_size != NULL && (*out_size) >= strlen(res))
    {
        memcpy(out, res, strlen(res));
        (*out_size) = strlen(res);
        err = HERE_TRACKING_OK;
    }

    return err;
}

/**************************************************************************************************/

here_tracking_error return_hmac_sha256(const char* msg,
                                       uint32_t msg_size,
                                       const char* secret,
                                       uint32_t secret_size,
                                       char* out,
                                       uint32_t* out_size)
{
    here_tracking_error err = HERE_TRACKING_ERROR;
    static const char res[] =
    {
        0xF5, 0x43, 0x97, 0xC6, 0x34, 0x76, 0xF1, 0xB5, 0x6B, 0x3E, 0xFF, 0xEF, 0x9F, 0xB6, 0x04,
        0xC1, 0xF9, 0x53, 0x36, 0xD0, 0xBD, 0x50, 0xE4, 0x27, 0x74, 0x3F, 0x27, 0xA5, 0xFE, 0x31,
        0x2C, 0x97
    };

    if(out != NULL && out_size != NULL && (*out_size) >= sizeof(res))
    {
        memcpy(out, res, sizeof(res));
        (*out_size) = sizeof(res);
        err = HERE_TRACKING_OK;
    }

    return err;
}

/**************************************************************************************************/

START_TEST(test_here_tracking_oauth_ok)
{
    static const char* device_id = "1b25138b-c795-4b20-a724-59a40162d8fd";
    static const char* device_secret = "Ohkai3eF-im5UGai4J-bIPizRburaiLohr4DQNE6cvM";
    static const char* base_url = "tracking.api.here.com";
    static const char* expected = \
        "OAuth oauth_consumer_key=\"1b25138b-c795-4b20-a724-59a40162d8fd\","\
        "oauth_nonce=\"4723056724\","\
        "oauth_signature_method=\"HMAC-SHA256\","\
        "oauth_timestamp=\"1234567890\","\
        "oauth_version=\"1.0\","\
        "oauth_signature=\"9UOXxjR28bVrPv%2Fvn7YEwflTNtC9UOQndD8npf4xLJc%3D\"";
    char oauth_hdr[HERE_TRACKING_OAUTH_MIN_OUT_SIZE];
    uint32_t oauth_hdr_size = HERE_TRACKING_OAUTH_MIN_OUT_SIZE;
    here_tracking_base64_enc_fake.custom_fake = return_base64;
    here_tracking_hmac_sha256_fake.custom_fake = return_hmac_sha256;
    mock_here_tracking_get_unixtime_set_result(1234567890);
    here_tracking_error res = here_tracking_oauth_create_header(device_id,
                                                                device_secret,
                                                                base_url,
                                                                0,
                                                                oauth_hdr,
                                                                &oauth_hdr_size);
    ck_assert_int_eq(res, HERE_TRACKING_OK);
    ck_assert_uint_eq(oauth_hdr_size, strlen(expected));
    ck_assert_int_eq(memcmp(oauth_hdr, expected, oauth_hdr_size), 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_oauth_invalid_input)
{
    static const char* device_id = "1b25138b-c795-4b20-a724-59a40162d8fd";
    static const char* device_secret = "Ohkai3eF-im5UGai4J-bIPizRburaiLohr4DQNE6cvM";
    static const char* base_url = "tracking.api.here.com";
    char oauth_hdr[HERE_TRACKING_OAUTH_MIN_OUT_SIZE];
    uint32_t oauth_hdr_size = HERE_TRACKING_OAUTH_MIN_OUT_SIZE;
    here_tracking_error res;
    res = here_tracking_oauth_create_header(NULL,
                                            device_secret,
                                            base_url,
                                            0,
                                            oauth_hdr,
                                            &oauth_hdr_size);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_oauth_create_header(device_id,
                                            NULL,
                                            base_url,
                                            0,
                                            oauth_hdr,
                                            &oauth_hdr_size);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_oauth_create_header(device_id,
                                            device_secret,
                                            NULL,
                                            0,
                                            oauth_hdr,
                                            &oauth_hdr_size);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_oauth_create_header(device_id,
                                            device_secret,
                                            base_url,
                                            0,
                                            NULL,
                                            &oauth_hdr_size);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_oauth_create_header(device_id,
                                            device_secret,
                                            base_url,
                                            0,
                                            oauth_hdr,
                                            NULL);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_INVALID_INPUT);
    oauth_hdr_size = HERE_TRACKING_OAUTH_MIN_OUT_SIZE - 1;
    res = here_tracking_oauth_create_header(device_id,
                                            device_secret,
                                            base_url,
                                            0,
                                            oauth_hdr,
                                            &oauth_hdr_size);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_oauth_error_add_utoa_fail)
{
    static const char* device_id = "1b25138b-c795-4b20-a724-59a40162d8fd";
    static const char* device_secret = "Ohkai3eF-im5UGai4J-bIPizRburaiLohr4DQNE6cvM";
    static const char* base_url = "tracking.api.here.com";
    char oauth_hdr[HERE_TRACKING_OAUTH_MIN_OUT_SIZE];
    uint32_t oauth_hdr_size = HERE_TRACKING_OAUTH_MIN_OUT_SIZE;
    here_tracking_base64_enc_fake.custom_fake = return_base64;
    here_tracking_hmac_sha256_fake.custom_fake = return_hmac_sha256;
    here_tracking_data_buffer_add_utoa_fake.return_val = HERE_TRACKING_ERROR;
    mock_here_tracking_get_unixtime_set_result(1234567890);
    here_tracking_error res = here_tracking_oauth_create_header(device_id,
                                                                device_secret,
                                                                base_url,
                                                                0,
                                                                oauth_hdr,
                                                                &oauth_hdr_size);
    ck_assert_int_eq(res, HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

TEST_SUITE_BEGIN(TEST_NAME)
    TEST_SUITE_ADD_SETUP_TEARDOWN_FN(test_here_tracking_oauth_setup, NULL)
    TEST_SUITE_ADD_TEST(test_here_tracking_oauth_ok);
    TEST_SUITE_ADD_TEST(test_here_tracking_oauth_invalid_input);
    TEST_SUITE_ADD_TEST(test_here_tracking_oauth_error_add_utoa_fail);
TEST_SUITE_END

/**************************************************************************************************/

TEST_MAIN(TEST_NAME)
