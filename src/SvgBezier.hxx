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

struct SvgPath;
struct SvgPoint;

/**
 * Generate a line path from the given SVG quadratic bezier curve.
 */
void
SvgQuadraticBezierToLines(SvgPath &dest, SvgPoint start,
			  SvgPoint control, SvgPoint end) noexcept;

/**
 * Generate a line path from the given SVG cubic bezier curve.
 */
void
SvgCubicBezierToLines(SvgPath &dest, SvgPoint start, SvgPoint control1,
		      SvgPoint control2, SvgPoint end) noexcept;
