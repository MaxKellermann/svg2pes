/*
 * Copyright (C) 2016 Max Kellermann <max.kellermann@gmail.com>
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

#include "PesWriter.hxx"
#include "util/ByteOrder.hxx"

#include <array>

#include <string.h>

struct PesHeader {
	char id[4]{'#', 'P', 'E', 'S'};
	char number[4]{'0', '0', '0', '1'};
	uint32_t pec_offset = ToLE32(sizeof(PesHeader));
	std::array<uint8_t, 36> unknown1;

	PesHeader() {
		unknown1.fill(0);
	}
};

static_assert(sizeof(PesHeader) == 48, "Wrong PES header size");

struct PecHeader {
	std::array<uint8_t, 48> unknown1;
	uint8_t n_colors = 0;
	std::array<uint8_t, 256> colors;
	std::array<uint8_t, 207> unknown2;
	std::array<uint8_t, 2> unknown3;
	gcc_packed uint32_t graphic_offset = ToLE32(0);
	uint16_t unknown4 = 0;
	uint16_t width = ToLE16(0), height = ToLE16(0);
	std::array<uint8_t, 8> unknown5;

	PecHeader(unsigned _width, unsigned _height)
		:width(ToLE16(_width)), height(ToLE16(_height))
	{
		unknown1.fill(0);
		colors.fill(0x20);
		colors.front() = 20; // black
		unknown2.fill(0);
		unknown3.fill(0);
		unknown5.fill(0);
	}
};

static_assert(sizeof(PecHeader) == 532, "Wrong PEC header size");

PesWriter::PesWriter(ConstBuffer<uint8_t> colors)
{
	PesHeader header;

	uint8_t *p = buffer.PrepareWrite(sizeof(header));
	memcpy(p, &header, sizeof(header));
	buffer.CommitWrite(sizeof(header));

	PecHeader pec_header(200, 100);

	assert(colors.size <= pec_header.colors.size());
	std::copy_n(colors.data, colors.size, pec_header.colors.begin());

	p = buffer.PrepareWrite(sizeof(pec_header));
	memcpy(p, &pec_header, sizeof(pec_header));
	buffer.CommitWrite(sizeof(pec_header));
}
