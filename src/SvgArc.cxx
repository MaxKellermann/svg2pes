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

#include "Geometry.hxx"
#include "SvgArc.hxx"
#include "SvgData.hxx"
#include "Compiler.h"

#include <math.h>

namespace {

class SvgArc {
	Ellipse<SvgPoint> unrotated_ellipse;
	double start_angle, delta_angle;
	const Rotation rotation;

public:
	SvgArc(SvgPoint start, SvgPoint radius,
	       double _rotation, bool large_arc, bool sweep,
	       SvgPoint end) noexcept;

	SvgPoint GetPoint(double t) const noexcept;
};

constexpr SvgPoint
ScaleY(SvgPoint p, double factor)
{
	return {p.x, p.y * factor};
}

SvgArc::SvgArc(const SvgPoint start, const SvgPoint _radius,
	       const double _rotation, const bool large_arc, const bool sweep,
	       const SvgPoint end) noexcept
	:rotation(_rotation)
{
	unrotated_ellipse.radius = _radius;

	const auto aspect_ratio = unrotated_ellipse.GetAspectRatio();

	/* transform the rotated ellipse into an unrotated circle,
	   because that is easier to calculate; GetPoint() will later
	   undo this transformation */
	const auto unrotated_start = ScaleY((-rotation)(start), aspect_ratio);
	const auto unrotated_end = ScaleY((-rotation)(end), aspect_ratio);

	unrotated_ellipse.center = CircleCenter(unrotated_start, unrotated_end,
						unrotated_ellipse.radius.x,
						sweep ^ large_arc);

	/* calculate the start and end angle and apply the large_arc
	   flag */
	start_angle = (unrotated_start - unrotated_ellipse.center).Atan();
	double end_angle = (unrotated_end - unrotated_ellipse.center).Atan();

	unrotated_ellipse.center.y /= aspect_ratio;

	delta_angle = end_angle - start_angle;

	if (large_arc) {
		/* fabs(delta_angle) must be >= M_PI */
		if (delta_angle >= 0) {
			if (delta_angle < M_PI)
				end_angle -= 2 * M_PI;
		} else {
			if (delta_angle > -M_PI)
				start_angle -= 2 * M_PI;
		}
	} else {
		/* fabs(delta_angle) must be <= M_PI */
		if (delta_angle > M_PI)
			end_angle -= 2 * M_PI;
		else if (delta_angle < -M_PI)
			start_angle -= 2 * M_PI;
	}

	delta_angle = end_angle - start_angle;
}

SvgPoint
SvgArc::GetPoint(double t) const noexcept
{
	const double angle = start_angle + delta_angle * t;
	const auto p = unrotated_ellipse.AtAngle(angle);

	return rotation(p);
}

}

void
SvgArcToLines(SvgPath &dest, SvgPoint start, SvgPoint radius,
	      double rotation, bool large_arc, bool sweep,
	      SvgPoint end)
{
	rotation *= M_PI / 180.;

	const SvgArc arc(start, radius, rotation, large_arc, sweep, end);
	for (double t = 0.03; t < 1; t += 0.03)
		dest.points.push_back(SvgVertex(SvgVertex::Type::LINE,
						arc.GetPoint(t)));

	dest.points.push_back(SvgVertex(SvgVertex::Type::LINE, end));
}
