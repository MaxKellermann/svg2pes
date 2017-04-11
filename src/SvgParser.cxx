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

#include <stdexcept>

#include <assert.h>
#include <string.h>

static const char *
FindAttribute(const XML_Char **atts, const char *name)
{
	for (; *atts != nullptr; atts += 2)
		if (strcmp(atts[0], name) == 0)
			return atts[1];

	return nullptr;
}

static const char *
StripLeft(const char *p)
{
	while (*p == ' ')
		++p;
	return p;
}

static const char *
StringAfterPrefix(const char *s, const char *prefix)
{
	while (*prefix != 0) {
		if (*s != *prefix)
			return nullptr;

		++s;
		++prefix;
	}

	return s;
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

class SvgPathParser : public SvgPath {
	SvgPoint cursor{0, 0};

public:
	void Parse(const char *d);

private:
	void ParseVertex(SvgVertex::Type type, bool relative, const char *&d);
};

inline void
SvgPathParser::ParseVertex(SvgVertex::Type type, bool relative, const char *&d)
{
	SvgPoint p;

	switch (type) {
	case SvgVertex::Type::MOVE:
	case SvgVertex::Type::LINE:
		points.emplace_back(type, ParsePoint(cursor, relative, d));
		cursor = points.back();
		break;

	case SvgVertex::Type::ARC:
		ParsePoint(cursor, relative, d);
		ParseDouble(d);
		ParseDouble(d);
		ParseDouble(d);
		ParsePoint(cursor, relative, d);
		break;

	case SvgVertex::Type::CURVE:
		ParsePoint(cursor, relative, d);
		ParsePoint(cursor, relative, d);
		points.emplace_back(type, ParsePoint(cursor, relative, d));
		cursor = points.back();
		break;
	}
}

inline void
SvgPathParser::Parse(const char *d)
{
	SvgVertex::Type type = SvgVertex::Type::MOVE;
	bool relative = false;
	size_t sub_start = 0;

	while (*(d = StripLeft(d)) != 0) {
		switch (*d) {
		case 'M':
			type = SvgVertex::Type::MOVE;
			relative = false;
			++d;
			break;

		case 'm':
			type = SvgVertex::Type::MOVE;
			relative = true;
			++d;
			break;

		case 'L':
			type = SvgVertex::Type::LINE;
			relative = false;
			++d;
			break;

		case 'l':
			type = SvgVertex::Type::LINE;
			relative = true;
			++d;
			break;

		case 'A':
			type = SvgVertex::Type::ARC;
			relative = false;
			++d;
			break;

		case 'a':
			type = SvgVertex::Type::ARC;
			relative = true;
			++d;
			break;

		case 'C':
			type = SvgVertex::Type::CURVE;
			relative = false;
			++d;
			break;

		case 'c':
			type = SvgVertex::Type::CURVE;
			relative = true;
			++d;
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

inline void
SvgParser::ParsePath(const char *d)
{
	SvgPathParser pp;
	pp.Parse(d);
	paths.emplace_front(std::move(pp));
}

inline void
SvgParser::ParseRect(const char *_x, const char *_y,
		     const char *_width, const char *_height)
{
	if (_width == nullptr && _height == nullptr)
		return;

	double x = _x != nullptr ? strtod(_x, nullptr) : 0;
	double y = _y != nullptr ? strtod(_y, nullptr) : 0;
	double width = strtod(_width, nullptr);
	double height = strtod(_height, nullptr);
	if (width <= 0 || height <= 0)
		return;

	paths.emplace_front();
	auto &points = paths.front().points;
	points.reserve(5);
	points.emplace_back(SvgVertex::Type::MOVE, x, y);
	points.emplace_back(SvgVertex::Type::LINE, x + width, y);
	points.emplace_back(SvgVertex::Type::LINE, x + width, y + height);
	points.emplace_back(SvgVertex::Type::LINE, x, y + height);
	points.emplace_back(SvgVertex::Type::LINE, x, y);
}

void
SvgParser::StartElement(const XML_Char *name, const XML_Char **atts)
{
	const char *transform = FindAttribute(atts, "transform");
	if (transform == nullptr)
		transform = "";

	groups.emplace_front(transform, paths.begin());

	if (strcmp(name, "path") == 0) {
		const char *d = FindAttribute(atts, "d");
		if (d != nullptr)
			ParsePath(d);
	} else if (strcmp(name, "rect") == 0) {
		const char *x = FindAttribute(atts, "x");
		const char *y = FindAttribute(atts, "y");
		const char *width = FindAttribute(atts, "width");
		const char *height = FindAttribute(atts, "height");
		ParseRect(x, y, width, height);
	}
}

struct SvgMatrix {
	double values[3][3] = {{1,0,0},{0,1,0},{0,0,1}};

	SvgMatrix operator*(SvgMatrix other) {
		SvgMatrix result;

		for (unsigned x = 0; x < 3; ++x) {
			for (unsigned y = 0; y < 3; ++y) {
				unsigned sum = 0;
				for (unsigned i = 0; i < 3; ++i)
					sum += values[y][i] * other.values[i][x];
				result.values[y][x] = sum;
			}
		}

		return *this;
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
