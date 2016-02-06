/*
 * Copyright (C) 2016 Max Kellermann <max@duempel.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdexcept>

#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char **argv)
try {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s INFILE.svg OUTFILE.pes\n", argv[0]);
		return EXIT_FAILURE;
	}

	const auto in_path = argv[1];
	const auto out_path = argv[1];

	(void)in_path;
	(void)out_path;

	return EXIT_SUCCESS;
} catch (const std::exception &e) {
	fprintf(stderr, "Error: %s\n", e.what());
	return EXIT_FAILURE;
}
