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

#include "common/system.h"
#include "engines/testbed/encoding.h"

namespace Testbed {

TestExitStatus Encodingtests::testConversionUnicodeMachineEndian() {
	Testsuite::displayMessage("Encoding conversion tests.");
	Common::String info = "Unicode conversion test. Multiple conversions between UTF-8, UTF-16 and UTF-32 in the default machinge endian will be performed.";

	Common::Point pt(0, 100);
	Testsuite::writeOnScreen("Testing unicode conversion in machine native endianness.", pt);

	if (Testsuite::handleInteractiveInput(info, "OK", "Skip", kOptionRight)) {
		Testsuite::logPrintf("Info! Skipping test : testConversionUnicodeMachineEndian\n");
		return kTestSkipped;
	}
					   //  |dolar|   cent    |     euro       |
	unsigned char utf8[] = {0x24, 0xC2, 0xA2, 0xE2, 0x82, 0xAC, 0};
#ifdef SCUMM_BIG_ENDIAN
							//| dolar |  cent  |    euro   |
	unsigned char utf16be[] = {0, 0x24, 0, 0xA2, 0x20, 0xAC, 0, 0};
							//| dolar       |  cent        |    euro  
	unsigned char utf32be[] = {0, 0, 0, 0x24, 0, 0, 0, 0xA2, 0, 0, 0x20, 0xAC, 0, 0, 0, 0};

	unsigned char *utf16 = utf16be;
	unsigned char *utf32 = utf32be;
#else
							//| dolar |  cent  |    euro   |
	unsigned char utf16le[] = {0x24, 0, 0xA2, 0, 0xAC, 0x20, 0, 0};
							//| dolar       |  cent        |    euro
	unsigned char utf32le[] = {0x24, 0, 0, 0, 0xA2, 0, 0, 0, 0xAC, 0x20, 0, 0, 0, 0, 0, 0};

	unsigned char *utf16 = utf16le;
	unsigned char *utf32 = utf32le;
#endif

	// UTF16 to UTF8
	Common::String resultstr8 = Common::U32String::decodeUTF16Native((uint16 *) utf16, 3).encode(Common::kUtf8);
	if (resultstr8.c_str() == NULL || memcmp(resultstr8.c_str(), utf8, 7)) {
		Testsuite::logPrintf("UTF-16 to UTF-8 conversion differs from the expected result.");
		return kTestFailed;
	}

	// UTF32 to UTF8

	resultstr8 = Common::U32String((uint32 *) utf32, 3).encode(Common::kUtf8);
	if (resultstr8.c_str() == NULL || memcmp(resultstr8.c_str(), utf8, 7)) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion differs from the expected result.");
		return kTestFailed;
	}


	// UTF32 to UTF16
	uint16 *result16 = Common::U32String((uint32 *) utf32, 3).encodeUTF16Native(NULL);
	if (result16 == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result16, utf16, 8)) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion differs from the expected result.");
		free(result16);
		return kTestFailed;
	}
	free(result16);

	// UTF8 to UTF16

	result16 = Common::U32String((char *) utf8, 6, Common::kUtf8).encodeUTF16Native(NULL);
	if (result16 == NULL) {
		Testsuite::logPrintf("UTF-8 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result16, utf16, 8)) {
		Testsuite::logPrintf("UTF-8 to UTF-16 conversion differs from the expected result.");
		free(result16);
		return kTestFailed;
	}
	free(result16);

	// UTF8 to UTF32
	Common::U32String resultustr = Common::String((const char *) utf8, 6).decode(Common::kUtf8);
	if (resultustr.c_str() == NULL || memcmp(resultustr.c_str(), utf32, 16)) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion differs from the expected result.");
		return kTestFailed;
	}

	// UTF16 to UTF32
	resultustr = Common::U32String::decodeUTF16Native((uint16 *) utf16, 3);
	if (resultustr.c_str() == nullptr || memcmp(resultustr.c_str(), utf32, 16)) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion differs from the expected result.");
		return kTestFailed;
	}

	return kTestPassed;
}

