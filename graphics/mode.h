/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef GRAPHICS_MODE_H
#define GRAPHICS_MODE_H

#include "common/array.h"

namespace Graphics {

/**
 * Represents a hardware video mode.
 */
struct Mode {
	int16 width; ///< The width in pixels
	int16 height; ///< The height in pixels

	Mode(const int16 w, const int16 h) :
		width(w),
		height(h) {}

	bool operator<(const Mode &other) const {
		return width < other.width && height < other.height;
	}
};

typedef Common::Array<Mode> ModeList;

/**
 * Represents value numerator/(2^shift)
 */
class ScaleFactor {
public:
	ScaleFactor(uint val) : numerator(val), shift(0) {}
	ScaleFactor(uint val, uint sh) : numerator(val), shift(sh) { normalizeInPlace(); }
	ScaleFactor() : numerator(1), shift(0) {}

	bool operator==(const ScaleFactor &b) const {
		ScaleFactor an = normalize(), bn = b.normalize();
		return an.numerator == bn.numerator && an.shift == bn.shift;
	}

	bool operator!=(const ScaleFactor &b) const {
		return !(*this == b);
	}

	ScaleFactor inc() {
		return ScaleFactor(numerator + 1, shift).normalize();
	}

	ScaleFactor dec() {
		ScaleFactor ret = { numerator, shift };
		if (numerator == 1) {
			ret.shift++;
			return ret;
		}
		return ScaleFactor(numerator - 1, shift).normalize();
	}

	uint operator*(uint b) {
		return (b * numerator) >> shift;
	}

	Common::U32String makeString() const {
		if (shift == 0)
			return Common::U32String::format("%dx", numerator);
		return Common::U32String::format("%d/%dx", numerator, 1 << shift);
	}

	uint32 makeTag() const {
		return numerator | (shift << 16);
	}

	uint32 getFracDenom() const {
		return 1 << shift;
	}

	uint32 getFracPartNum() const {
		return numerator & ((1 << shift) - 1);
	}

	uint32 getIntPart() const {
		return numerator >> shift;
	}

	uint32 isInteger() const {
		return shift == 0;
	}

	bool operator<(int b) const {
		return b >= 0 && numerator < ((uint) b << shift);
	}

	bool operator>(int b) const {
		return b <= 0 || numerator > ((uint) b << shift);
	}

private:
	void normalizeInPlace() {
		while (shift > 0 && (numerator & 1) == 0) {
			shift--;
			numerator >>= 1;
		}
	}

	ScaleFactor normalize() const {
		ScaleFactor ret = *this;
		ret.normalizeInPlace();
		return ret;
	}

	uint numerator;
	uint shift;
};


FORCEINLINE uint operator*(uint a, ScaleFactor b) {
	return b * a;
}

}

#endif
