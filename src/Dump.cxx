/*
 * Copyright (C) 2017 Max Kellermann <max.kellermann@gmail.com>
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

#include "PesFormat.hxx"
#include "util/SystemError.hxx"
#include "util/ScopeExit.hxx"

#include <stdexcept>

#include <stdio.h>
#include <stdlib.h>

static void
FullRead(FILE *file, void *buffer, size_t size)
{
	if (fread(buffer, size, 1, file) != 1)
		throw std::runtime_error("Short read");
}

static uint8_t
ReadByte(FILE *file)
{
	int ch = fgetc(file);
	if (ch == EOF)
		throw std::runtime_error("Premature end of file");

	return ch;
}

static void
Seek(FILE *file, long offset, int whence)
{
	if (fseek(file, offset, whence) != 0)
		throw MakeErrno("Seek failed");
}

static void
CheckHeader(const PesHeader &header)
{
	const PesHeader expected{};

	if (memcmp(header.id, expected.id, sizeof(expected.id)) != 0)
		throw std::runtime_error("Malformed PES id");
}

int
main(int argc, char **argv)
try {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s INFILE.pes\n", argv[0]);
		return EXIT_FAILURE;
	}

	const auto in_path = argv[1];

	FILE *file = fopen(in_path, "rb");
	if (file == nullptr) {
		fprintf(stderr, "Failed to open %s: %s\n",
			in_path, strerror(errno));
		return EXIT_FAILURE;
	}

	AtScopeExit(file) { fclose(file); };

	PesHeader pes_header;
	FullRead(file, &pes_header, sizeof(pes_header));
	CheckHeader(pes_header);

	Seek(file, FromLE32(pes_header.pec_offset), SEEK_SET);

	PecHeader pec_header(0, 0);
	FullRead(file, &pec_header, sizeof(pec_header));

	printf("width = %u\n"
	       "height = %u\n",
	       FromLE16(pec_header.width),
	       FromLE16(pec_header.height));

	for (unsigned i = 0, n = pec_header.n_colors; i < n; ++i)
		printf("color[%u] = %u\n", i, pec_header.colors[i]);

	while (true) {
		uint8_t b = ReadByte(file);
		if ((b & 0x80) == 0) {
			int delta = b;
			if (delta & 0x40)
				delta -= 0x80;

			printf("smallstitch %d\n", delta);
		} else if (b == 0xfe) {
			b = ReadByte(file);
			if (b == 0xb0)
				printf("color %u\n", ReadByte(file));
			else
				throw std::runtime_error("Unknown 0xfe command");
		} else if (b == 0xff) {
			b = ReadByte(file);
			if (b == 0x00)
				break;

			throw std::runtime_error("Unknown 0xff command");
		} else if ((b & 0xc0) == 0x80) {
			unsigned flags = b & 0x30;
			int delta = ((b & 0xf) << 8) | ReadByte(file);
			if (delta & 0x800)
				delta -= 0x1000;

			const char *operation = "bigstitch";
			if (flags & 0x20)
				operation = "trim";
			else if (flags & 0x10)
				operation = "jump";

			printf("%s %d\n", operation, delta);
		} else
			throw std::runtime_error("Unknown sequence");
	}

	return EXIT_SUCCESS;
} catch (const std::exception &e) {
	fprintf(stderr, "Error: %s\n", e.what());
	return EXIT_FAILURE;
}
