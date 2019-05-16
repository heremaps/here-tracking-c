/**************************************************************************************************
 * Copyright (C) 2018 HERE Europe B.V.                                                            *
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

#include <ctype.h>

#include "here_tracking_test.h"
#include "here_tracking_uuid_gen.h"

#include "mock_here_tracking_time.h"

#define TEST_NAME "here_tracking_uuid_gen"

/**************************************************************************************************/

DEFINE_FFF_GLOBALS;

#define TEST_HERE_TRACKING_UUID_GEN_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TIME_FAKE_LIST(FAKE)

/**************************************************************************************************/

static void test_here_tracking_uuid_gen_validate_uuid(char* buf)
{
    int i, pos;

    pos = 0;

    for(i = pos; i < (pos + 8); i++)
    {
        ck_assert(isxdigit(buf[pos]));
    }

    pos += 8;
    ck_assert(buf[pos++] == '-');

    for(i = pos; i < (pos + 4); i++)
    {
        ck_assert(isxdigit(buf[pos]));
    }

    pos += 4;
    ck_assert(buf[pos++] == '-');
    ck_assert(buf[pos++] == '4'); /* UUID Version */

    for(i = pos; i < (pos + 3); i++)
    {
        ck_assert(isxdigit(buf[pos]));
    }

    pos += 3;
    ck_assert(buf[pos++] == '-');
    ck_assert(buf[pos++] == 'a'); /* UUID Variant */

    for(i = pos; i < (pos + 3); i++)
    {
        ck_assert(isxdigit(buf[pos]));
    }

    pos += 3;
    ck_assert(buf[pos++] == '-');

    for(i = pos; i < (pos + 12); i++)
    {
        ck_assert(isxdigit(buf[pos]));
    }
}

/**************************************************************************************************/

void test_here_tracking_uuid_gen_tc_setup(void)
{
    TEST_HERE_TRACKING_UUID_GEN_FAKE_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_OK;
    here_tracking_get_unixtime_fake.custom_fake = mock_here_tracking_get_unixtime_custom;
}

/**************************************************************************************************/

void test_here_tracking_uuid_gen_tc_teardown(void)
{
}

/**************************************************************************************************/

START_TEST(test_here_tracking_uuid_gen_ok)
{
    here_tracking_error err;
    char buf[HERE_TRACKING_UUID_SIZE];

    err = here_tracking_uuid_gen_new(buf, sizeof(buf));
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    test_here_tracking_uuid_gen_validate_uuid(buf);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_uuid_gen_ok_unique)
{
    here_tracking_error err;
    char buf[HERE_TRACKING_UUID_SIZE];
    char buf2[HERE_TRACKING_UUID_SIZE];

    err = here_tracking_uuid_gen_new(buf, sizeof(buf));
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    test_here_tracking_uuid_gen_validate_uuid(buf);
    err = here_tracking_uuid_gen_new(buf2, sizeof(buf2));
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    test_here_tracking_uuid_gen_validate_uuid(buf2);
    ck_assert_str_ne(buf, buf2);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_uuid_gen_null_buffer)
{
    here_tracking_error err;

    err = here_tracking_uuid_gen_new(NULL, HERE_TRACKING_UUID_SIZE);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_uuid_gen_too_small_buffer)
{
    here_tracking_error err;
    char buf[HERE_TRACKING_UUID_SIZE - 1];

    err = here_tracking_uuid_gen_new(buf, sizeof(buf));
    ck_assert_int_eq(err, HERE_TRACKING_ERROR_BUFFER_TOO_SMALL);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_uuid_gen_get_unixtime_error)
{
    here_tracking_error err;
    char buf[HERE_TRACKING_UUID_SIZE];

    here_tracking_get_unixtime_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_uuid_gen_new(buf, sizeof(buf));
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

TEST_SUITE_BEGIN(TEST_NAME)
    TEST_SUITE_ADD_SETUP_TEARDOWN_FN(test_here_tracking_uuid_gen_tc_setup,
                                     test_here_tracking_uuid_gen_tc_teardown)
    TEST_SUITE_ADD_TEST(test_here_tracking_uuid_gen_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_uuid_gen_ok_unique)
    TEST_SUITE_ADD_TEST(test_here_tracking_uuid_gen_null_buffer)
    TEST_SUITE_ADD_TEST(test_here_tracking_uuid_gen_too_small_buffer)
    TEST_SUITE_ADD_TEST(test_here_tracking_uuid_gen_get_unixtime_error)
TEST_SUITE_END

/**************************************************************************************************/

TEST_MAIN(TEST_NAME)
