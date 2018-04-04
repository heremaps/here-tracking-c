/**************************************************************************************************
 * Copyright (C) 2017-2018 HERE Europe B.V.                                                       *
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

#include <fff.h>

#include "here_tracking_test.h"
#include "here_tracking_tls_writer.h"

#include "mock_here_tracking_data_buffer.h"
#include "mock_here_tracking_tls.h"

#define TEST_NAME "here_tracking_tls_writer"

/**************************************************************************************************/

DEFINE_FFF_GLOBALS;

#define TEST_HERE_TRACKING_TLS_WRITER_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_DATA_BUFFER_FAKE_LIST(FAKE) \
    MOCK_HERE_TRACKING_TLS_FAKE_LIST(FAKE)

/**************************************************************************************************/

#define TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(WRITER, TLS, BUFFER, BUFFER_SIZE) \
    err = here_tracking_tls_init(&(TLS)); \
    ck_assert(err == HERE_TRACKING_OK); \
    err = here_tracking_tls_writer_init((WRITER), (TLS), (BUFFER), (BUFFER_SIZE)); \
    ck_assert(err == HERE_TRACKING_OK);

/**************************************************************************************************/

void test_here_tracking_tls_writer_tc_setup(void)
{
    TEST_HERE_TRACKING_TLS_WRITER_FAKE_LIST(RESET_FAKE);
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
    here_tracking_tls_init_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_init_fake.custom_fake = mock_here_tracking_tls_init_custom;
    here_tracking_tls_connect_fake.return_val = HERE_TRACKING_OK;
    here_tracking_tls_write_fake.return_val = HERE_TRACKING_OK;
}

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_init_ok)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_data_chunks_ok)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    static const uint8_t buffer_size = 10;
    uint8_t buffer[buffer_size];
    static const uint8_t data_size = 15;
    uint8_t data[data_size];
    here_tracking_error add_data_res[3] =
    {
        HERE_TRACKING_OK,
        HERE_TRACKING_ERROR_BUFFER_TOO_SMALL,
        HERE_TRACKING_OK
    };

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, buffer_size);
    SET_RETURN_SEQ(here_tracking_data_buffer_add_data, add_data_res, 3);
    err = here_tracking_tls_writer_write_data(&tls_writer, data, buffer_size);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    /* Buffer should be flushed after write to full capacity. */
    ck_assert_uint_eq(HERE_TRACKING_DATA_BUFFER_BYTES_FREE(&tls_writer.data_buffer), buffer_size);
    err = here_tracking_tls_writer_write_data(&tls_writer, data, data_size);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    /* Buffer not filled to capacity, there should be data remaining in the write buffer */
    ck_assert_uint_lt(HERE_TRACKING_DATA_BUFFER_BYTES_FREE(&tls_writer.data_buffer), buffer_size);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_init_invalid_input)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];

    err = here_tracking_tls_init(&tls_ctx);
    ck_assert(err == HERE_TRACKING_OK);
    err = here_tracking_tls_writer_init(NULL, tls_ctx, buffer, 10);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_tls_writer_init(&tls_writer, NULL, buffer, 10);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_tls_writer_init(&tls_writer, tls_ctx, NULL, 10);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_tls_writer_init(&tls_writer, tls_ctx, buffer, 0);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_data_add_data_fail)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];
    uint8_t data[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    here_tracking_data_buffer_add_data_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_tls_writer_write_data(&tls_writer, data, 10);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_data_add_data_fail2)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    here_tracking_error add_data_res[2] =
    {
        HERE_TRACKING_ERROR_BUFFER_TOO_SMALL,
        HERE_TRACKING_ERROR
    };
    uint8_t buffer[10];
    uint8_t data[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    SET_RETURN_SEQ(here_tracking_data_buffer_add_data, add_data_res, 2);
    err = here_tracking_tls_writer_write_data(&tls_writer, data, 10);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_data_add_data_tls_write_fail)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    here_tracking_error add_data_res[2] =
    {
        HERE_TRACKING_ERROR_BUFFER_TOO_SMALL,
        HERE_TRACKING_OK
    };
    uint8_t buffer[5];
    uint8_t data[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 5);
    SET_RETURN_SEQ(here_tracking_data_buffer_add_data, add_data_res, 2);
    here_tracking_tls_write_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_tls_writer_write_data(&tls_writer, data, 10);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_data_invalid_input)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];
    uint8_t data[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    err = here_tracking_tls_writer_write_data(NULL, data, 10);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_tls_writer_write_data(&tls_writer, NULL, 10);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_tls_writer_write_data(&tls_writer, data, 0);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_string_invalid_input)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];
    char* data = "HELLO!";

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    err = here_tracking_tls_writer_write_string(NULL, data);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
    err = here_tracking_tls_writer_write_string(&tls_writer, NULL);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_utoa_invalid_input)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    err = here_tracking_tls_writer_write_utoa(NULL, 100, 10);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_utoa_data_buffer_init_fail)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    here_tracking_data_buffer_init_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_tls_writer_write_utoa(&tls_writer, 100, 10);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_utoa_data_buffer_utoa_fail)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    here_tracking_data_buffer_add_utoa_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_tls_writer_write_utoa(&tls_writer, 100, 10);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_write_utoa_write_data_fail)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    uint8_t buffer[10];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, 10);
    here_tracking_data_buffer_add_data_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_tls_writer_write_utoa(&tls_writer, 100, 10);
    ck_assert(err == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_flush_invalid_input)
{
    here_tracking_error err;

    err = here_tracking_tls_writer_flush(NULL);
    ck_assert(err == HERE_TRACKING_ERROR_INVALID_INPUT);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_tls_writer_flush_write_fail)
{
    here_tracking_error err;
    here_tracking_tls_writer tls_writer;
    here_tracking_tls tls_ctx;
    static const uint8_t buffer_size = 10;
    uint8_t buffer[buffer_size];
    uint8_t data[buffer_size - 1];

    TEST_HERE_TRACKING_TLS_WRITER_INIT_OK(&tls_writer, tls_ctx, buffer, buffer_size);
    err = here_tracking_tls_writer_write_data(&tls_writer, data, buffer_size - 1);
    ck_assert_int_eq(err, HERE_TRACKING_OK);
    here_tracking_tls_write_fake.return_val = HERE_TRACKING_ERROR;
    err = here_tracking_tls_writer_flush(&tls_writer);
    ck_assert_int_eq(err, HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

TEST_SUITE_BEGIN(TEST_NAME)
    TEST_SUITE_ADD_SETUP_TEARDOWN_FN(test_here_tracking_tls_writer_tc_setup, NULL)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_init_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_init_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_data_chunks_ok)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_data_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_data_add_data_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_data_add_data_fail2)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_data_add_data_tls_write_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_string_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_utoa_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_utoa_data_buffer_init_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_utoa_data_buffer_utoa_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_write_utoa_write_data_fail)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_flush_invalid_input)
    TEST_SUITE_ADD_TEST(test_here_tracking_tls_writer_flush_write_fail)
TEST_SUITE_END

/**************************************************************************************************/

TEST_MAIN(TEST_NAME)
