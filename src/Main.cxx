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
#include "PesWriter.hxx"
#include "util/SystemError.hxx"
#include "util/ScopeExit.hxx"

#include <stdexcept>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

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

static void
WriteFile(int fd, ConstBuffer<uint8_t> src)
{
	ssize_t nbytes = write(fd, src.data, src.size);
	if (nbytes < 0)
		throw MakeErrno("Failed to write file");

	if (size_t(nbytes) != src.size)
		throw std::runtime_error("Short write to file");
}

static void
WriteFile(const char *path, ConstBuffer<uint8_t> src)
{
	int fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0666);
	if (fd < 0)
		throw FormatErrno("Failed to create %s", path);

	AtScopeExit(fd) { close(fd); };
	WriteFile(fd, src);
}

static void
SvgToPes(PesWriter &pes, const SvgParser &svg)
{
	SvgPoint cursor(0, 0);

	for (const auto &path : svg.GetPaths()) {
		//pes.ColorChange(0);
		for (const auto &point : path.points) {
			auto relative = point - cursor;
			auto deltax = round(relative.x);
			auto deltay = round(relative.y);
			while(!(deltax>-2048 && deltax<2048
				&& deltay>-2048 && deltay<2048)) {
				if(deltax<-2048) {pes.Stitch(-2048,0);deltax+=2048;cursor += SvgPoint(-2048,0);}
				if(deltax>2047) {pes.Stitch(2047,0);deltax-=2047;cursor += SvgPoint(2047,0);}
				if(deltay<-2048) {pes.Stitch(0,-2048);deltay+=2048;cursor += SvgPoint(0,-2048);}
				if(deltay>2047) {pes.Stitch(0,2047);deltay-=2047;cursor += SvgPoint(0,2047);}
				}
				printf("(%lf, %lf)\n",deltax,deltay);
			pes.Stitch(deltax,deltay);
			cursor += SvgPoint(deltax,deltay);
		}
	}
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

	PesWriter writer;
	SvgToPes(writer, parser);
	WriteFile(out_path, writer.Finish());

	return EXIT_SUCCESS;
} catch (const std::exception &e) {
	fprintf(stderr, "Error: %s\n", e.what());
	return EXIT_FAILURE;
}
