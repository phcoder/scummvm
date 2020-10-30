/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/str.h"
#include "common/ustr.h"
#include "common/util.h"
#include "common/endian.h"
#include "common/error.h"
#include "common/system.h"
#include "common/enc-internal.h"

namespace Common {

// //TODO: This is a quick and dirty converter. Refactoring needed:
// 1. Original version has an option for performing strict / nonstrict
//    conversion for the 0xD800...0xDFFF interval
// 2. Original version returns a result code. This version does NOT
//    insert 'FFFD' on errors & does not inform caller on any errors
//
// More comprehensive one lives in wintermute/utils/convert_utf.cpp
void U32String::decodeUTF8(const char *src, uint32 len) {
	ensureCapacity(len, false);

	// The String class, and therefore the Font class as well, assume one
	// character is one byte, but in this case it's actually an UTF-8
	// string with up to 4 bytes per character. To work around this,
	// convert it to an U32String before drawing it, because our Font class
	// can handle that.
	for (uint i = 0; i < len;) {
		uint32 chr = 0;
		uint num = 1;

		if ((src[i] & 0xF8) == 0xF0) {
			num = 4;
		} else if ((src[i] & 0xF0) == 0xE0) {
			num = 3;
		} else if ((src[i] & 0xE0) == 0xC0) {
			num = 2;
		}

		if (len - i >= num) {
			switch (num) {
			case 4:
				chr |= (src[i++] & 0x07) << 18;
				chr |= (src[i++] & 0x3F) << 12;
				chr |= (src[i++] & 0x3F) << 6;
				chr |= (src[i++] & 0x3F);
				break;

			case 3:
				chr |= (src[i++] & 0x0F) << 12;
				chr |= (src[i++] & 0x3F) << 6;
				chr |= (src[i++] & 0x3F);
				break;

			case 2:
				chr |= (src[i++] & 0x1F) << 6;
				chr |= (src[i++] & 0x3F);
				break;

			default:
				chr = (src[i++] & 0x7F);
				break;
			}
		} else {
			break;
		}

		operator+=(chr);
	}
}

const uint16 invalidCode = 0xFFFD;

// //TODO: This is a quick and dirty converter. Refactoring needed:
// 1. Original version has an option for performing strict / nonstrict
//    conversion for the 0xD800...0xDFFF interval
// 2. Original version returns a result code. This version inserts '0xFFFD' if
//    character does not fit in 4 bytes & does not inform caller on any errors
//
// More comprehensive one lives in wintermute/utils/convert_utf.cpp
void String::encodeUTF8(const U32String &src) {
	ensureCapacity(src.size(), false);
	static const uint8 firstByteMark[5] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0 };
	char writingBytes[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

	uint i = 0;
	while (i < src.size()) {
		unsigned short bytesToWrite = 0;
		const uint32 byteMask = 0xBF;
		const uint32 byteMark = 0x80;

		uint32 ch = src[i++];
		if (ch < (uint32)0x80) {
			bytesToWrite = 1;
		} else if (ch < (uint32)0x800) {
			bytesToWrite = 2;
		} else if (ch < (uint32)0x10000) {
			bytesToWrite = 3;
		} else if (ch <= 0x0010FFFF) {
			bytesToWrite = 4;
		} else {
			bytesToWrite = 3;
			ch = invalidCode;
		}

		char *pBytes = writingBytes + (4 - bytesToWrite);

		switch (bytesToWrite) {
		case 4:
			pBytes[3] = (char)((ch | byteMark) & byteMask);
			ch >>= 6;
			// fallthrough
		case 3:
			pBytes[2] = (char)((ch | byteMark) & byteMask);
			ch >>= 6;
			// fallthrough
		case 2:
			pBytes[1] = (char)((ch | byteMark) & byteMask);
			ch >>= 6;
			// fallthrough
		case 1:
			pBytes[0] = (char)(ch | firstByteMark[bytesToWrite]);
			break;
		default:
			break;
		}

		operator+=(pBytes);
	}
}

#define decodeUTF16Template(suffix, read)				\
Common::U32String U32String::decodeUTF16 ## suffix (uint16 *start, uint len) { \
	uint16 *ptr = start;							\
	Common::U32String dst;						\
	dst.ensureCapacity(len, false);					\
									\
	while (len-- > 0) {						\
		uint16 c = read(ptr++);					\
		if (c >= 0xD800 && c <= 0xDBFF && len > 0) {		\
			uint16 low = read(ptr++);			\
			if (low >= 0xDC00 && low <= 0xDFFF)		\
				dst += ((c & 0x3ff) << 10)              \
					| (low & 0x3ff);                \
			else						\
				dst += invalidCode;			\
			continue;					\
                }							\
									\
		if (c >= 0xD800 && c <= 0xDFFF) {			\
			dst += invalidCode;				\
			continue;					\
		}							\
		dst += c;						\
	}								\
									\
	return dst;							\
}

decodeUTF16Template(BE, READ_BE_UINT16)
decodeUTF16Template(LE, READ_LE_UINT16)
decodeUTF16Template(Native, READ_UINT16)

// Upper bound on unicode codepoint in any single-byte encoding. Must be divisible by 0x100 and be strictly above large codepoint
static const int kMaxCharSingleByte = 0x3000;


