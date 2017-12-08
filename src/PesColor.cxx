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

#include "PesColor.hxx"
#include "Color.hxx"

#include <cstdlib>

namespace {

/**
 * PES color space, copied from the "pesconvert" project which got it
 * from http://bobosch.dyndns.org/embroidery/showFile.php?pes.php
 */
constexpr Color pes_colors[] = {
	{},
	{  14,  31, 124 },
	{  10,  85, 163 },
	{  48, 135, 119 },
	{  75, 107, 175 },
	{ 237,  23,  31 },
	{ 209,  92,   0 },
	{ 145,  54, 151 },
	{ 228, 154, 203 },
	{ 145,  95, 172 },
	{ 157, 214, 125 },
	{ 232, 169,   0 },
	{ 254, 186,  53 },
	{ 255, 255,   0 },
	{ 112, 188,  31 },
	{ 192, 148,   0 },
	{ 168, 168, 168 },
	{ 123, 111,   0 },
	{ 255, 255, 179 },
	{  79,  85,  86 },
	{   0,   0,   0 },
	{  11,  61, 145 },
	{ 119,   1, 118 },
	{  41,  49,  51 },
	{  42,  19,   1 },
	{ 246,  74, 138 },
	{ 178, 118,  36 },
	{ 252, 187, 196 },
	{ 254,  55,  15 },
	{ 240, 240, 240 },
	{ 106,  28, 138 },
	{ 168, 221, 196 },
	{  37, 132, 187 },
	{ 254, 179,  67 },
	{ 255, 240, 141 },
	{ 208, 166,  96 },
	{ 209,  84,   0 },
	{ 102, 186,  73 },
	{  19,  74,  70 },
	{ 135, 135, 135 },
	{ 216, 202, 198 },
	{  67,  86,   7 },
	{ 254, 227, 197 },
	{ 249, 147, 188 },
	{   0,  56,  34 },
	{ 178, 175, 212 },
	{ 104, 106, 176 },
	{ 239, 227, 185 },
	{ 247,  56, 102 },
	{ 181,  76, 100 },
	{  19,  43,  26 },
	{ 199,   1,  85 },
	{ 254, 158,  50 },
	{ 168, 222, 235 },
	{   0, 103,  26 },
	{  78,  41, 144 },
	{  47, 126,  32 },
	{ 253, 217, 222 },
	{ 255, 217,  17 },
	{   9,  91, 166 },
	{ 240, 249, 112 },
	{ 227, 243,  91 },
	{ 255, 200, 100 },
	{ 255, 200, 150 },
	{ 255, 200, 200 },
};

unsigned
ColorMatch(Color a, Color b)
{
	return std::abs(int(a.r) - int(b.r)) +
		std::abs(int(a.g) - int(b.g)) +
		std::abs(int(a.b) - int(b.b));
}

}

unsigned
NearestPesColor(Color c) noexcept
{
	unsigned best = 0;
	unsigned best_diff = ~0;

	for (unsigned i = 1u; i < sizeof(pes_colors) / sizeof(pes_colors[0]); ++i) {
		unsigned diff = ColorMatch(c, pes_colors[i]);
		if (diff < best_diff) {
			best = i;
			best_diff = diff;
		}
	}

	return best;
}
