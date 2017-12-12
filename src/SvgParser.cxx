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

#include "SvgParser.hxx"
#include "SvgData.hxx"
#include "SvgMatrix.hxx"
#include "SvgArc.hxx"
#include "SvgBezier.hxx"
#include "CssColor.hxx"
#include "CssParser.hxx"
#include "ExpatUtil.hxx"
#include "util/StringUtil.hxx"

#include <stdexcept>

#include <assert.h>
#include <string.h>
#include <math.h>

SvgParser::SvgParser() = default;
SvgParser::~SvgParser() noexcept = default;

static bool
ParseFlag(const char *&d)
{
	bool value;

	switch (*d) {
	case '0':
		value = false;
		break;

	case '1':
		value = true;
		break;

	default:
		throw std::runtime_error("Malformed flag");
	}

	++d;
	if (*d != 0) {
		if (*d != ' ' && *d != ',')
			throw std::runtime_error("Malformed flag");

		d = StripLeft(d);
	}

	return value;
}

static double
ParseDouble(const char *&d)
{
	char *endptr;
	double value = strtod(d, &endptr);
	if (endptr == d)
		throw std::runtime_error("Malformed number");

	d = StripLeft(endptr);
	return value;
}

static SvgPoint
ParsePoint(const SvgPoint cursor, bool relative, const char *&d)
{
	SvgPoint p;
	p.x = ParseDouble(d);
	if (*d == ',')
		++d;
	p.y = ParseDouble(d);

	if (relative)
		p += cursor;

	return p;
}

static SvgPoint
ParseHorizontal(const SvgPoint cursor, bool relative, const char *&d)
{
	auto value = ParseDouble(d);

	SvgPoint p = cursor;
	if (relative)
		p.x += value;
	else
		p.x = value;

	return p;
}

static SvgPoint
ParseVertical(const SvgPoint cursor, bool relative, const char *&d)
{
	auto value = ParseDouble(d);

	SvgPoint p = cursor;
	if (relative)
		p.y += value;
	else
		p.y = value;

	return p;
}

class SvgPathParser : public SvgPath {
	SvgPoint cursor{0, 0};

	enum class Type {
		MOVE,
		LINE,
		ARC,
		QUADRATIC_CURVE,
		CUBIC_CURVE,
		SMOOTH_QUADRATIC_CURVE,
		SMOOTH_CUBIC_CURVE,
	};

public:
	void Parse(const char *d);

private:
	void ParseVertex(Type type, bool relative, const char *&d);
};

inline void
SvgPathParser::ParseVertex(Type type, bool relative, const char *&d)
{
	SvgPoint p;

	switch (type) {
	case Type::MOVE:
		points.emplace_back(SvgVertex::Type::MOVE,
				    ParsePoint(cursor, relative, d));
		cursor = points.back();
		break;

	case Type::LINE:
		points.emplace_back(SvgVertex::Type::LINE,
				    ParsePoint(cursor, relative, d));
		cursor = points.back();
		break;

	case Type::ARC:
		{
			SvgPoint radius = ParsePoint({}, false, d);
			if (*d == ',')
				d = StripLeft(d + 1);

			double rotation = ParseDouble(d);
			if (*d == ',')
				d = StripLeft(d + 1);

			bool large_arc = ParseFlag(d);
			if (*d == ',')
				d = StripLeft(d + 1);

			bool sweep = ParseFlag(d);
			if (*d == ',')
				d = StripLeft(d + 1);

			SvgPoint end = ParsePoint(cursor, relative, d);

			SvgArcToLines(*this, cursor, radius, rotation,
				      large_arc, sweep,
				      end);
		}

		cursor = points.back();

		break;

	case Type::QUADRATIC_CURVE:
		{
			const auto control = ParsePoint(cursor, relative, d);
			const auto end = ParsePoint(cursor, relative, d);

			SvgQuadraticBezierToLines(*this, cursor, control, end);
		}

		cursor = points.back();
		break;

	case Type::CUBIC_CURVE:
		{
			const auto control1 = ParsePoint(cursor, relative, d);
			const auto control2 = ParsePoint(cursor, relative, d);
			const auto end = ParsePoint(cursor, relative, d);

			SvgCubicBezierToLines(*this, cursor,
					      control1, control2, end);
		}

		cursor = points.back();
		break;

	case Type::SMOOTH_QUADRATIC_CURVE:
		// TODO: implement
		points.emplace_back(SvgVertex::Type::LINE,
				    ParsePoint(cursor, relative, d));
		cursor = points.back();
		break;

	case Type::SMOOTH_CUBIC_CURVE:
		// TODO: implement
		ParsePoint(cursor, relative, d);
		points.emplace_back(SvgVertex::Type::LINE,
				    ParsePoint(cursor, relative, d));
		cursor = points.back();
		break;
	}
}

