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

#ifndef GRAPHICS_BIG5_H
#define GRAPHICS_BIG5_H

#include "common/array.h"
#include "common/stream.h"
#include "graphics/surface.h"

namespace Graphics {

class Big5Font {
public:
	Big5Font();
	~Big5Font();
	void loadPrefixedRaw(Common::ReadStream &input, int height);
	bool draw(byte *dest, int maxX, int maxY, uint32 destPitch, uint16 ch, byte color, byte outlineColor);
	bool draw(Graphics::Surface *surf, int x, int y, uint16 ch, uint32 color);
	inline bool getHeight() const { return _chineseTraditionalHeight; }

	static const int kChineseTraditionalWidth = 16;
private:
	static const int kChineseTraditionalMaxHeight = 16;
	struct ChineseTraditionalGlyph {
		byte bitmap[kChineseTraditionalMaxHeight][kChineseTraditionalWidth / 8];
		byte outline[kChineseTraditionalMaxHeight][kChineseTraditionalWidth / 8];

		void makeOutline(int height);
	};

	bool drawReal(byte *dest, int maxX, int maxY, uint32 destPitch, uint16 textChar, byte color, byte outlineColor, bool outline);

	Common::Array<ChineseTraditionalGlyph> _chineseTraditionalFont;
	Common::Array<int> _chineseTraditionalIndex;
	int _chineseTraditionalHeight;
};

}
#endif
