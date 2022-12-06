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

/**********************************************************************
 *
 * Filename:    crc.c
 *
 * Description: Fast implementation of the CRC standards.
 *
 * Notes:
 *
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#ifndef COMMON_CRC_H
#define COMMON_CRC_H

#include "common/system.h" // For types.

namespace Common {

template <typename T, T poly>
class CRCNormal {
public:
	CRCNormal(T init_remainder, T final_xor);

	T crcFast(byte const message[], int nBytes) const;
	T processByte(byte byteVal, T remainder) const;
	T processBytes(const byte* buf, uint32 sz, T remainder) const;
	T getInitRemainder() const { return _init_remainder; }
	T finalize(T remainder) const { return remainder ^ _final_xor; }

private:
	const T _init_remainder;
	const T _final_xor;

  	static T _crcTable[256];
	static bool _is_inited;
};

template <typename T, T reflected_poly>
class CRCReflected {
public:
	CRCReflected(T init_remainder, T final_xor);

	T crcFast(byte const message[], int nBytes) const;
	T processByte(byte byteVal, T remainder) const;
	T processBytes(const byte* buf, uint32 sz, T remainder) const;
	T getInitRemainder() const { return _reflected_init_remainder; }
	T finalize(T remainder) const { return remainder ^ _final_xor; }

private:
	const T _reflected_init_remainder;
	const T _final_xor;

  	static T _crcTable[256];
	static bool _is_inited;
};

#define DEFINE_CRC(class_type, type, poly)			\
 	template<> type class_type<type, poly>::_crcTable[256] = {0};	\
	template<> bool class_type<type, poly>::_is_inited = false

#define DEFINE_NORMAL_CRC(type, poly) DEFINE_CRC(CRCNormal, type, poly)
#define DEFINE_REFLECTED_CRC(type, poly) DEFINE_CRC(CRCReflected, type, poly)

template <typename T, T poly>
CRCNormal<T, poly>::CRCNormal(T init_remainder, T final_xor) : _init_remainder(init_remainder), _final_xor(final_xor) {
	const T topbit = 1ULL << (8 * sizeof(T) - 1);

	if (_is_inited)
		return;

	/*
	 * Compute the remainder of each possible dividend.
	 */
	for (int dividend = 0; dividend < 256; ++dividend) {
		/*
		 * Start with the dividend followed by zeros.
		 */
		T remainder = dividend << (8 * sizeof(T) - 8);

		/*
		 * Perform modulo-2 division, a bit at a time.
		 */
		for (byte bit = 8; bit > 0; --bit) {
			/*
			 * Try to divide the current data bit.
			 */
			if (remainder & topbit) {
				remainder = (remainder << 1) ^ poly;
			} else {
				remainder = (remainder << 1);
			}
		}

		/*
		 * Store the result into the table.
		 */
		_crcTable[dividend] = remainder;
	}

	_is_inited = true;
}

template <typename T, T reflected_poly>
CRCReflected<T, reflected_poly>::CRCReflected(T reflected_init_remainder, T final_xor) : _reflected_init_remainder(reflected_init_remainder), _final_xor(final_xor) {
	if (_is_inited)
		return;

	/*
	 * Compute the remainder of each possible dividend.
	 */
	for (int dividend = 0; dividend < 256; ++dividend) {
		/*
		 * Start with the dividend followed by zeros.
		 */
		T remainder = dividend;

		/*
		 * Perform modulo-2 division, a bit at a time.
		 */
		for (byte bit = 8; bit > 0; --bit) {
			/*
			 * Try to divide the current data bit.
			 */
			if (remainder & 1) {
				remainder = (remainder >> 1) ^ reflected_poly;
			} else {
				remainder = (remainder >> 1);
			}
		}

		/*
		 * Store the result into the table.
		 */
		_crcTable[dividend] = remainder;
	}

	_is_inited = true;
}

/*********************************************************************
 *
 * Function:    crcFast()
 *
 * Description: Compute the CRC of a given message.
 *
 * Notes:       crcInit() must be called first.
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
template<typename T, T poly>
T CRCNormal<T, poly>::crcFast(byte const message[], int nBytes) const {
	T remainder = _init_remainder;

	/*
	 * Divide the message by the polynomial, a byte at a time.
	 */
	for (int b = 0; b < nBytes; ++b) {
		byte data = message[b] ^ (remainder >> (8 * sizeof(T) - 8));
		remainder = _crcTable[data] ^ (remainder << 8);
	}

	/*
	 * The final remainder is the CRC.
	 */
	return remainder ^ _final_xor;
}

/*********************************************************************
 *
 * Function:    crcFast()
 *
 * Description: Compute the CRC of a given message.
 *
 * Notes:       crcInit() must be called first.
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
template<typename T, T reflected_poly>
T CRCReflected<T, reflected_poly>::crcFast(byte const message[], int nBytes) const {
	T remainder = _reflected_init_remainder;

	/*
	 * Divide the message by the polynomial, a byte at a time.
	 */
	for (int b = 0; b < nBytes; ++b) {
		byte data = message[b] ^ remainder;
		remainder = _crcTable[data] ^ (remainder >> 8);
	}

	/*
	 * The final remainder is the CRC.
	 */
	return remainder ^ _final_xor;
}

template<typename T, T poly>
T CRCNormal<T, poly>::processByte(byte byteVal, T remainder) const {
	byte data = byteVal ^ (remainder >> (8 * sizeof(T) - 8));

	return _crcTable[data] ^ (remainder << 8);
}

template<typename T, T poly>
T CRCReflected<T, poly>::processByte(byte byteVal, T remainder) const {
	byte data = byteVal ^ remainder;

	return _crcTable[data] ^ (remainder >> 8);
}

template<typename T, T poly>
T CRCNormal<T, poly>::processBytes(const byte *buf, uint32 sz, T remainder) const {
	for (uint32 i = 0; i < sz; i++) {
		byte data = buf[i] ^ (remainder >> (8 * sizeof(T) - 8));
		remainder = _crcTable[data] ^ (remainder << 8);
	}

	return remainder;
}

template<typename T, T poly>
T CRCReflected<T, poly>::processBytes(const byte *buf, uint32 sz, T remainder) const {
	for (uint32 i = 0; i < sz; i++) {
		byte data = buf[i] ^ remainder;
		remainder = _crcTable[data] ^ (remainder >> 8);
	}

	return remainder;
}

class CRC_CCITT : public CRCNormal<uint16, 0x1021> {
public:
	CRC_CCITT() : CRCNormal<uint16, 0x1021>(0xFFFF, 0x0000) {}
};

class CRC_BINHEX : public CRCNormal<uint16, 0x1021> {
public:
	CRC_BINHEX() : CRCNormal<uint16, 0x1021>(0x0000, 0x0000) {}
};

class CRC16 : public CRCReflected<uint16, 0xa001> {
public:
	CRC16() : CRCReflected<uint16, 0xa001>(0x0000, 0x0000) {}
};

class CRC32 : public CRCReflected<uint32, 0xEDB88320> {
public:
	CRC32() : CRCReflected<uint32, 0xEDB88320>(0xFFFFFFFF, 0xFFFFFFFF) {}
};

} // End of namespace Common

#endif
