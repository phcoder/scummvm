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

#ifndef COMMON_USTR_H
#define COMMON_USTR_H

#include "common/scummsys.h"
#include "common/str-enc.h"
#include "common/base-str.h"

namespace Common {

/**
 * @defgroup common_ustr UTF-32 strings
 * @ingroup common_str
 *
 * @brief API for working with UTF-32 strings.
 *
 * @{
 */
 
class String;
class UnicodeBiDiText;

/**
 * Very simple string class for UTF-32 strings in ScummVM. The main intention
 * behind this class is to feature a simple way of displaying UTF-32 strings
 * through the Graphics::Font API.
 *
 * Please note that operations like equals, deleteCharacter, toUppercase, etc.
 * are only very simplified convenience operations. They might not fully work
 * as you would expect for a proper UTF-32 string class.
 *
 * The presence of \0 characters in the string will cause undefined
 * behavior in some operations.
 */
class U32String : public BaseString<uint32> {
public:
	typedef uint32 unsigned_type;
public:
	/** Construct a new empty string. */
	U32String() : BaseString() {}

	/** Construct a new string from the given NULL-terminated C string. */
	explicit U32String(const value_type *str) : BaseString(str) {}

	/** Construct a new string containing exactly len characters read from address str. */
	U32String(const value_type *str, uint32 len) : BaseString(str, len) {}

	/** Construct a new string containing the characters between beginP (including) and endP (excluding). */
	U32String(const value_type *beginP, const value_type *endP) : BaseString(beginP, endP) {}

	/** Construct a copy of the given string. */
	U32String(const U32String &str) : BaseString(str) {}

	/** Construct a copy of the given unicode BiDi converted string. */
	U32String(const UnicodeBiDiText &txt);

	/** Construct a new string from the given NULL-terminated C string. */
	explicit U32String(const char *str, CodePage page = kUtf8);

	/** Construct a new string containing exactly len characters read from address str. */
	U32String(const char *str, uint32 len, CodePage page = kUtf8);

	/** Construct a new string containing the characters between beginP (including) and endP (excluding). */
	U32String(const char *beginP, const char *endP, CodePage page = kUtf8);

	/** Construct a copy of the given string. */
	U32String(const String &str, CodePage page = kUtf8);

	~U32String();

	U32String &operator=(const U32String &str);
	U32String &operator=(const String &str);
	U32String &operator=(const value_type *str);
	U32String &operator=(const char *str);
	U32String &operator+=(const U32String &str);
	U32String &operator+=(value_type c);
	using BaseString<uint32>::operator==;
	using BaseString<uint32>::operator!=;
	bool operator==(const String &x) const;
	bool operator==(const char *x) const;
	bool operator!=(const String &x) const;
	bool operator!=(const char *x) const;

    /** Python-like method **/
    String encode(CodePage page = kUtf8) const;

	/**
	 * Print formatted data into a U32String object.
	 */
	static U32String format(U32String fmt, ...);
	static U32String format(const char *fmt, ...);

	/**
	 * Print formatted data into a U32String object. It takes in the
	 * output by reference and works with iterators.
	 */
	static int vformat(const value_type *fmt, const value_type *fmtEnd, U32String &output, va_list args);

	/**
	 * Helper function for vformat, convert an int to string
	 * minimal implementation, only for base 10
	*/
	static char* itoa(int num, char* str, int base);

	using BaseString<uint32>::insertString;
	void insertString(const char *s, uint32 p, CodePage page = kUtf8);
	void insertString(const String &s, uint32 p, CodePage page = kUtf8);

	static Common::U32String decodeUTF16BE(uint16 *start, uint len);
	static Common::U32String decodeUTF16LE(uint16 *start, uint len);
	static Common::U32String decodeUTF16Native(uint16 *start, uint len);

private:
	void decodeInternal(const char *str, uint32 len, CodePage page);
	void decodeOneByte(const char *str, uint32 len, CodePage page);
	void decodeWindows949(const char *src, uint32 len);
	void decodeUTF8(const char *str, uint32 len);
		
	friend class String;
};

U32String operator+(const U32String &x, const U32String &y);
U32String operator+(const U32String &x, U32String::value_type y);

/** @} */

} // End of namespace Common

#endif
