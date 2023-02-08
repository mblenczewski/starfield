/*  Copyright (C) 2023  Mikołaj Lenczewski <mblenczewski@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "starfield-client.h"

s32
main(s32 argc, char **argv) {
	(void)argc;
	(void)argv;

	printf("Version: " STARFIELD_CLIENT_VERSION "\n");
	printf("Hello, World!\n");

	return 0;
}

