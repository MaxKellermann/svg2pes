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

#include "SvgBezier.hxx"
#include "SvgData.hxx"
#include "BezierCurve.hxx"

void
SvgQuadraticBezierToLines(SvgPath &dest, SvgPoint start,
			  SvgPoint control, SvgPoint end) noexcept
{
	const QuadraticBezierCurve<SvgPoint> curve(start, control, end);
	for (double t = 0.03; t < 1; t += 0.06)
		dest.points.push_back(SvgVertex(SvgVertex::Type::LINE,
						curve.GetPoint(t)));

	dest.points.push_back(SvgVertex(SvgVertex::Type::LINE, end));
}

void
SvgCubicBezierToLines(SvgPath &dest, SvgPoint start, SvgPoint control1,
		      SvgPoint control2, SvgPoint end) noexcept
{
	const CubicBezierCurve<SvgPoint> curve(start, control1, control2, end);
	for (double t = 0.03; t < 1; t += 0.06)
		dest.points.push_back(SvgVertex(SvgVertex::Type::LINE,
						curve.GetPoint(t)));

	dest.points.push_back(SvgVertex(SvgVertex::Type::LINE, end));
}
