/*
 * Copyright (C) 2016-2017 Max Kellermann <max.kellermann@gmail.com>
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
#include "PesFormat.hxx"

#include <string.h>

PesWriter::PesWriter(ConstBuffer<uint8_t> colors)
{
	PesHeader header;

	uint8_t *p = buffer.PrepareWrite(sizeof(header));
	memcpy(p, &header, sizeof(header));
	buffer.CommitWrite(sizeof(header));

	PecHeader pec_header(200, 100);

	assert(colors.size <= pec_header.colors.size());
	pec_header.n_colors = colors.size;
	std::copy_n(colors.data, colors.size, pec_header.colors.begin());

	p = buffer.PrepareWrite(sizeof(pec_header));
	memcpy(p, &pec_header, sizeof(pec_header));
	buffer.CommitWrite(sizeof(pec_header));
}
