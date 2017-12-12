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

struct SvgPath;

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
	SvgParser();
	~SvgParser() noexcept;

	const PathList &GetPaths() const {
		return paths;
	}

private:
	void BeginTransform(const char *transform);
	void EndTransform();

	PathList::iterator ParsePath(const char *d);
	PathList::iterator ParseRect(const char *x, const char *y,
				     const char *width, const char *height);
	PathList::iterator ParseCircle(const char *cx, const char *cy,
				       const char *r);

	void ApplyPathAttributes(SvgPath &path, const XML_Char **atts);

protected:
	void StartElement(const XML_Char *name,
			  const XML_Char **atts) override;
	void EndElement(const XML_Char *name) override;
	void CharacterData(const XML_Char *s, int len) override;
};

#endif
