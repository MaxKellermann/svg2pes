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

static inline uint8_t *
PesJumpStitch(uint8_t *p, int length)
{
	*p++ = 0x80 | ((length >> 8) & 0xf);
	*p++ = length & 0xff;
	return p;
}

static inline uint8_t *
PesStitch(uint8_t *p, int x, int y)
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

	void JumpStitch(int length) {
		GenerateWrite(PesJumpStitch, length);
	}

	void Stitch(int x, int y) {
		if(x<63 && x>-64 && y<63 && y>-64) {
			//Regular stitch
			GenerateWrite(PesStitch, x, y);
		} else {
			//Jump stitch
			x+=x<0?0x1000:0;
			y+=y<0?0x1000:0;
			x&=0x7FF;
			y&=0x7FF;
			x|=0x800;
			y|=0x800;
			GenerateWrite(PesStitch, x, y);
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