TestExitStatus Encodingtests::testConversionUnicodeBigEndian() {
	Common::String info = "Unicode conversion test. Multiple conversions between UTF-8, UTF-16 and UTF-32 in big endian will be performed.";

	Common::Point pt(0, 100);
	Testsuite::writeOnScreen("Testing unicode conversion in big endian.", pt);

	if (Testsuite::handleInteractiveInput(info, "OK", "Skip", kOptionRight)) {
		Testsuite::logPrintf("Info! Skipping test : testConversionUnicodeBigEndian\n");
		return kTestSkipped;
	}
					   //  |dolar|   cent    |     euro       |
	unsigned char utf8[] = {0x24, 0xC2, 0xA2, 0xE2, 0x82, 0xAC, 0};
							//| dolar |  cent  |    euro   |
	unsigned char utf16be[] = {0, 0x24, 0, 0xA2, 0x20, 0xAC, 0, 0};
							//| dolar       |  cent        |    euro  
	unsigned char utf32be[] = {0, 0, 0, 0x24, 0, 0, 0, 0xA2, 0, 0, 0x20, 0xAC, 0, 0, 0, 0};

#if SCUMM_BIG_ENDIAN
						//| dolar |  cent  |    euro   |
	unsigned char utf16[] = {0, 0x24, 0, 0xA2, 0x20, 0xAC, 0, 0};
						//| dolar       |  cent        |    euro
	unsigned char utf32[] = {0, 0, 0, 0x24, 0, 0, 0, 0xA2, 0, 0, 0x20, 0xAC, 0, 0, 0, 0};
#else
						//| dolar |  cent  |    euro   |
	unsigned char utf16[] = {0x24, 0, 0xA2, 0, 0xAC, 0x20, 0, 0};
						//| dolar       |  cent        |    euro
	unsigned char utf32[] = {0x24, 0, 0, 0, 0xA2, 0, 0, 0, 0xAC, 0x20, 0, 0, 0, 0, 0, 0};
#endif

	// UTF16 to UTF8
	Common::String resultstr8 = Common::U32String::decodeUTF16BE((uint16 *) utf16be, 3).encode(Common::kUtf8);
	if (resultstr8.c_str() == NULL || memcmp(resultstr8.c_str(), utf8, 7)) {
		Testsuite::logPrintf("UTF-16 to UTF-8 conversion differs from the expected result.");
		return kTestFailed;
	}

#if 0
	// UTF32 to UTF8
	converter.setFrom("UTF-32BE");

	result = converter.convert((char *) utf32be, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf8, 7)) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-8", "UTF-32BE", (char *) utf32be, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf8, 7)) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF32 to UTF16
	converter.setTo("UTF-16BE");

	result = converter.convert((char *) utf32be, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16be, 8)) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-16BE", "UTF-32BE", (char *) utf32be, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16be, 8)) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);
