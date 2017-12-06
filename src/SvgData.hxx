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

#ifndef SVG_DATA_HXX
#define SVG_DATA_HXX

#include "Color.hxx"

#include <vector>

#include <math.h>

struct SvgPoint {
	double x, y;

	SvgPoint() = default;
	constexpr SvgPoint(double _x, double _y)
		:x(_x), y(_y) {}

	constexpr SvgPoint operator+(SvgPoint other) const {
		return {x + other.x, y + other.y};
	}

	constexpr SvgPoint operator-(SvgPoint other) const {
		return {x - other.x, y - other.y};
	}

	constexpr SvgPoint operator*(double factor) const {
		return {x * factor, y * factor};
	}

	SvgPoint &operator+=(SvgPoint other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	SvgPoint &operator-=(SvgPoint other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	SvgPoint &operator*=(double factor) {
		x *= factor;
		y *= factor;
		return *this;
	}

	constexpr double SquareMagnitude() const noexcept {
		return x * x + y * y;
	}

	double Atan() const noexcept {
		return atan2(y, x);
	}
};

struct SvgVertex : SvgPoint {
	enum class Type {
		MOVE,
		LINE,
		ARC,
		QUADRATIC_CURVE,
		CUBIC_CURVE,
		SMOOTH_QUADRATIC_CURVE,
		SMOOTH_CUBIC_CURVE,
	} type;

	constexpr SvgVertex(Type _type, SvgPoint _p):SvgPoint(_p), type(_type) {}

	constexpr SvgVertex(Type _type, double _x, double _y):SvgPoint(_x, _y), type(_type) {}
};

struct SvgPath {
	std::vector<SvgVertex> points;

	Color fill_color, stroke_color;

	bool fill = false, stroke = false;
};

#endif
