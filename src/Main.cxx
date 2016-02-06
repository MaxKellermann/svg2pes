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

#include "SvgParser.hxx"
#include "util/SystemError.hxx"
#include "util/ScopeExit.hxx"

#include <stdexcept>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static void
FeedFile(SvgParser &parser, int fd)
{
	while (true) {
		char buffer[8192];
		ssize_t nbytes = read(fd, buffer, sizeof(buffer));
		if (nbytes < 0)
			throw MakeErrno("Failed to read file");

		if (nbytes == 0)
			break;

		parser.Parse(buffer, nbytes, false);
	}

	parser.Parse("", 0, true);
}

static void
FeedFile(SvgParser &parser, const char *path)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0)
		throw FormatErrno("Failed to open %s", path);

	AtScopeExit(fd) { close(fd); };
	FeedFile(parser, fd);
}

int
main(int argc, char **argv)
try {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s INFILE.svg OUTFILE.pes\n", argv[0]);
		return EXIT_FAILURE;
	}

	const auto in_path = argv[1];
	const auto out_path = argv[2];

	(void)out_path;

	SvgParser parser;
	FeedFile(parser, in_path);

	return EXIT_SUCCESS;
} catch (const std::exception &e) {
	fprintf(stderr, "Error: %s\n", e.what());
	return EXIT_FAILURE;
}
