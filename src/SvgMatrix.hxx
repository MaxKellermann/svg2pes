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

#ifndef SVG_MATRIX_HXX
#define SVG_MATRIX_HXX

#include "SvgData.hxx"

struct SvgMatrix {
	double values[3][3] = {{1,0,0},{0,1,0},{0,0,1}};

	SvgMatrix operator*(SvgMatrix other) const {
		SvgMatrix result;

		for (unsigned x = 0; x < 3; ++x) {
			for (unsigned y = 0; y < 3; ++y) {
				unsigned sum = 0;
				for (unsigned i = 0; i < 3; ++i)
					sum += values[y][i] * other.values[i][x];
				result.values[y][x] = sum;
			}
		}

		return result;
	}

	SvgMatrix &operator*=(SvgMatrix other) {
		return *this = (*this * other);
	}

	SvgPoint operator*(SvgPoint p) {
		SvgPoint result;
		result.x = values[0][0] * p.x + values[0][1] * p.y + values[0][2];
		result.y = values[1][0] * p.x + values[1][1] * p.y + values[1][2];
		return result;
	}
};

#endif
