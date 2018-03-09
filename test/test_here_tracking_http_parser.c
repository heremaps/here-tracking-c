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

#include <stdlib.h>

#include <check.h>

#include "here_tracking_http_parser.h"

#define TEST_NAME "here_tracking_http_parser"

const char* here_tracking_http_crlf = "\r\n";
const char* here_tracking_http_header_content_length = "Content-Length";

static const char* simple_resp = \
        "HTTP/1.1 200 OK\r\n"\
        "Content-Length: 12\r\n"\
        "Server: HERE_TRACKING_TEST/1.0.0\r\n"\
        "\r\n"\
        "HELLO WORLD!";

static const char* simple_resp_no_content_length = \
        "HTTP/1.1 200 OK\r\n"\
        "Server: HERE_TRACKING_TEST/1.0.0\r\n"\
        "\r\n"\
        "HELLO WORLD!";

static const char* simple_resp_zero_content_length = \
        "HTTP/2.15 404 Not Found\r\n"\
        "Content-Length: 0\r\n"\
        "Server: HERE_TRACKING_TEST/1.0.0\r\n"\
        "\r\n";

static const char* simple_resp_no_content = \
        "HTTP/1.1 204 No Content\r\n"\
        "\r\n";

/**************************************************************************************************/

static bool test_here_tracking_http_parser_cb(const here_tracking_http_parser_evt* evt,
                                              bool last,
                                              void* cb_data)
{
    uint32_t* cb_count = (uint32_t*)cb_data;
    (*cb_count)++;

    switch(evt->id)
    {
        case HERE_TRACKING_HTTP_PARSER_EVT_VERSION:
        {
            ck_assert((*cb_count) == 1);
            ck_assert(evt->data.version.major == 1);
            ck_assert(evt->data.version.minor == 1);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE:
        {
            ck_assert((*cb_count) == 2);
            ck_assert(evt->data.status_code == 200);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_REASON:
        {
            ck_assert((*cb_count == 3));
            ck_assert(evt->data.reason.buffer_size == 2);
            ck_assert(memcmp(evt->data.reason.buffer, "OK", 2) == 0);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_HDR:
        {
            ck_assert((*cb_count) == 4 || (*cb_count) == 6);

            if((*cb_count) == 4)
            {
                ck_assert(evt->data.hdr.hdr_key_size == 14);
                ck_assert(memcmp(evt->data.hdr.hdr_key,
                                 here_tracking_http_header_content_length,
                                 14) == 0);
                ck_assert(evt->data.hdr.hdr_val_size == 2);
                ck_assert(memcmp(evt->data.hdr.hdr_val, "12", 2) == 0);
                ck_assert(last);
            }
            else if((*cb_count) == 6)
            {
                ck_assert(evt->data.hdr.hdr_key_size == 6);
                ck_assert(memcmp(evt->data.hdr.hdr_key, "Server", 6) == 0);
                ck_assert(evt->data.hdr.hdr_val_size == 24);
                ck_assert(memcmp(evt->data.hdr.hdr_val, "HERE_TRACKING_TEST/1.0.0", 24) == 0);
                ck_assert(last);
            }
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
        {
            ck_assert((*cb_count) == 7);
            ck_assert(evt->data.body.buffer_size == 12);
            ck_assert(memcmp(evt->data.body.buffer, "HELLO WORLD!", 12) == 0);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY_SIZE:
        {
            ck_assert((*cb_count) == 5);
            ck_assert(evt->data.body_size == 12);
        }
        break;

        default:
            break;
    }

    return false;
}

/**************************************************************************************************/

static bool test_here_tracking_http_parser_multi_cb_body(const here_tracking_http_parser_evt* evt,
                                                         bool last,
                                                         void* cb_data)
{
    uint32_t* cb_count = (uint32_t*)cb_data;
    (*cb_count)++;

    switch(evt->id)
    {
        case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
        {
            ck_assert((*cb_count) == 7 || (*cb_count) == 8);

            if((*cb_count) == 7)
            {
                ck_assert(evt->data.body.buffer_size == 5);
                ck_assert(memcmp(evt->data.body.buffer, "HELLO", 5) == 0);
                ck_assert(!last);
            }
            else if((*cb_count) == 8)
            {
                ck_assert(evt->data.body.buffer_size == 7);
                ck_assert(memcmp(evt->data.body.buffer, " WORLD!", 7) == 0);
                ck_assert(last);
            }
        }
        break;

        default:
            break;
    }

    return false;
}

/**************************************************************************************************/

static bool \
    test_here_tracking_http_parser_zero_content_length_cb(const here_tracking_http_parser_evt* evt,
                                                          bool last,
                                                          void* cb_data)
{
    uint32_t* cb_count = (uint32_t*)cb_data;
    (*cb_count)++;

    switch(evt->id)
    {
        case HERE_TRACKING_HTTP_PARSER_EVT_VERSION:
        {
            ck_assert((*cb_count) == 1);
            ck_assert(evt->data.version.major == 2);
            ck_assert(evt->data.version.minor == 15);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE:
        {
            ck_assert((*cb_count) == 2);
            ck_assert(evt->data.status_code == 404);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_REASON:
        {
            ck_assert((*cb_count == 3));
            ck_assert(evt->data.reason.buffer_size == 9);
            ck_assert(memcmp(evt->data.reason.buffer, "Not Found", 9) == 0);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_HDR:
        {
            ck_assert((*cb_count) == 4 || (*cb_count) == 6);

            if((*cb_count) == 4)
            {
                ck_assert(evt->data.hdr.hdr_key_size == 14);
                ck_assert(memcmp(evt->data.hdr.hdr_key,
                                 here_tracking_http_header_content_length,
                                 14) == 0);
                ck_assert(evt->data.hdr.hdr_val_size == 1);
                ck_assert(memcmp(evt->data.hdr.hdr_val, "0", 1) == 0);
                ck_assert(last);
            }
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
        {
            ck_assert(true);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY_SIZE:
        {
            ck_assert((*cb_count) == 5);
            ck_assert(evt->data.body_size == 0);
        }
        break;

        default:
            break;
    }

    return false;
}

/**************************************************************************************************/

static bool \
    test_here_tracking_http_parser_no_content_cb(const here_tracking_http_parser_evt* evt,
                                                 bool last,
                                                 void* cb_data)
{
    uint32_t* cb_count = (uint32_t*)cb_data;
    (*cb_count)++;

    switch(evt->id)
    {
        case HERE_TRACKING_HTTP_PARSER_EVT_VERSION:
        {
            ck_assert((*cb_count) == 1);
            ck_assert(evt->data.version.major == 1);
            ck_assert(evt->data.version.minor == 1);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_STATUS_CODE:
        {
            ck_assert((*cb_count) == 2);
            ck_assert(evt->data.status_code == 204);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_REASON:
        {
            ck_assert((*cb_count == 4));
            ck_assert(evt->data.reason.buffer_size == 10);
            ck_assert(memcmp(evt->data.reason.buffer, "No Content", 10) == 0);
            ck_assert(last);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_HDR:
        {
            ck_assert(true);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY:
        {
            ck_assert(true);
        }
        break;

        case HERE_TRACKING_HTTP_PARSER_EVT_BODY_SIZE:
        {
            ck_assert((*cb_count) == 3);
            ck_assert(evt->data.body_size == 0);
        }
        break;

        default:
            break;
    }

    return false;
}

/**************************************************************************************************/

static bool test_here_tracking_http_parser_cb_nop(const here_tracking_http_parser_evt* evt,
                                                  bool last,
                                                  void* cb_data)
{
    return false;
}

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_ok_simple)
{
    char* resp;
    uint32_t cb_count = 0;
    uint32_t resp_size = strlen(simple_resp);
    here_tracking_http_parser parser;
    here_tracking_error res = here_tracking_http_parser_init(&parser,
                                                             test_here_tracking_http_parser_cb,
                                                             (void*)(&cb_count));
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    memcpy(resp, simple_resp, resp_size);
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(resp_size == strlen(simple_resp));
    ck_assert(cb_count == 7);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_ok_need_more_data)
{
    char* resp;
    uint32_t cb_count = 0;
    uint32_t resp_size = strlen(simple_resp);
    here_tracking_http_parser parser;
    here_tracking_error res = here_tracking_http_parser_init(&parser,
                                                             test_here_tracking_http_parser_cb,
                                                             (void*)(&cb_count));
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    memcpy(resp, simple_resp, resp_size);
    resp_size /= 2;
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_ERROR_NEED_MORE_DATA);
    ck_assert(resp_size == 37); /* 37 is the offset to beginning of "Server:" header */
    ck_assert(cb_count == 5);
    resp_size = strlen(simple_resp) - 37;
    res = here_tracking_http_parser_parse(&parser, resp + 37, &resp_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(resp_size == strlen(simple_resp) - 37);
    ck_assert(cb_count == 7);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_ok_multi_cb_body)
{
    char* resp;
    uint32_t cb_count = 0;
    uint32_t resp_size = strlen(simple_resp);
    here_tracking_http_parser parser;
    here_tracking_error res = \
        here_tracking_http_parser_init(&parser,
                                       test_here_tracking_http_parser_multi_cb_body,
                                       (void*)(&cb_count));
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    memcpy(resp, simple_resp, resp_size);
    resp_size = 78;
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_ERROR_NEED_MORE_DATA);
    ck_assert(resp_size == 78); /* 78 is offset to end of "HELLO" in the body */
    ck_assert(cb_count == 7);
    resp_size = strlen(simple_resp) - 78;
    res = here_tracking_http_parser_parse(&parser, resp + 78, &resp_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(resp_size == strlen(simple_resp) - 78);
    ck_assert(cb_count == 8);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_ok_zero_content_length)
{
    char* resp;
    uint32_t cb_count = 0;
    uint32_t resp_size = strlen(simple_resp_zero_content_length);
    here_tracking_http_parser parser;
    here_tracking_error res = \
        here_tracking_http_parser_init(&parser,
                                       test_here_tracking_http_parser_zero_content_length_cb,
                                       (void*)(&cb_count));
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    memcpy(resp, simple_resp_zero_content_length, resp_size);
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(resp_size == strlen(simple_resp_zero_content_length));
    ck_assert(cb_count == 6);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_ok_end_of_header_split)
{
    char* resp, *body;
    uint32_t cb_count = 0;
    uint32_t resp_size = strlen(simple_resp) + 1;
    here_tracking_http_parser parser;
    here_tracking_error res = here_tracking_http_parser_init(&parser,
                                                             test_here_tracking_http_parser_cb,
                                                             (void*)(&cb_count));
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    strcpy(resp, simple_resp);
    body = strstr(resp, "\r\n\r\n");
    body += 4;
    resp_size = body - resp;
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_ERROR_NEED_MORE_DATA);
    ck_assert(resp_size == (body - resp));
    ck_assert(cb_count == 6);
    resp_size = strlen(body);
    res = here_tracking_http_parser_parse(&parser, body, &resp_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(resp_size == strlen(body));
    ck_assert(cb_count == 7);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_ok_one_byte_increment)
{
    char* resp;
    uint32_t pos = 0, size, parse_size;
    here_tracking_http_parser parser;
    here_tracking_error res = here_tracking_http_parser_init(&parser,
                                                             test_here_tracking_http_parser_cb_nop,
                                                             NULL);
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(strlen(simple_resp));
    memcpy(resp, simple_resp, strlen(simple_resp));
    size = 1;

    do
    {
        ck_assert(pos < strlen(simple_resp));
        ck_assert(size <= strlen(simple_resp) - pos);
        parse_size = size;
        res = here_tracking_http_parser_parse(&parser, resp + pos, &parse_size);

        if(parse_size > 0)
        {
            pos += parse_size;
            size = 1;
        }
        else
        {
            size++;
        }

        if(res == HERE_TRACKING_OK)
        {
            ck_assert(pos == strlen(simple_resp));
        }
    } while(res == HERE_TRACKING_ERROR_NEED_MORE_DATA);

    ck_assert(res == HERE_TRACKING_OK);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_ok_no_content)
{
    char* resp;
    uint32_t cb_count = 0;
    uint32_t resp_size = strlen(simple_resp_no_content);
    here_tracking_http_parser parser;
    here_tracking_error res = \
        here_tracking_http_parser_init(&parser,
                                       test_here_tracking_http_parser_no_content_cb,
                                       (void*)(&cb_count));
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    memcpy(resp, simple_resp_no_content, resp_size);
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_OK);
    ck_assert(resp_size == strlen(simple_resp_no_content));
    ck_assert(cb_count == 4);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_invalid_input)
{
    char* resp;
    uint32_t resp_size = strlen(simple_resp);
    here_tracking_http_parser parser;
    here_tracking_error res = here_tracking_http_parser_init(NULL,
                                                             test_here_tracking_http_parser_cb,
                                                             NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_http_parser_init(&parser, NULL, NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_http_parser_init(&parser,
                                         test_here_tracking_http_parser_cb,
                                         NULL);
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    memcpy(resp, simple_resp, resp_size);
    res = here_tracking_http_parser_parse(NULL, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_http_parser_parse(&parser, NULL, &resp_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    res = here_tracking_http_parser_parse(&parser, resp, NULL);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    resp_size = 0;
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_ERROR_INVALID_INPUT);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_no_content_length)
{
    char* resp;
    uint32_t resp_size = strlen(simple_resp_no_content_length);
    here_tracking_http_parser parser;
    here_tracking_error res = here_tracking_http_parser_init(&parser,
                                                             test_here_tracking_http_parser_cb_nop,
                                                             NULL);
    ck_assert(res == HERE_TRACKING_OK);
    resp = malloc(resp_size);
    memcpy(resp, simple_resp_no_content_length, resp_size);
    res = here_tracking_http_parser_parse(&parser, resp, &resp_size);
    ck_assert(res == HERE_TRACKING_ERROR);
    free(resp);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_invalid_version)
{
    static char* wrong_protocol = "ERROR/1.1 200 OK\r\n";
    static char* no_version = "HTTP/ 200 OK\r\n";
    static char* no_minor_version = "HTTP/1 200 OK\r\n";
    static char* no_major_version = "HTTP/.1 200 OK\r\n";
    static char* minor_version_not_digit = "HTTP/1.Z 200 OK\r\n";
    static char* major_version_not_digit = "HTTP/Z.1 200 OK\r\n";
    static char* only_dot = "HTTP/. 200 OK\r\n";
    static char* wrong_version_format = "HTTP/FOOBAR 200 OK\r\n";
    here_tracking_http_parser parser;
    uint32_t size;
    here_tracking_error res = here_tracking_http_parser_init(&parser,
                                                             test_here_tracking_http_parser_cb_nop,
                                                             NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(wrong_protocol);
    res = here_tracking_http_parser_parse(&parser, wrong_protocol, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_version);
    res = here_tracking_http_parser_parse(&parser, no_version, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_minor_version);
    res = here_tracking_http_parser_parse(&parser, no_minor_version, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_major_version);
    res = here_tracking_http_parser_parse(&parser, no_major_version, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_major_version);
    res = here_tracking_http_parser_parse(&parser, no_major_version, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(minor_version_not_digit);
    res = here_tracking_http_parser_parse(&parser, minor_version_not_digit, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(major_version_not_digit);
    res = here_tracking_http_parser_parse(&parser, major_version_not_digit, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(only_dot);
    res = here_tracking_http_parser_parse(&parser, only_dot, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(wrong_version_format);
    res = here_tracking_http_parser_parse(&parser, wrong_version_format, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

START_TEST(test_here_tracking_http_parser_invalid_status_code)
{
    static char* too_short = "HTTP/1.1 99 OK\r\n";
    static char* too_long = "HTTP/1.1 1234 OK\r\n";
    static char* no_digits = "HTTP/1.1 ABC OK\r\n";
    static char* no_digit_first = "HTTP/1.1 A23 OK\r\n";
    static char* no_digit_second = "HTTP/1.1 1B3 OK\r\n";
    static char* no_digit_third = "HTTP/1.1 12C OK\r\n";
    here_tracking_http_parser parser;
    uint32_t size;
    here_tracking_error res = here_tracking_http_parser_init(&parser,
                                                             test_here_tracking_http_parser_cb_nop,
                                                             NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(too_short);
    res = here_tracking_http_parser_parse(&parser, too_short, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(too_long);
    res = here_tracking_http_parser_parse(&parser, too_long, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_digits);
    res = here_tracking_http_parser_parse(&parser, no_digits, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_digit_first);
    res = here_tracking_http_parser_parse(&parser, no_digit_first, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_digit_second);
    res = here_tracking_http_parser_parse(&parser, no_digit_second, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
    res = here_tracking_http_parser_init(&parser, test_here_tracking_http_parser_cb_nop, NULL);
    ck_assert(res == HERE_TRACKING_OK);
    size = strlen(no_digit_third);
    res = here_tracking_http_parser_parse(&parser, no_digit_third, &size);
    ck_assert(res == HERE_TRACKING_ERROR);
}
END_TEST

/**************************************************************************************************/

Suite* test_here_tracking_http_parser_suite(void)
{
    Suite* s = suite_create(TEST_NAME);
    TCase* tc = tcase_create(TEST_NAME);
    tcase_add_test(tc, test_here_tracking_http_parser_ok_simple);
    tcase_add_test(tc, test_here_tracking_http_parser_ok_need_more_data);
    tcase_add_test(tc, test_here_tracking_http_parser_ok_multi_cb_body);
    tcase_add_test(tc, test_here_tracking_http_parser_ok_zero_content_length);
    tcase_add_test(tc, test_here_tracking_http_parser_ok_end_of_header_split);
    tcase_add_test(tc, test_here_tracking_http_parser_ok_one_byte_increment);
    tcase_add_test(tc, test_here_tracking_http_parser_ok_no_content);
    tcase_add_test(tc, test_here_tracking_http_parser_invalid_input);
    tcase_add_test(tc, test_here_tracking_http_parser_no_content_length);
    tcase_add_test(tc, test_here_tracking_http_parser_invalid_version);
    tcase_add_test(tc, test_here_tracking_http_parser_invalid_status_code);
    suite_add_tcase(s, tc);
    return s;
}

/**************************************************************************************************/

int main()
{
    int failed;
    SRunner* sr = srunner_create(test_here_tracking_http_parser_suite());
    srunner_set_xml(sr, TEST_NAME"_test_result.xml");
    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