#endif

	// UTF8 to UTF16
	uint16 *result16 = Common::U32String((char *) utf8, 6, Common::kUtf8).encodeUTF16BE(NULL);
	if (result16 == NULL) {
		Testsuite::logPrintf("UTF-8 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result16, utf16be, 8)) {
		Testsuite::logPrintf("UTF-8 to UTF-16 conversion differs from the expected result.");
		free(result16);
		return kTestFailed;
	}
	free(result16);

#if 0
	// UTF8 to UTF32
	converter.setTo("UTF-32BE");

	result = converter.convert((char *) utf8, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32be, 16)) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-32BE", "UTF-8", (char *) utf8, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32be, 16)) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF16 to UTF32
	converter.setFrom("UTF-16BE");

	result = converter.convert((char *) utf16be, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32be, 16)) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-32BE", "UTF-16BE", (char *) utf16be, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32be, 16)) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF16 to UTF16
	converter.setFrom("UTF-16BE");
	converter.setTo("UTF-16");

	result = converter.convert((char *) utf16be, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16BE to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16, 8)) {
		Testsuite::logPrintf("UTF-16BE to UTF-16 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	converter.setFrom("UTF-16");
	converter.setTo("UTF-16BE");

	result = converter.convert((char *) utf16, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16 to UTF-16BE conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16be, 8)) {
		Testsuite::logPrintf("UTF-16 to UTF-16BE conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF32 to UTF32
	converter.setFrom("UTF-32BE");
	converter.setTo("UTF-32");

	result = converter.convert((char *) utf32be, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32BE to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32, 16)) {
		Testsuite::logPrintf("UTF-32BE to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	converter.setFrom("UTF-32");
	converter.setTo("UTF-32BE");

	result = converter.convert((char *) utf32, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-32BE conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32be, 16)) {
		Testsuite::logPrintf("UTF-32 to UTF-32BE conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);
#endif

	return kTestPassed;
}

TestExitStatus Encodingtests::testConversionUnicodeLittleEndian() {
	Common::String info = "Unicode conversion test. Multiple conversions between UTF-8, UTF-16 and UTF-32 in little endian will be performed.";

	Common::Point pt(0, 100);
	Testsuite::writeOnScreen("Testing unicode conversion in little endianness.", pt);

	if (Testsuite::handleInteractiveInput(info, "OK", "Skip", kOptionRight)) {
		Testsuite::logPrintf("Info! Skipping test : testConversionUnicodeLittleEndian\n");
		return kTestSkipped;
	}
					   //  |dolar|   cent    |     euro       |
	unsigned char utf8[] = {0x24, 0xC2, 0xA2, 0xE2, 0x82, 0xAC, 0};
							//| dolar |  cent  |    euro   |
	unsigned char utf16le[] = {0x24, 0, 0xA2, 0, 0xAC, 0x20, 0, 0};
							//| dolar       |  cent        |    euro
	unsigned char utf32le[] = {0x24, 0, 0, 0, 0xA2, 0, 0, 0, 0xAC, 0x20, 0, 0, 0, 0, 0, 0};

#if SCUMM_BIG_ENDIAN
	unsigned char utf16[] = {0, 0x24, 0, 0xA2, 0x20, 0xAC, 0, 0};
	//| dolar       |  cent        |    euro
	unsigned char utf32[] = {0, 0, 0, 0x24, 0, 0, 0, 0xA2, 0, 0, 0x20, 0xAC, 0, 0, 0, 0};
#else
	//| dolar |  cent  |    euro   |
	unsigned char utf16[] = {0x24, 0, 0xA2, 0, 0xAC, 0x20, 0, 0};
	//| dolar       |  cent        |    euro
	unsigned char utf32[] = {0x24, 0, 0, 0, 0xA2, 0, 0, 0, 0xAC, 0x20, 0, 0, 0, 0, 0, 0};
#endif

	// UTF16 to UTF8
	Common::String resultstr8 = Common::U32String::decodeUTF16LE((uint16 *) utf16le, 3).encode(Common::kUtf8);
	if (resultstr8.c_str() == NULL || memcmp(resultstr8.c_str(), utf8, 7)) {
		Testsuite::logPrintf("UTF-16 to UTF-8 conversion differs from the expected result.");
		return kTestFailed;
	}

#if 0
	// UTF32 to UTF8
	converter.setFrom("UTF-32LE");

	result = converter.convert((char *) utf32le, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf8, 7)) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-8", "UTF-32LE", (char *) utf32le, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf8, 7)) {
		Testsuite::logPrintf("UTF-32 to UTF-8 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF32 to UTF16
	converter.setTo("UTF-16LE");

	result = converter.convert((char *) utf32le, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16le, 8)) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-16LE", "UTF-32LE", (char *) utf32le, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16le, 8)) {
		Testsuite::logPrintf("UTF-32 to UTF-16 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);
#endif
	// UTF8 to UTF16
	uint16 *result16 = Common::U32String((char *) utf8, 6, Common::kUtf8).encodeUTF16LE(NULL);
	if (result16 == NULL) {
		Testsuite::logPrintf("UTF-8 to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result16, utf16le, 8)) {
		Testsuite::logPrintf("UTF-8 to UTF-16 conversion differs from the expected result.");
		free(result16);
		return kTestFailed;
	}
	free(result16);
	
#if 0

	// UTF8 to UTF32
	converter.setTo("UTF-32LE");

	result = converter.convert((char *) utf8, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32le, 16)) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-32LE", "UTF-8", (char *) utf8, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32le, 16)) {
		Testsuite::logPrintf("UTF-8 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF16 to UTF32
	converter.setFrom("UTF-16LE");

	result = converter.convert((char *) utf16le, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32le, 16)) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	result = Common::Encoding::convert("UTF-32LE", "UTF-16LE", (char *) utf16le, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32le, 16)) {
		Testsuite::logPrintf("UTF-16 to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF16 to UTF16
	converter.setFrom("UTF-16LE");
	converter.setTo("UTF-16");

	result = converter.convert((char *) utf16le, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16LE to UTF-16 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16, 8)) {
		Testsuite::logPrintf("UTF-16LE to UTF-16 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	converter.setFrom("UTF-16");
	converter.setTo("UTF-16LE");

	result = converter.convert((char *) utf16, 6);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-16 to UTF-16LE conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf16le, 8)) {
		Testsuite::logPrintf("UTF-16 to UTF-16LE conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// UTF32 to UTF32
	converter.setFrom("UTF-32LE");
	converter.setTo("UTF-32");

	result = converter.convert((char *) utf32le, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32LE to UTF-32 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32, 16)) {
		Testsuite::logPrintf("UTF-32LE to UTF-32 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	converter.setFrom("UTF-32");
	converter.setTo("UTF-32LE");

	result = converter.convert((char *) utf32, 12);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-32 to UTF-32LE conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf32le, 16)) {
		Testsuite::logPrintf("UTF-32 to UTF-32LE conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);
#endif
	return kTestPassed;
}

#if 0
TestExitStatus Encodingtests::testCyrillicTransliteration() {
	Common::String info = "Cyrillic transliteration test. Multiple conversions between unicode, iso-8859-5 and ASCII will be performed.";

	Common::Point pt(0, 100);
	Testsuite::writeOnScreen("Testing Cyrillic transliteration", pt);

	if (Testsuite::handleInteractiveInput(info, "OK", "Skip", kOptionRight)) {
		Testsuite::logPrintf("Info! Skipping test : testCyrillicTransliteration\n");
		return kTestSkipped;
	}
	unsigned char utf8[] = {/* Z */0xD0, 0x97, /* d */ 0xD0, 0xB4, /* r */ 0xD1, 0x80, /* a */ 0xD0, 0xB0, /* v */ 0xD0, 0xB2, /* s */ 0xD1, 0x81, /* t */ 0xD1, 0x82, /* v */ 0xD0, 0xB2, /* u */ 0xD1, 0x83, /* j */ 0xD0, 0xB9, /* t */ 0xD1, 0x82, /* e */ 0xD0, 0xB5, 0};
	unsigned char iso_8859_5[] = {0xB7, 0xD4, 0xE0, 0xD0, 0xD2, 0xE1, 0xE2, 0xD2, 0xE3, 0xD9, 0xE2, 0xD5, 0};
	unsigned char ascii[] = "Zdravstvujte";

	Common::Encoding converter("ASCII", "UTF-8");
	char *result = converter.convert((char *)utf8, 24);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-8 to ASCII conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, ascii, 13)) {
		Testsuite::logPrintf("UTF-8 to ASCII conversion isn'differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	converter.setFrom("iso-8859-5");
	result = converter.convert((char *)iso_8859_5, 12);
	if (result == NULL) {
		Testsuite::logPrintf("iso-8859-5 to ASCII conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, ascii, 13)) {
		Testsuite::logPrintf("iso-8859-5 to ASCII conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	converter.setTo("UTF-8");
	result = converter.convert((char *)iso_8859_5, 12);
	if (result == NULL) {
		Testsuite::logPrintf("iso-8859-5 to UTF-8 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, utf8, 25)) {
		Testsuite::logPrintf("iso-8859-5 to UTF-differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	converter.setTo("iso-8859-5");
	converter.setFrom("UTF-8");
	result = converter.convert((char *)utf8, 24);
	if (result == NULL) {
		Testsuite::logPrintf("UTF-8 to iso-8859-5 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, iso_8859_5, 13)) {
		Testsuite::logPrintf("UTF-8 to iso-8859-differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);

	// this should stay the same
	converter.setFrom("ASCII");
	result = converter.convert((char *)ascii, 12);
	if (result == NULL) {
		Testsuite::logPrintf("ASCII to iso-8859-5 conversion isn't available");
		return kTestFailed;
	}
	if (memcmp(result, ascii, 13)) {
		Testsuite::logPrintf("ASCII to iso-8859-5 conversion differs from the expected result.");
		free(result);
		return kTestFailed;
	}
	free(result);
	return kTestPassed;
}
#endif

TestExitStatus Encodingtests::testOtherConversions() {
	Common::String info = "Other conversions test. Some regular encoding conversions will be performed.";

	Common::Point pt(0, 100);
	Testsuite::writeOnScreen("Testing other encoding conversions", pt);

	if (Testsuite::handleInteractiveInput(info, "OK", "Skip", kOptionRight)) {
		Testsuite::logPrintf("Info! Skipping test : testOtherConversions\n");
		return kTestSkipped;
	}
	unsigned char cp850[] = {0x99, 0xE0, 0xEA, 0x41, 0x64, 0};
	unsigned char utf8_1[] = {0xC3, 0x96, 0xC3, 0x93, 0xC3, 0x9B, 0x41, 0x64, 0};

	unsigned char iso_8859_2[] = {0xA9, 0xE1, 0x6C, 0x65, 0xE8, 0x65, 0x6B, 0};
	unsigned char utf8_2[] = {0xC5, 0xA0, 0xC3, 0xA1, 0x6C, 0x65, 0xC4, 0x8D, 0x65, 0x6B, 0};

	Common::String result = Common::U32String((const char *) cp850, 5, Common::kDos850).encode(Common::kUtf8);
	if (result.c_str() == nullptr || memcmp(result.c_str(), utf8_1, 9)) {
		Testsuite::logPrintf("CP850 to UTF-8 conversion isn'differs from the expected result.");
		return kTestFailed;
	}

	result = Common::U32String((const char *) utf8_1, 8, Common::kUtf8).encode(Common::kDos850);
	if (result.c_str() == nullptr || memcmp(result.c_str(), cp850, 6)) {
		Testsuite::logPrintf("CP850 to UTF-8 conversion isn'differs from the expected result.");
		return kTestFailed;
	}

	result = Common::U32String((const char *) iso_8859_2, 7, Common::kISO8859_2).encode(Common::kUtf8);
	if (result.c_str() == nullptr || memcmp(result.c_str(), utf8_2, 11)) {
		Testsuite::logPrintf("CP850 to UTF-8 conversion isn'differs from the expected result.");
		return kTestFailed;
	}

	result = Common::U32String((const char *) utf8_2, 11, Common::kUtf8).encode(Common::kISO8859_2);
	if (result.c_str() == nullptr || memcmp(result.c_str(), iso_8859_2, 8)) {
		Testsuite::logPrintf("CP850 to UTF-8 conversion isn'differs from the expected result.");
		return kTestFailed;
	}

	return kTestPassed;
}

EncodingTestSuite::EncodingTestSuite() {
	addTest("testConversionUnicodeMachineEndian", &Encodingtests::testConversionUnicodeMachineEndian, true);
	addTest("testConversionUnicodeBigEndian", &Encodingtests::testConversionUnicodeBigEndian, true);
	addTest("testConversionUnicodeLittleEndian", &Encodingtests::testConversionUnicodeLittleEndian, true);
#if 0
	addTest("testCyrillicTransliteration", &Encodingtests::testCyrillicTransliteration, true);
#endif
	addTest("testOtherConversions", &Encodingtests::testOtherConversions, true);
}

} // End of namespace Testbed
