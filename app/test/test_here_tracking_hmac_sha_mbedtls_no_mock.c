/**************************************************************************************************
* Copyright (C) 2017 HERE Europe B.V.                                                             *
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

#include "here_tracking_hmac_sha.h"

#define TEST_NAME "here_tracking_hmac_sha_mbedtls_no_mock"

/**************************************************************************************************/

START_TEST(test_here_tracking_hmac_sha_mbedtls_no_mock_ok)
{
    /* Example from https://en.wikipedia.org/wiki/Hash-based_message_authentication_code#Examples */
    char msg[] = "The quick brown fox jumps over the lazy dog";
    char secret[] = "key";
    unsigned char expected[HERE_TRACKING_HMAC_SHA256_OUT_SIZE] =
    {
        0xF7, 0xBC, 0x83, 0xF4, 0x30, 0x53, 0x84, 0x24, 0xB1, 0x32, 0x98, 0xE6, 0xAA, 0x6F, 0xB1,
        0x43, 0xEF, 0x4D, 0x59, 0xA1, 0x49, 0x46, 0x17, 0x59, 0x97, 0x47, 0x9D, 0xBC, 0x2D, 0x1A,
        0x3C, 0xD8
    };
    char out[HERE_TRACKING_HMAC_SHA256_OUT_SIZE];
    uint32_t out_size = sizeof(out);
    here_tracking_error res = here_tracking_hmac_sha256(msg,
                                                        sizeof(msg) - 1,
                                                        secret,
                                                        sizeof(secret) - 1,
                                                        out,
                                                        &out_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(out_size == HERE_TRACKING_HMAC_SHA256_OUT_SIZE);
    ck_assert(memcmp(out, expected, HERE_TRACKING_HMAC_SHA256_OUT_SIZE) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_hmac_sha_mbedtls_no_mock_oauth_ok)
{
    char msg[] = "POST&https%3A%2F%2Ftracking.api.here.com%2Fv2%2Ftoken&"\
        "oauth_consumer_key%3D1b25138b-c795-4b20-a724-59a40162d8fd%26oauth_nonce%3D4723056724%26"\
        "oauth_signature_method%3DHMAC-SHA256%26oauth_timestamp%3D1234567890%26oauth_version%3D1.0";
    char secret[] = "Ohkai3eF-im5UGai4J-bIPizRburaiLohr4DQNE6cvM&";
    unsigned char expected[HERE_TRACKING_HMAC_SHA256_OUT_SIZE] =
    {
        0x50, 0xC9, 0x70, 0x44, 0x22, 0x23, 0x19, 0xC3, 0xBE, 0xA7, 0x25, 0x94, 0x63, 0x38, 0x92,
        0x98, 0x92, 0xFD, 0x5B, 0x4B, 0x9E, 0x3F, 0x45, 0x2B, 0x52, 0xD4, 0x86, 0x73, 0xD4, 0x1E,
        0x7B, 0x61
    };
    char out[HERE_TRACKING_HMAC_SHA256_OUT_SIZE];
    uint32_t out_size = sizeof(out);
    here_tracking_error res = here_tracking_hmac_sha256(msg,
                                                        sizeof(msg) - 1,
                                                        secret,
                                                        sizeof(secret) - 1,
                                                        out,
                                                        &out_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(out_size == HERE_TRACKING_HMAC_SHA256_OUT_SIZE);
    ck_assert(memcmp(out, expected, HERE_TRACKING_HMAC_SHA256_OUT_SIZE) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_hmac_sha_mbedtls_no_mock_err)
{
    char msg[] = "MSG";
    char secret[] = "SECRET";
    char out[HERE_TRACKING_HMAC_SHA256_OUT_SIZE];
    uint32_t out_size;
    here_tracking_error res = here_tracking_hmac_sha256(NULL,
                                                        sizeof(msg) - 1,
                                                        secret,
                                                        sizeof(secret) - 1,
                                                        out,
                                                        &out_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_hmac_sha256(msg,
                                    0,
                                    secret,
                                    sizeof(secret) - 1,
                                    out,
                                    &out_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_hmac_sha256(msg,
                                    sizeof(msg) - 1,
                                    NULL,
                                    sizeof(secret) - 1,
                                    out,
                                    &out_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_hmac_sha256(msg,
                                    sizeof(msg) - 1,
                                    secret,
                                    0,
                                    out,
                                    &out_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_hmac_sha256(msg,
                                    sizeof(msg) - 1,
                                    secret,
                                    sizeof(secret) - 1,
                                    NULL,
                                    &out_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_hmac_sha256(msg,
                                    sizeof(msg) - 1,
                                    secret,
                                    sizeof(secret) - 1,
                                    out,
                                    NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    out_size = 3;
    res = here_tracking_hmac_sha256(msg,
                                    sizeof(msg) - 1,
                                    secret,
                                    sizeof(secret) - 1,
                                    out,
                                    &out_size);
    ck_assert(res == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
}
END_TEST

/**************************************************************************************************/

Suite* test_here_tracking_hmac_sha_mbedtls_no_mock_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_hmac_sha_mbedtls_no_mock_ok);
    tcase_add_test(tc, test_here_tracking_hmac_sha_mbedtls_no_mock_oauth_ok);
    tcase_add_test(tc, test_here_tracking_hmac_sha_mbedtls_no_mock_err);
    suite_add_tcase(s, tc);
    return s;
}

/**************************************************************************************************/

int main()
{
    int failed;
    SRunner* sr = srunner_create(test_here_tracking_hmac_sha_mbedtls_no_mock_suite());
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
