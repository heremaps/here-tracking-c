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

#include "here_tracking_base64.h"

#define TEST_NAME "here_tracking_base64_mbedtls_no_mock"

/**************************************************************************************************/

START_TEST(test_here_tracking_base64_mbedtls_no_mock_ok)
{
    /* Example from https://en.wikipedia.org/wiki/Base64#Examples */
    char dec[] = \
        "Man is distinguished, not only by his reason, but by this singular passion from "\
        "other animals, which is a lust of the mind, that by a perseverance of delight "\
        "in the continued and indefatigable generation of knowledge, exceeds the short "\
        "vehemence of any carnal pleasure.";
    char enc_expected[] = \
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"\
        "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"\
        "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"\
        "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"\
        "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";
    uint32_t enc_data_size = here_tracking_base64_enc_size((sizeof(dec) - 1));
    char* enc_data = malloc(enc_data_size);
    ck_assert(enc_data_size == (sizeof(enc_expected)));
    ck_assert(enc_data != NULL);
    here_tracking_error res = \
        here_tracking_base64_enc(dec, sizeof(dec) -1, enc_data, &enc_data_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(enc_data_size == (sizeof(enc_expected) - 1));
    ck_assert(memcmp(enc_data, enc_expected, enc_data_size) == 0);
    free(enc_data);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_base64_mbedtls_no_mock_err)
{
    char dec[] = "FOO!";
    char enc[] = "BAR!";
    uint32_t enc_size = sizeof(enc);
    here_tracking_error res = here_tracking_base64_enc(NULL, sizeof(dec) -1, enc, &enc_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_base64_enc(dec, 0, enc, &enc_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_base64_enc(dec, sizeof(dec) -1, NULL, &enc_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_base64_enc(dec, sizeof(dec) -1, enc, NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_base64_enc(dec, sizeof(dec) -1, enc, &enc_size);
    ck_assert(res == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
}
END_TEST

/**************************************************************************************************/

Suite* test_here_tracking_base64_mbedtls_no_mock_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_base64_mbedtls_no_mock_ok);
    tcase_add_test(tc, test_here_tracking_base64_mbedtls_no_mock_err);
    suite_add_tcase(s, tc);
    return s;
}

/**************************************************************************************************/

int main()
{
    int failed;
    SRunner* sr = srunner_create(test_here_tracking_base64_mbedtls_no_mock_suite());
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
