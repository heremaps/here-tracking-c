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

#include "here_tracking_data_buffer.h"

#define TEST_NAME "here_tracking_data_buffer"

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_init_ok)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 10);
    ck_assert(res == HERE_TRACKING_OK);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_init_invalid_input)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(NULL, buf, 10);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_data_buffer_init(&data_buf, NULL, 10);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_data_buffer_init(&data_buf, buf, 0);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_char_ok)
{
    here_tracking_data_buffer data_buf;
    char buf[3];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 3);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_char(&data_buf, 'A');
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_char(&data_buf, 'B');
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_char(&data_buf, 'C');
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(buf[0] == 'A');
    ck_assert(buf[1] == 'B');
    ck_assert(buf[2] == 'C');
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_char_invalid_input)
{
    here_tracking_error res = here_tracking_data_buffer_add_char(NULL, 'A');
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_char_buffer_full)
{
    here_tracking_data_buffer data_buf;
    char buf[2];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 2);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_char(&data_buf, 'A');
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_char(&data_buf, 'B');
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_char(&data_buf, 'C');
    ck_assert(res == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
    ck_assert(buf[0] == 'A');
    ck_assert(buf[1] == 'B');
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_string_ok)
{
    here_tracking_data_buffer data_buf;
    char buf[6];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 6);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_string(&data_buf, "ABC");
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_string(&data_buf, "DEF");
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(memcmp(buf, "ABC", 3) == 0);
    ck_assert(memcmp(buf + 3, "DEF", 3) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_string_invalid_input)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 10);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_string(NULL, "ABC");
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_data_buffer_add_string(&data_buf, NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_string_buffer_full)
{
    here_tracking_data_buffer data_buf;
    char buf[3];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 3);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_string(&data_buf, "ABC");
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_string(&data_buf, "DEF");
    ck_assert(res == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
    ck_assert(memcmp(buf, "ABC", 3) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_data_ok)
{
    here_tracking_data_buffer data_buf;
    char buf[6];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 6);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_data(&data_buf, "ABC", 3);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_data(&data_buf, "DEF", 3);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(memcmp(buf, "ABC", 3) == 0);
    ck_assert(memcmp(buf + 3, "DEF", 3) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_data_invalid_input)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 10);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_data(NULL, "ABC", 3);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_data_buffer_add_data(&data_buf, NULL, 3);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_data_buffer_add_data(&data_buf, "ABC", 0);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_data_buffer_full)
{
    here_tracking_data_buffer data_buf;
    char buf[3];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 3);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_data(&data_buf, "ABC", 3);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_data(&data_buf, "DEF", 3);
    ck_assert(res == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
    ck_assert(memcmp(buf, "ABC", 3) == 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_utoa_ok)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 10);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_utoa(&data_buf, 54321);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(memcmp(buf, "54321", 5) == 0);
    ck_assert(data_buf.buffer_size == 5);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_utoa_ok_max)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 10);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_utoa(&data_buf, 4294967295);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(memcmp(buf, "4294967295", 10) == 0);
    ck_assert(data_buf.buffer_size == 10);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_utoa_ok_zero)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 10);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_utoa(&data_buf, 0);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(memcmp(buf, "0", 1) == 0);
    ck_assert(data_buf.buffer_size == 1);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_utoa_invalid_input)
{
    here_tracking_data_buffer data_buf;
    char buf[10];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 10);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_utoa(NULL, 54321);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_data_buffer_add_utoa_buffer_full)
{
    here_tracking_data_buffer data_buf;
    char buf[4];
    here_tracking_error res = here_tracking_data_buffer_init(&data_buf, buf, 4);
    ck_assert(res == HERE_TRACKING_OK);
    res = here_tracking_data_buffer_add_utoa(&data_buf, 54321);
    ck_assert(res == HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
}
END_TEST

/**************************************************************************************************/

Suite* test_here_tracking_data_buffer_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_data_buffer_init_ok);
    tcase_add_test(tc, test_here_tracking_data_buffer_init_invalid_input);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_char_ok);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_char_invalid_input);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_char_buffer_full);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_string_ok);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_string_invalid_input);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_string_buffer_full);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_data_ok);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_data_invalid_input);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_data_buffer_full);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_utoa_ok);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_utoa_ok_max);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_utoa_ok_zero);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_utoa_invalid_input);
    tcase_add_test(tc, test_here_tracking_data_buffer_add_utoa_buffer_full);
    suite_add_tcase(s, tc);
    return s;
}

/**************************************************************************************************/

int main()
{
    int failed;
    SRunner* sr = srunner_create(test_here_tracking_data_buffer_suite());
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
