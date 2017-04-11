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

#ifndef SVG_PARSER_HXX
#define SVG_PARSER_HXX

#include "ExpatParser.hxx"

#include <forward_list>
#include <vector>

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

	SvgPoint &operator+=(SvgPoint other) {
		x += other.x;
		y += other.y;
		return *this;
	}
};

struct SvgVertex : SvgPoint {
	enum class Type {
		MOVE,
		LINE,
		ARC,
		CUBIC_CURVE,
	} type;

	constexpr SvgVertex(Type _type, SvgPoint _p):SvgPoint(_p), type(_type) {}

	constexpr SvgVertex(Type _type, double _x, double _y):SvgPoint(_x, _y), type(_type) {}
};

struct SvgPath {
	std::vector<SvgVertex> points;
};

class SvgParser final : public CommonExpatParser {
	typedef std::forward_list<SvgPath> PathList;
	PathList paths;

	struct Group {
		std::string transform;
		PathList::iterator end;

		Group(const char *_transform, PathList::iterator _end)
			:transform(_transform), end(_end) {}
	};

	std::forward_list<Group> groups;

public:
	const PathList &GetPaths() const {
		return paths;
	}

private:
	void BeginTransform(const char *transform);
	void EndTransform();

	void ParsePath(const char *d);
	void ParseRect(const char *x, const char *y,
		       const char *width, const char *height);

protected:
	void StartElement(const XML_Char *name,
			  const XML_Char **atts) override;
	void EndElement(const XML_Char *name) override;
	void CharacterData(const XML_Char *s, int len) override;
};

#endif
