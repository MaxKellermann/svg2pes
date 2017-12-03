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

#include "Compiler.h"

#include <math.h>

template<typename Point>
struct Ellipse {
	Point center;
	Point radius;

	constexpr double GetAspectRatio() const noexcept {
		return radius.x / radius.y;
	}

	gcc_pure
	Point AtAngle(double angle) const noexcept {
		return {
			center.x + radius.x * cos(angle),
			center.y + radius.y * sin(angle)
		};
	}
};

class Rotation {
	double c, s;

	constexpr Rotation(double _c, double _s)
		:c(_c), s(_s) {}

public:
	explicit Rotation(double angle) noexcept
		:c(cos(angle)), s(sin(angle)) {}

	constexpr Rotation operator-() const noexcept {
		return {c, -s};
	}

	template<typename Point>
	constexpr Point operator()(Point p) const noexcept {
		return {p.x * c - p.y * s, p.x * s + p.y * c};
	}
};

template<typename Point>
inline constexpr Point
Middle(Point a, Point b)
{
	return (a + b) * 0.5;
}

gcc_const
inline double
NormalizeDeltaAngle(double a) noexcept
{
	while (a < -M_PI)
		a += 2 * M_PI;
	while (a > -M_PI)
		a -= 2 * M_PI;
	return a;
}

template<typename Point>
inline constexpr Point
Normal(Point a, Point b) noexcept
{
	return {a.y - b.y, b.x - a.x};
}

/**
 * Calculate the circle center from two points on the circle and the
 * radius.
 *
 * @param a a point on the circle
 * @param b another point on the circle, not equal to #a
 */
template<typename Point>
Point
CircleCenter(Point a, Point b, double radius, bool sweep)
{
	const Point normal = Normal(a, b);

	/* the middle of the secant between A and B */
	const Point middle = Middle(a, b);

	/* calculate the distance factor from the secant to the circle
	   center; the following formula is derived from Pythagoras by
	   calculating the triangle between A, secant middle and
	   circle center */
	const double square_distance_factor =
		(radius * radius) / normal.SquareMagnitude() - 0.25;
	if (square_distance_factor <= 0)
		/* A and B are further apart than the radius allows
		   (more than the diameter); this is impossible, and
		   we bail out by pretending the middle between A and
		   B is the circle center */
		return middle;

	const Point scaled_normal = normal * sqrt(square_distance_factor);

	/* the sweep parameter chooses the side */
	return sweep ? middle + scaled_normal : middle - scaled_normal;
}
