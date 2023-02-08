/*  Copyright (C) 2023  Mikołaj Lenczewski <mblenczewski@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "test.h"
#include "starfield.h"

s32 test_example(void) {
	s32 a = 42;

	TEST_EXPECT(a + 2 == 44, "a + 2 != 44")
	TEST_ASSERT(a == 42, "a != 42")

	TEST_PASS()
}

s32
main(s32 argc, char **argv) {
	(void)argc;
	(void)argv;

	TESTS_BEGIN()

	TEST_RUN(test_example)

	TESTS_END()
}
