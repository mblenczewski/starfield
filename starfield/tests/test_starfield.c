/*  Copyright (C) 2023  Mikołaj Lenczewski <mblenczewski@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
