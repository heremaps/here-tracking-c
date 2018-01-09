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
#include <fff.h>

#include "here_tracking_time.h"

#define TEST_NAME "here_tracking_time"

DEFINE_FFF_GLOBALS;
FAKE_VALUE_FUNC1(time_t, time, time_t*);

/**************************************************************************************************/

START_TEST(test_here_tracking_time_ok)
{
    uint32_t ts;
    time_fake.return_val = 2000;
    here_tracking_error res = here_tracking_get_unixtime(&ts);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(ts == 2000);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_time_fail)
{
    uint32_t ts;
    time_fake.return_val = -1;
    here_tracking_error res = here_tracking_get_unixtime(&ts);
    ck_assert(res == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_time_invalid_input)
{
    here_tracking_error res = here_tracking_get_unixtime(NULL);
    ck_assert(res == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

Suite* test_here_tracking_time_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_time_ok);
    tcase_add_test(tc, test_here_tracking_time_fail);
    tcase_add_test(tc, test_here_tracking_time_invalid_input);
    suite_add_tcase(s, tc);
    return s;
}

/**************************************************************************************************/

int main()
{
    int failed;
    SRunner* sr = srunner_create(test_here_tracking_time_suite());
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
