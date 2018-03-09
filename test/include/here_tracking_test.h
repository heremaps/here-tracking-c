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

#ifndef HERE_TRACKING_TEST_H
#define HERE_TRACKING_TEST_H

#include <stdlib.h>

#include <check.h>

#define TEST_SUITE_BEGIN(NAME) \
    Suite* NAME##_suite(void) \
    { \
        Suite* s = suite_create(NAME); \
        TCase* tc = tcase_create(NAME);

#define TEST_SUITE_ADD_SETUP_TEARDOWN_FN(SETUP_FN, TEARDOWN_FN) \
    tcase_add_checked_fixture(tc, SETUP_FN, TEARDOWN_FN);

#define TEST_SUITE_ADD_TEST(NAME) \
    tcase_add_test(tc, NAME);

#define TEST_SUITE_END \
    suite_add_tcase(s, tc);\
    return s; \
    }

#define TEST_MAIN(NAME) \
    int main() \
    { \
        int failed; \
        SRunner* sr = srunner_create(NAME##_suite()); \
        srunner_set_fork_status(sr, CK_NOFORK); \
        srunner_set_xml(sr, NAME"_test_result.xml");\
        srunner_run_all(sr, CK_VERBOSE); \
        failed = srunner_ntests_failed(sr); \
        srunner_free(sr); \
        return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE; \
    }

#endif /* HERE_TRACKING_TEST_H */
