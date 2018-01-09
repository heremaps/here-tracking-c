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

#include "here_tracking_utils.h"

#define TEST_NAME "here_tracking_utils"

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_memcasecmp_lc)
{
    static char* t1 = "abcd";
    static char* t2 = "abce";
    int32_t res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t1, strlen(t1));
    ck_assert(res == 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t2, strlen(t1));
    ck_assert(res < 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t1, strlen(t2));
    ck_assert(res > 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_memcasecmp_uc)
{
    static char* t1 = "ABCD";
    static char* t2 = "ABCE";
    int32_t res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t1, strlen(t1));
    ck_assert(res == 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t2, strlen(t1));
    ck_assert(res < 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t1, strlen(t2));
    ck_assert(res > 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_memcasecmp_mc)
{
    static char* t1 = "AbcD";
    static char* t2 = "aBCd";
    static char* t3 = "AbCd";
    static char* t4 = "AcBd";
    int32_t res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t2, strlen(t1));
    ck_assert(res == 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t3, strlen(t1));
    ck_assert(res == 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t3, strlen(t2));
    ck_assert(res == 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t4, strlen(t1));
    ck_assert(res < 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t4, (uint8_t*)t1, strlen(t2));
    ck_assert(res > 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t4, strlen(t2));
    ck_assert(res < 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t4, (uint8_t*)t2, strlen(t2));
    ck_assert(res > 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t3, (uint8_t*)t4, strlen(t3));
    ck_assert(res < 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t4, (uint8_t*)t3, strlen(t3));
    ck_assert(res > 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_isalnum)
{
    ck_assert(!here_tracking_utils_isalnum('+'));
    ck_assert(!here_tracking_utils_isalnum('?'));
    ck_assert(!here_tracking_utils_isalnum('_'));
    ck_assert(!here_tracking_utils_isalnum('|'));
    ck_assert(here_tracking_utils_isalnum('A'));
    ck_assert(here_tracking_utils_isalnum('a'));
    ck_assert(here_tracking_utils_isalnum('0'));
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_isalpha)
{
    ck_assert(!here_tracking_utils_isalpha('+'));
    ck_assert(!here_tracking_utils_isalpha('?'));
    ck_assert(!here_tracking_utils_isalpha('_'));
    ck_assert(!here_tracking_utils_isalpha('|'));
    ck_assert(!here_tracking_utils_isalpha('0'));
    ck_assert(here_tracking_utils_isalpha('A'));
    ck_assert(here_tracking_utils_isalpha('a'));
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_isdigit)
{
    ck_assert(!here_tracking_utils_isdigit('+'));
    ck_assert(!here_tracking_utils_isdigit('?'));
    ck_assert(!here_tracking_utils_isdigit('_'));
    ck_assert(!here_tracking_utils_isdigit('|'));
    ck_assert(!here_tracking_utils_isdigit('A'));
    ck_assert(here_tracking_utils_isdigit('0'));
}
END_TEST

/**************************************************************************************************/

Suite* test_here_tracking_utils_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_utils_memcasecmp_lc);
    tcase_add_test(tc, test_here_tracking_utils_memcasecmp_uc);
    tcase_add_test(tc, test_here_tracking_utils_memcasecmp_mc);
    tcase_add_test(tc, test_here_tracking_utils_isalnum);
    tcase_add_test(tc, test_here_tracking_utils_isalpha);
    tcase_add_test(tc, test_here_tracking_utils_isdigit);
    suite_add_tcase(s, tc);
    return s;
}

/**************************************************************************************************/

int main()
{
    int failed;
    SRunner* sr = srunner_create(test_here_tracking_utils_suite());
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
