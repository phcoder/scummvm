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

#include "common/array.h"
#include "common/gzio.h"
#include "common/debug.h"
#include "common/ptr.h"
#include "common/substream.h"
#include "common/memstream.h"
#include "common/rnc_deco.h"

#include "dreamweb/rnca_archive.h"

namespace DreamWeb {

RNCAArchive* RNCAArchive::open(Common::SeekableReadStream *stream, DisposeAfterUse::Flag dispose) {
	FileMap files;

	if (stream->readUint32BE() != 0x524e4341)
		return nullptr;

	uint16 metadataSize1 = stream->readUint16BE();
	stream->readUint16BE(); // No ida
	uint16 metadataSize2 = stream->readUint16BE();

	if (metadataSize1 != metadataSize2 || metadataSize1 < 15)
		return nullptr;

	while(stream->pos() <= metadataSize1 - 5) {
		Common::String fileName = stream->readString(0, metadataSize1 - 4 - stream->pos());
		uint32 off = stream->readUint32BE();
		files[fileName] = RNCAFileDescriptor(fileName, off);
	}

	return new RNCAArchive(files, stream, dispose);
}

static Common::String translateName(const Common::Path &path) {
	return Common::normalizePath(path.toString('\\'), '\\');
}

bool RNCAArchive::hasFile(const Common::Path &path) const {
	return _files.contains(translateName(path));
}

int RNCAArchive::listMembers(Common::ArchiveMemberList &list) const {
	for (FileMap::const_iterator i = _files.begin(), end = _files.end(); i != end; ++i) {
		list.push_back(Common::ArchiveMemberList::value_type(new Common::GenericArchiveMember(i->_key, this)));
	}

	return _files.size();
}

const Common::ArchiveMemberPtr RNCAArchive::getMember(const Common::Path &path) const {
	Common::String translated = translateName(path);
	if (!_files.contains(translated))
		return nullptr;

	return Common::ArchiveMemberPtr(new Common::GenericArchiveMember(_files.getVal(translated)._fileName, this));
}

// TODO: Make streams stay valid after destructing of archive
Common::SeekableReadStream *RNCAArchive::createReadStreamForMember(const Common::Path &path) const {
	Common::String translated = translateName(path);
	if (!_files.contains(translated))
		return nullptr;
	const RNCAFileDescriptor& desc = _files.getVal(translated);
	if (_cache.contains(desc._fileName)) {
		return new Common::MemoryReadStream(&_cache[desc._fileName][0], _cache[desc._fileName].size(), DisposeAfterUse::NO);
	}

	_stream->seek(desc._fileDataOffset);

	if (_stream->readUint32BE() != Common::RncDecoder::kRnc1Signature) {
		_cache[desc._fileName].clear();
		return new Common::MemoryReadStream(&_cache[desc._fileName][0], 0, DisposeAfterUse::NO);
	}

	// read unpacked/packed file length
	uint32 unpackLen = _stream->readUint32BE();
	uint32 packLen = _stream->readUint32BE();

	if (unpackLen > 0x7ffff000 || packLen > 0x7ffff000) {
		_cache[desc._fileName].clear();
		return new Common::MemoryReadStream(&_cache[desc._fileName][0], 0, DisposeAfterUse::NO);
	}

	// Rewind back the header
	_stream->seek(desc._fileDataOffset);
	packLen += 0x12;

	byte *compressedBuffer = new byte[packLen];
	_stream->read(compressedBuffer, packLen);
	_cache[desc._fileName].resize(unpackLen);

	Common::RncDecoder rnc;
	
	if (rnc.unpackM1(compressedBuffer, packLen,
			 &_cache[desc._fileName][0]) != (int32) unpackLen) {
		_cache[desc._fileName].clear();
		return new Common::MemoryReadStream(&_cache[desc._fileName][0], 0, DisposeAfterUse::NO);
	}

	return new Common::MemoryReadStream(&_cache[desc._fileName][0], 0, DisposeAfterUse::NO);
}

}
