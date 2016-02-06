/*
 * Copyright (C) 2016 Max Kellermann <max@duempel.org>
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

void
SvgParser::StartElement(const XML_Char *name, const XML_Char **atts)
{
	(void)name;
	(void)atts;
}

void
SvgParser::EndElement(const XML_Char *name)
{
	(void)name;
}

void
SvgParser::CharacterData(const XML_Char *s, int len)
{
	(void)s;
	(void)len;
}
