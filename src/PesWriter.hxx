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

#ifndef PES_WRITER_HXX
#define PES_WRITER_HXX

#include "util/GrowingBuffer.hxx"

#include <assert.h>
#include <stdint.h>
#include <stddef.h>

static inline uint8_t *
PesColorChange(uint8_t *p, unsigned color)
{
	*p++ = 254;
	*p++ = 176;
	*p++ = color;
	return p;
}

inline constexpr bool
PesCheckSmallStitch(int delta)
{
	return delta >= -64 && delta <= 63;
}

inline constexpr bool
PesCheckSmallStitch(int x, int y)
{
	return PesCheckSmallStitch(x) && PesCheckSmallStitch(y);
}

inline constexpr bool
PesCheckBigStitch(int delta)
{
	return delta >= -2048 && delta <= 2048;
}

inline constexpr bool
PesCheckBigStitch(int x, int y)
{
	return PesCheckBigStitch(x) && PesCheckBigStitch(y);
}

/**
 * A "big" stitch in the range (-2048..2047).
 */
static inline uint8_t *
PesBigStitch(uint8_t *p, int x, int y, bool jump, bool trim)
{
	uint8_t flags = 0x80;
	if (jump)
		flags |= 0x10;
	if (trim)
		flags |= 0x20;

	*p++ = flags | ((x >> 8) & 0xf);
	*p++ = x & 0xff;
	*p++ = flags | ((y >> 8) & 0xf);
	*p++ = y & 0xff;
	return p;
}

/**
 * A "small" stitch in the range (-64..63).
 */
static inline uint8_t *
PesSmallStitch(uint8_t *p, int x, int y)
{
	*p++ = x & 0x7f;
	*p++ = y & 0x7f;
	return p;
}

static inline uint8_t *
PesEnd(uint8_t *p)
{
	*p++ = 0xff;
	*p++ = 0;
	return p;
}

class PesWriter {
	GrowingBuffer<uint8_t> buffer;

public:
	PesWriter();

	void ColorChange(unsigned color) {
		GenerateWrite(PesColorChange, color);
	}

	void SmallStitch(int x, int y) {
		GenerateWrite(PesSmallStitch, x, y);
	}

	void BigStitch(int x, int y, bool jump, bool trim) {
		GenerateWrite(PesBigStitch, x, y, jump, trim);
	}

	void BigStitch(int x, int y) {
		BigStitch(x, y, false, false);
	}

	void JumpStitch(int x, int y) {
		BigStitch(x, y, true, false);
	}

	void Stitch(int x, int y) {
		if (PesCheckSmallStitch(x, y)) {
			SmallStitch(x, y);
		} else {
			BigStitch(x, y);
		}
	}

	/**
	 * Stitch a line to the given relative position.  If the
	 * dimensions exceed the encodable PES limits even for a "big"
	 * stitch command, multiple stitch commands are emitted.
	 */
	void StitchLine(int x, int y) {
		while (!PesCheckBigStitch(x, y)) {
			int step_x = 0, step_y = 0;

			if (x < -2048)
				step_x = -2048;
			else if (x > 2047)
				step_x = 2047;

			if (y < -2048)
				step_y = -2048;
			else if (y > 2047)
				step_y = 2047;

			BigStitch(step_x, step_y);

			x -= step_x;
			y -= step_y;
		}

		Stitch(x, y);
	}

	void Jump(int x, int y) {
		while (x != 0 && y != 0) {
			int step_x = std::max(-2048, std::min(2047, x));
			int step_y = std::max(-2048, std::min(2047, y));
			JumpStitch(step_x, step_y);

			x -= step_x;
			y -= step_y;
		}
	}

	void End() {
		GenerateWrite(PesEnd);
	}

	ConstBuffer<uint8_t> Finish() {
		End();
		return buffer;
	}

private:
	template<typename F, typename... Args>
	void GenerateWrite(F &&f, Args... args) {
		uint8_t *p = buffer.PrepareWrite(4);
		uint8_t *end = f(p, args...);
		buffer.CommitWrite(end - p);
	}
};

#endif
