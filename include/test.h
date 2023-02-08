/*  Copyright (C) 2023  Mikołaj Lenczewski <mblenczewski@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TEST_H
#define TEST_H

#include "common.h"

#define TEST_PASS() return 1;
#define TEST_FAIL() return 0;

#define TESTS_BEGIN() s32 __test_suite_result = 0;
#define TESTS_END() return __test_suite_result;

#define TEST_RUN(name)								\
do {										\
	fprintf(stderr, "%s:%s:\n", __FILE__, #name);				\
	if (name()) {								\
		fprintf(stderr, "%s:%s: OK\n", __FILE__, #name);		\
	} else {								\
		fprintf(stderr, "%s:%s: FAILED\n", __FILE__, #name);		\
		__test_suite_result = 1;					\
	}									\
} while (0);

#define _TEST_ASSERT_IMPL(cond, ...)						\
do {										\
	fprintf(stderr, "[%s:%d] %s:", __func__, __LINE__, #cond);		\
	fprintf(stderr, __VA_ARGS__);						\
	fprintf(stderr, "\n");							\
	fflush(stderr);								\
} while (0);

#define TEST_ASSERT(cond, ...)							\
if (!(cond)) { _TEST_ASSERT_IMPL(cond, __VA_ARGS__); TEST_FAIL() }

#define TEST_EXPECT(cond, ...)							\
if (!(cond)) { _TEST_ASSERT_IMPL(cond, __VA_ARGS__); }

#endif /* TEST_H */