inline void
SvgPathParser::Parse(const char *d)
{
	Type type = Type::MOVE;
	bool relative = false;
	size_t sub_start = 0;

	while (*(d = StripLeft(d)) != 0) {
		switch (*d) {
		case 'M':
			type = Type::MOVE;
			relative = false;
			++d;
			break;

		case 'm':
			type = Type::MOVE;
			relative = true;
			++d;
			break;

		case 'L':
			type = Type::LINE;
			relative = false;
			++d;
			break;

		case 'l':
			type = Type::LINE;
			relative = true;
			++d;
			break;

		case 'A':
			type = Type::ARC;
			relative = false;
			++d;
			break;

		case 'a':
			type = Type::ARC;
			relative = true;
			++d;
			break;

		case 'Q':
			type = Type::QUADRATIC_CURVE;
			relative = false;
			++d;
			break;

		case 'q':
			type = Type::QUADRATIC_CURVE;
			relative = true;
			++d;
			break;

		case 'C':
			type = Type::CUBIC_CURVE;
			relative = false;
			++d;
			break;

		case 'c':
			type = Type::CUBIC_CURVE;
			relative = true;
			++d;
			break;

		case 'T':
			type = Type::SMOOTH_QUADRATIC_CURVE;
			relative = false;
			++d;
			break;

		case 't':
			type = Type::SMOOTH_QUADRATIC_CURVE;
			relative = true;
			++d;
			break;

		case 'S':
			type = Type::SMOOTH_CUBIC_CURVE;
			relative = false;
			++d;
			break;

		case 's':
			type = Type::SMOOTH_CUBIC_CURVE;
			relative = true;
			++d;
			break;

		case 'H':
			++d;
			points.emplace_back(SvgVertex::Type::LINE,
					    ParseHorizontal(cursor, false, d));
			cursor = points.back();
			break;

		case 'h':
			++d;
			points.emplace_back(SvgVertex::Type::LINE,
					    ParseHorizontal(cursor, true, d));
			cursor = points.back();
			break;

		case 'V':
			++d;
			points.emplace_back(SvgVertex::Type::LINE,
					    ParseVertical(cursor, false, d));
			cursor = points.back();
			break;

		case 'v':
			++d;
			points.emplace_back(SvgVertex::Type::LINE,
					    ParseVertical(cursor, true, d));
			cursor = points.back();
			break;

		case 'z':
		case 'Z':
			if (sub_start < points.size())
				points.emplace_back(SvgVertex::Type::LINE,
						    points[sub_start]);

			sub_start = points.size();
			++d;
			break;

		default:
			ParseVertex(type, relative, d);
		}
	}
}

inline SvgParser::PathList::iterator
SvgParser::ParsePath(const char *d)
{
	SvgPathParser pp;
	pp.Parse(d);
	paths.emplace_front(std::move(pp));
	return paths.begin();
}

inline SvgParser::PathList::iterator
SvgParser::ParseRect(const char *_x, const char *_y,
		     const char *_width, const char *_height)
{
	if (_width == nullptr && _height == nullptr)
		return paths.end();

	double x = _x != nullptr ? strtod(_x, nullptr) : 0;
	double y = _y != nullptr ? strtod(_y, nullptr) : 0;
	double width = strtod(_width, nullptr);
	double height = strtod(_height, nullptr);
	if (width <= 0 || height <= 0)
		return paths.end();

	paths.emplace_front();
	auto &points = paths.front().points;
	points.reserve(5);
	points.emplace_back(SvgVertex::Type::MOVE, x, y);
	points.emplace_back(SvgVertex::Type::LINE, x + width, y);
	points.emplace_back(SvgVertex::Type::LINE, x + width, y + height);
	points.emplace_back(SvgVertex::Type::LINE, x, y + height);
	points.emplace_back(SvgVertex::Type::LINE, x, y);
	return paths.begin();
}

inline SvgParser::PathList::iterator
SvgParser::ParseCircle(const char *_cx, const char *_cy, const char *_r)
{
	if (_r == nullptr)
		return paths.end();

	double cx = _cx != nullptr ? strtod(_cx, nullptr) : 0;
	double cy = _cy != nullptr ? strtod(_cy, nullptr) : 0;
	double r = strtod(_r, nullptr);
	if (r <= 0)
		return paths.end();

	paths.emplace_front();
	auto &points = paths.front().points;
	points.reserve(5);
	points.emplace_back(SvgVertex::Type::MOVE, cx + r, cy);

	for (double angle = 0.03; angle < 2 * M_PI; angle += 0.06)
		points.emplace_back(SvgVertex::Type::LINE,
				    cx + r * cos(angle),
				    cy + r * sin(angle));

	points.emplace_back(SvgVertex::Type::LINE, cx + r, cy);

	return paths.begin();
}

namespace {

void
ApplyStroke(SvgPath &path, const char *stroke)
{
	if (strcmp(stroke, "none") == 0) {
		path.stroke = false;
		return;
	}

	path.stroke_color = ParseCssColor(stroke);
	path.stroke = true;
}

void
ApplyFill(SvgPath &path, const char *fill)
{
	if (strcmp(fill, "none") == 0) {
		path.fill = false;
		return;
	}

	path.fill_color = ParseCssColor(fill);
	path.fill = true;
}

} // anonymous namespace

