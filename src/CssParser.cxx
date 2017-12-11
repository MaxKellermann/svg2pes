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

#include "CssParser.hxx"
#include "util/StringUtil.hxx"

#include <string.h>

namespace {

constexpr bool
IsCssNameChar(char ch) noexcept
{
	return (ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z') ||
		ch == '-';
}

std::string
NextCssName(const char *&s) noexcept
{
	const char *name = s;
	while (IsCssNameChar(*s))
		++s;
	return std::string(name, s);
}

std::string
NextCssValue(const char *&s) noexcept
{
	const char *value = s;
	const char *semicolon = strchr(s, ';');
	if (semicolon == nullptr)
		semicolon = s + strlen(s);
	s = semicolon;
	return std::string(value, semicolon);
}

}

std::map<std::string, std::string>
ParseCss(const char *s)
{
	std::map<std::string, std::string> result;

	while (*s) {
		s = StripLeft(s);
		auto name = NextCssName(s);
		s = StripLeft(s);
		if (name.empty() || *s != ':') {
			s = strchr(s, ';');
			if (s == nullptr)
				break;
			++s;
			continue;
		}

		s = StripLeft(s + 1);

		auto value = NextCssValue(s);
		result.emplace(name, value);
			       /*std::make_pair(std::move(name),
				 std::move(value)));*/


		s = StripLeft(s);

		if (*s != ';') {
			s = strchr(s, ';');
			if (s == nullptr)
				break;
		}

		++s;
	}

	return result;
}
