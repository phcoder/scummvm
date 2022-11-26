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

#ifndef COMMON_CONCATSTREAM_H
#define COMMON_CONCATSTREAM_H

#include "common/stream.h"

/*
 * ConcatReadStream provides access to a virtually concatenated stream.
 *
 * Manipulating the parent stream directly /will/ mess up a substream.
 * @see SubReadStream
 */
class ConcatReadStream : public SeekableReadStream {
protected:
	Common::Array<Common::SharedPtr<Common::SeekableReadStream>> _parentStreams;
	uint32 _totalSize, _linearPos;
	uint32 _volume, _volumePos;
	bool _err, _eos;
public:
	ConcatReadStream(Common::Array<Common::SharedPtr<Common::SeekableReadStream>> parentStreams);

	int64 pos() const override { return _linearPos; }
	int64 size() const override { return _totalSize; }
	bool eos() const override { return _eos; }
	bool err() const override { return _err; }
	void clearErr() override {
		_err = false;
		_eos = false;
	}
	bool seek(int64 offset, int whence = SEEK_SET) override;
	uint32 read(void *dataPtr, uint32 dataSize) override;
};

#endif
