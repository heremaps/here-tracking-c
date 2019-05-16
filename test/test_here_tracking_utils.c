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

#include "here_tracking_test.h"
#include "here_tracking_utils.h"

#define TEST_NAME "here_tracking_utils"

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_memcasecmp_lc)
{
    static char* t1 = "abcd";
    static char* t2 = "abce";
    int32_t res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t1, strlen(t1));
    ck_assert_int_eq(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t2, strlen(t1));
    ck_assert_int_lt(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t1, strlen(t2));
    ck_assert_int_gt(res, 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_memcasecmp_uc)
{
    static char* t1 = "ABCD";
    static char* t2 = "ABCE";
    int32_t res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t1, strlen(t1));
    ck_assert_int_eq(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t2, strlen(t1));
    ck_assert_int_lt(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t1, strlen(t2));
    ck_assert_int_gt(res, 0);
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
    ck_assert_int_eq(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t3, strlen(t1));
    ck_assert_int_eq(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t3, strlen(t2));
    ck_assert_int_eq(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t1, (uint8_t*)t4, strlen(t1));
    ck_assert_int_lt(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t4, (uint8_t*)t1, strlen(t2));
    ck_assert_int_gt(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t2, (uint8_t*)t4, strlen(t2));
    ck_assert_int_lt(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t4, (uint8_t*)t2, strlen(t2));
    ck_assert_int_gt(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t3, (uint8_t*)t4, strlen(t3));
    ck_assert_int_lt(res, 0);
    res = here_tracking_utils_memcasecmp((uint8_t*)t4, (uint8_t*)t3, strlen(t3));
    ck_assert_int_gt(res, 0);
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

START_TEST(test_here_tracking_utils_strcasecmp)
{
    static char* str1 = "User-Agent";
    static char* str2 = "user-agent";
    static char* str3 = "uSeR-AgEnT";
    static char* str4 = "UserAgent";
    ck_assert_int_eq(here_tracking_utils_strcasecmp(str1, str2), 0);
    ck_assert_int_eq(here_tracking_utils_strcasecmp(str1, str3), 0);
    ck_assert_int_eq(here_tracking_utils_strcasecmp(str2, str3), 0);
    ck_assert_int_ne(here_tracking_utils_strcasecmp(str1, str4), 0);
    ck_assert_int_ne(here_tracking_utils_strcasecmp(str2, str4), 0);
    ck_assert_int_ne(here_tracking_utils_strcasecmp(str3, str4), 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_atoi)
{
    static char* str1 = "123456789";
    static char* str2 = "987654321";
    static char* str3 = "-123456789";
    static char* str4 = "ABCDEFG";
    static char* str5 = "123ABC";
    ck_assert_uint_eq(here_tracking_utils_atoi(str1, strlen(str1)), 123456789);
    ck_assert_uint_eq(here_tracking_utils_atoi(str2, strlen(str2)), 987654321);
    ck_assert_uint_eq(here_tracking_utils_atoi(str3, strlen(str3)), -123456789);
    ck_assert_uint_eq(here_tracking_utils_atoi(str4, strlen(str4)), 0);
    ck_assert_uint_eq(here_tracking_utils_atoi(str5, strlen(str5)), 123);
    ck_assert_uint_eq(here_tracking_utils_atoi(NULL, strlen(str1)), 0);
    ck_assert_uint_eq(here_tracking_utils_atoi(str3, strlen(str3) - 2), -1234567);
    ck_assert_uint_eq(here_tracking_utils_atoi(str1, 0), 0);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_utils_atou)
{
    static char* str1 = "123456789";
    static char* str2 = "987654321";
    static char* str3 = "-123456789";
    static char* str4 = "ABCDEFG";
    static char* str5 = "123ABC";
    ck_assert_uint_eq(here_tracking_utils_atou(str1, strlen(str1)), 123456789);
    ck_assert_uint_eq(here_tracking_utils_atou(str2, strlen(str2)), 987654321);
    ck_assert_uint_eq(here_tracking_utils_atou(str3, strlen(str3)), 0);
    ck_assert_uint_eq(here_tracking_utils_atou(str4, strlen(str4)), 0);
    ck_assert_uint_eq(here_tracking_utils_atou(str5, strlen(str5)), 123);
    ck_assert_uint_eq(here_tracking_utils_atou(NULL, strlen(str1)), 0);
    ck_assert_uint_eq(here_tracking_utils_atou(str1, strlen(str1) - 2), 1234567);
    ck_assert_uint_eq(here_tracking_utils_atou(str1, 0), 0);
}
END_TEST

/**************************************************************************************************/

TEST_SUITE_BEGIN(TEST_NAME)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_memcasecmp_lc)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_memcasecmp_uc)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_memcasecmp_mc)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_isalnum)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_isalpha)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_isdigit)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_strcasecmp)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_atoi)
    TEST_SUITE_ADD_TEST(test_here_tracking_utils_atou)
TEST_SUITE_END

/**************************************************************************************************/

TEST_MAIN(TEST_NAME)