void
SvgParser::ApplyPathAttributes(SvgPath &path, const XML_Char **atts)
{
	const char *style = FindXmlAttribute(atts, "style");
	if (style != nullptr) {
		try {
			auto css = ParseCss(style);
			auto i = css.find("stroke");
			if (i != css.end())
				ApplyStroke(path, i->second.c_str());

			i = css.find("fill");
			if (i != css.end())
				ApplyFill(path, i->second.c_str());
		} catch (...) {
			fprintf(stderr, "Failed to parse CSS '%s'\n", style);
		}
	}

	const char *stroke = FindXmlAttribute(atts, "stroke");
	if (stroke != nullptr) {
		try {
			ApplyStroke(path, stroke);
		} catch (...) {
			fprintf(stderr, "Failed to parse color '%s'\n", stroke);
		}
	}

	const char *fill = FindXmlAttribute(atts, "fill");
	if (fill != nullptr) {
		try {
			ApplyFill(path, fill);
		} catch (...) {
			fprintf(stderr, "Failed to parse color '%s'\n", fill);
		}
	}
}

void
SvgParser::StartElement(const XML_Char *name, const XML_Char **atts)
{
	const char *transform = FindXmlAttribute(atts, "transform");
	if (transform == nullptr)
		transform = "";

	groups.emplace_front(transform, paths.begin());

	auto path = paths.end();
	if (strcmp(name, "path") == 0) {
		const char *d = FindXmlAttribute(atts, "d");
		if (d != nullptr)
			path = ParsePath(d);
	} else if (strcmp(name, "rect") == 0) {
		const char *x = FindXmlAttribute(atts, "x");
		const char *y = FindXmlAttribute(atts, "y");
		const char *width = FindXmlAttribute(atts, "width");
		const char *height = FindXmlAttribute(atts, "height");
		path = ParseRect(x, y, width, height);
	} else if (strcmp(name, "circle") == 0) {
		const char *cx = FindXmlAttribute(atts, "cx");
		const char *cy = FindXmlAttribute(atts, "cy");
		const char *r = FindXmlAttribute(atts, "r");
		path = ParseCircle(cx, cy, r);
	}

	if (path != paths.end())
		ApplyPathAttributes(*path, atts);
}

class SvgTransform {
	SvgMatrix matrix;

public:
	void Parse(const char *p);

	void Apply(SvgPath &path) {
		for (auto &i : path.points)
			(SvgPoint &)i = matrix * i;
	}
};

static const char *
ParseMatrix(SvgMatrix &m, const char *p)
{
	for (unsigned x = 0; x < 3; ++x) {
		for (unsigned y = 0; y < 2; ++y) {
			m.values[y][x] = ParseDouble(p);
			if (*p == ',')
				++p;
		}
	}

	if (*p != ')')
		throw std::runtime_error("')' expected");

	return p + 1;
}

static const char *
ParseTranslate(SvgMatrix &m, const char *p)
{
	m.values[0][2] = ParseDouble(p);
	if (*p == ',')
		++p;

	if (*p != ')')
		m.values[1][2] = ParseDouble(p);

	if (*p != ')')
		throw std::runtime_error("')' expected");

	return p + 1;
}

static const char *
ParseScale(SvgMatrix &m, const char *p)
{
	m.values[0][0] = m.values[1][1] = ParseDouble(p);
	if (*p == ',')
		++p;

	if (*p != ')')
		m.values[1][1] = ParseDouble(p);

	if (*p != ')')
		throw std::runtime_error("')' expected");

	return p + 1;
}

inline void
SvgTransform::Parse(const char *p)
{
	while (*(p = StripLeft(p)) != 0) {
		const char *q;
		if ((q = StringAfterPrefix(p, "matrix(")) != nullptr) {
			SvgMatrix m;
			p = ParseMatrix(m, q);
			matrix = m * matrix;
		} else if ((q = StringAfterPrefix(p, "translate(")) != nullptr) {
			SvgMatrix m;
			p = ParseTranslate(m, q);
			matrix = m * matrix;
		} else if ((q = StringAfterPrefix(p, "scale(")) != nullptr) {
			SvgMatrix m;
			p = ParseScale(m, q);
			matrix = m * matrix;
		} else {
			throw std::runtime_error("Failed to parse transform");
		}
	}
}

template<typename I>
static void
ApplyTransform(const char *_transform, I begin, I end)
{
	if (*_transform == 0)
		return;

	SvgTransform transform;
	transform.Parse(_transform);

	for (I i = begin; i != end; ++i)
		transform.Apply(*i);
}

void
SvgParser::EndElement(const XML_Char *name)
{
	(void)name;

	assert(!groups.empty());

	ApplyTransform(groups.front().transform.c_str(),
		       paths.begin(), groups.front().end);
	groups.pop_front();
}

void
SvgParser::CharacterData(const XML_Char *s, int len)
{
	(void)s;
	(void)len;
}
