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

#pragma once

#include "util/MathUtil.hxx"

template<typename P>
class QuadraticBezierCurve {
	P start, control, end;

public:
	constexpr QuadraticBezierCurve(P _start, P _control, P _end)
		:start(_start), control(_control), end(_end) {}

	constexpr P GetPoint(double t) const noexcept {
		const double u = 1 - t;
		return start * Square(u) + control * 2 * Square(u) * t
			+ end * Square(t);
	}
};

template<typename P>
class CubicBezierCurve {
	P start, control1, control2, end;

public:
	constexpr CubicBezierCurve(P _start, P _control1, P _control2, P _end)
		:start(_start), control1(_control1),
		 control2(_control2), end(_end) {}

	constexpr P GetPoint(double t) const noexcept {
		const double u = 1 - t;
		return start * Cube(u) + control1 * 3 * Square(u) * t
			+ control2 * 3 * (u) * Square(t) + end * Cube(t);
	}
};