static const uint32 *
getConversionTable(CodePage page) {
	switch (page) {
	case kWindows1250:
		return kWindows1250ConversionTable;
	case kWindows1251:
		return kWindows1251ConversionTable;
	case kWindows1252:
		return kWindows1252ConversionTable;
	case kWindows1253:
		return kWindows1253ConversionTable;
	case kWindows1254:
		return kWindows1254ConversionTable;
	case kWindows1255:
		return kWindows1255ConversionTable;
	case kWindows1256:
		return kWindows1256ConversionTable;
	case kWindows1257:
		return kWindows1257ConversionTable;
	case kMacCentralEurope:
		return kMacCentralEuropeConversionTable;
	case kISO8859_1:
		return kLatin1ConversionTable;
	case kISO8859_2:
		return kLatin2ConversionTable;
	case kDos850:
		return kDos850ConversionTable;

	case kCodePageInvalid:
	// Multibyte encodings. Can't be represented in simple table way
	case kUtf8:
	case kWindows932:
	case kWindows949:
	case kWindows950:
		return nullptr;
	}
	return nullptr;
}

struct ReverseTablePrefixTreeLevel1 {
	struct ReverseTablePrefixTreeLevel2 *next[kMaxCharSingleByte / 0x100];
	bool valid;
};

struct ReverseTablePrefixTreeLevel2 {
	uint8 end[256];

	ReverseTablePrefixTreeLevel2() {
		memset(end, 0, sizeof(end));
	}
};

ReverseTablePrefixTreeLevel1 reverseTables[kLastEncoding + 1];

static const ReverseTablePrefixTreeLevel1 *
getReverseConversionTable(CodePage page) {
	if (reverseTables[page].valid)
		return &reverseTables[page];
	const uint32 *conversionTable = getConversionTable(page);
	if (!conversionTable)
		return nullptr;
	reverseTables[page].valid = true;
	for (uint i = 0; i < 0x80; i++) {
		uint32 c = conversionTable[i];
		if (c >= kMaxCharSingleByte)
			continue;
		if (!reverseTables[page].next[c >> 8]) {
			reverseTables[page].next[c >> 8] = new ReverseTablePrefixTreeLevel2();
		}

		reverseTables[page].next[c >> 8]->end[c&0xff] = i | 0x80;
	}

	return &reverseTables[page];
}

void U32String::decodeOneByte(const char *src, uint32 len, CodePage page) {
    	const uint32 *conversionTable = getConversionTable(page);

	if (conversionTable == nullptr) {
		return;
	}

	ensureCapacity(len, false);

	for (uint i = 0; i < _size; ++i) {
		if ((src[i] & 0x80) == 0) {
			operator+=(src[i]);
			continue;
		}

		operator+=(conversionTable[src[i] & 0x7f]);
	}
}

void String::encodeOneByte(const U32String &src, CodePage page) {
	const ReverseTablePrefixTreeLevel1 *conversionTable = 
		getReverseConversionTable(page);

	ensureCapacity(src.size(), false);

	if (conversionTable == nullptr) {
		for (uint i = 0; i < _size; ++i) {
			uint32 c = src[i];
			if (c <= 0x7F) {
				operator+=((char)c);
				continue;
			}
		}
		return;
	}

	for (uint i = 0; i < _size; ++i) {
		uint32 c = src[i];
		if (c <= 0x7F) {
			operator+=((char)c);
			continue;
		}

		if (c >= kMaxCharSingleByte)
			continue;
		unsigned char uc = conversionTable->next[c>>8]->end[c&0xff];
		if (uc != 0) {
			operator+=((char)uc);
		}
	}
}

void String::encodeInternal(const U32String &src, CodePage page) {
	if (page == kUtf8) {
		encodeUTF8(src);
	} else {
		encodeOneByte(src, page);
	}
	// TODO:
	// "MS932", /* kWindows932 */
	// "MSCP949", /* kWindows949 */
	// "CP950"  /* kWindows950 */
}

U32String convertToU32String(const char *str, CodePage page) {
	return String(str).decode(page);
}

U32String convertUtf8ToUtf32(const String &str) {
	return str.decode(kUtf8);
}

String convertFromU32String(const U32String &string, CodePage page) {
	return string.encode(page);
}

String convertUtf32ToUtf8(const U32String &u32str) {
	return u32str.encode(kUtf8);
}

void U32String::decodeInternal(const char *str, uint32 len, CodePage page) {
	assert(str);

	_storage[0] = 0;
	_size = 0;

	if (page == kUtf8) {
		decodeUTF8(str, len);
	} else {
		decodeOneByte(str, len, page);
	}
}

U32String String::decode(CodePage page) const {
	if (page == kCodePageInvalid ||
			page > kLastEncoding) {
		error("Invalid codepage");
	}

	U32String unicodeString;
	unicodeString.decodeInternal(_str, _size, page);
	return unicodeString;
}

String U32String::encode(CodePage page) const {
	if (page == kCodePageInvalid ||
			page > kLastEncoding) {
		error("Invalid codepage");
	}

	String string;
	string.encodeInternal(*this, page);
	return string;
}

} // End of namespace Common
