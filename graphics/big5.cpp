#include "graphics/big5.h"

namespace Graphics {

Big5Font::Big5Font() : _chineseTraditionalHeight(0) {
}

Big5Font::~Big5Font() {
	_chineseTraditionalFont.clear();
	_chineseTraditionalIndex.clear();
}

void Big5Font::ChineseTraditionalGlyph::makeOutline(int height) {
	outline[0][0] = 0;
	outline[0][1] = 0;
	// OR into outline the original bitmap moved by 1 pixel
	// 1 pixel down
	for (int y = 0; y < height - 1; y++) {
		outline[y+1][0] = bitmap[y][0];
		outline[y+1][1] = bitmap[y][1];
	}
	// 1 pixel up
	for (int y = 0; y < height - 1; y++) {
		outline[y][0] |= bitmap[y+1][0];
		outline[y][1] |= bitmap[y+1][1];
	}
	for (int y = 0; y < height; y++) {
		// 1 pixel right
		outline[y][0] |= bitmap[y][0] >> 1;
		outline[y][1] |= bitmap[y][0] << 7;
		outline[y][1] |= bitmap[y][1] >> 1;

		// 1 pixel left
		outline[y][0] |= bitmap[y][0] << 1;
		outline[y][0] |= bitmap[y][1] >> 7;
		outline[y][1] |= bitmap[y][1] << 1;
	}

	// Then AND-out the original bitmap
	for (int y = 0; y < height; y++) {
		outline[y][0] &= ~bitmap[y][0];
		outline[y][1] &= ~bitmap[y][1];
	}
}

void Big5Font::loadPrefixedRaw(Common::ReadStream &input, int height) {
	_chineseTraditionalHeight = height;
	_chineseTraditionalFont.clear();

	_chineseTraditionalIndex = Common::move(Common::Array<int>(0x8000, -1));
	// So far the smallest version had 1981 glyphs. Optimize a little bit for this number
	// but don't rely on it in any way
	_chineseTraditionalFont.reserve(1981);
	while(!input.eos()) {
		// Big-endian because it's not really a u16 but a big5 sequence.
		uint16 ch = input.readUint16BE();
		ChineseTraditionalGlyph glyph;
		if (ch == 0xffff)
			break;
		memset(&glyph.bitmap, 0, sizeof(glyph.bitmap));
		memset(&glyph.outline, 0, sizeof(glyph.outline));
		input.read(&glyph.bitmap, (kChineseTraditionalWidth / 8) * _chineseTraditionalHeight);
		glyph.makeOutline(height);
		_chineseTraditionalIndex[ch & 0x7fff] = _chineseTraditionalFont.size();
		_chineseTraditionalFont.push_back(glyph);
	}
}

	
bool Big5Font::drawReal(byte *dest, int maxX, int maxY, uint32 destPitch, uint16 textChar, byte color, byte outlineColor, bool outline) {
	int glyphIdx = _chineseTraditionalIndex[textChar & 0x7fff];
	if (glyphIdx < 0) {
		return false;
	}

	const ChineseTraditionalGlyph& glyph = _chineseTraditionalFont[glyphIdx];

	for (int y = 0; y < _chineseTraditionalHeight && y < maxY; y++) {
		uint8 *cur = dest + y * destPitch;
		uint8 *curMax = cur + maxX;

		for (int byte = 0; byte < 2; byte++)
			for (int bit = 0; bit < 8 && cur < curMax; bit++, cur++)
				if ((glyph.bitmap[y][byte] << bit) & 0x80)
					*cur = color;
				else if (outline && (((glyph.outline[y][byte] << bit) & 0x80)))
					*cur = outlineColor;
	}
	return true;
}

bool Big5Font::draw(byte *dest, int maxX, int maxY, uint32 destPitch, uint16 ch, byte color, byte outlineColor) {
	return drawReal(dest, maxX, maxY, destPitch, ch, color, outlineColor, true);
}

bool Big5Font::draw(Graphics::Surface *surf, int x, int y, uint16 ch, uint32 color) {
	if (surf->format.bytesPerPixel != 1)
		error("Big5 fot for bpp=%d is not supported", surf->format.bytesPerPixel);
	return drawReal((byte*)surf->getBasePtr(x, y), surf->w - x, surf->h - y, surf->pitch, ch, color, 0, false);
}

	

}
