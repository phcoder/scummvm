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

#define FORBIDDEN_SYMBOL_ALLOW_ALL
#include "backends/graphics/dos/dos-graphics.h"
#include <allegro.h>

static const Graphics::PixelFormat kOverlayFormat(1, 3, 3, 2, 0, 5, 2, 0, 0);

DosGraphicsManager::DosGraphicsManager() {
	_overlaySurface.create(kOverlayWidth, kOverlayHeight, kOverlayFormat);
	for (int i = 0; i < 256; i++) {
		_normalPalette[3 * i] = desktop_palette[i].r << 2;
		_normalPalette[3 * i + 1] = desktop_palette[i].g << 2;
		_normalPalette[3 * i + 2] = desktop_palette[i].b << 2;
	}
}

void DosGraphicsManager::applyNormalPalette() {
	PALETTE pal;
	for (int i = 0; i < 256; i++) {
		pal[i].r = _normalPalette[3 * i] >> 2;
		pal[i].g = _normalPalette[3 * i + 1] >> 2;
		pal[i].b = _normalPalette[3 * i + 2] >> 2;
		pal[i].filler = 0;
	}
	set_palette(pal);
}

void DosGraphicsManager::setPalette(const byte *colors, uint start, uint num) {
	memcpy(_normalPalette + 3 * start, colors, 3 * num);
	if (!_overlayVisible) {
		applyNormalPalette();
	}
}
void DosGraphicsManager::grabPalette(byte *colors, uint start, uint num) const {
	memcpy(colors, _normalPalette + 3 * start, 3 * num);
}

bool DosGraphicsManager::hasFeature(OSystem::Feature f) const {
	return false;
}

void DosGraphicsManager::setFeatureState(OSystem::Feature f, bool enable) {
}

bool DosGraphicsManager::getFeatureState(OSystem::Feature f) const {
	return false;
}

void DosGraphicsManager::initSize(uint width, uint height, const Graphics::PixelFormat *format) {
	debug("initSize: %d, %d, %d", width, height, format ? format->bytesPerPixel : 1);

	_pendingState.width = width;
	_pendingState.height = height;
	_pendingState.format = format ? *format : Graphics::PixelFormat::createFormatCLUT8();

	if ((_pendingState.width > 0 && _pendingState.height > 0)
		&& (_currentState.width != _pendingState.width || _currentState.height != _pendingState.height)) {
		_pendingState.change |= GraphicsState::kVideoMode;
	}
}

int DosGraphicsManager::getScreenChangeID() const { return 0; }

void DosGraphicsManager::beginGFXTransaction() {
	debug("beginGFXTransaction");

	// these serve as a flag whether we are launching a game; if not, they will be always zeroed
	_pendingState.width = 0;
	_pendingState.height = 0;
	_pendingState.change &= ~GraphicsState::kVideoMode;
}

OSystem::TransactionError DosGraphicsManager::endGFXTransaction() {
	debug("endGFXTransaction");

	int error = OSystem::TransactionError::kTransactionSuccess;

	if (_pendingState.change & GraphicsState::kVideoMode) {
		set_color_depth(_pendingState.format.bytesPerPixel * 8);
		if (set_gfx_mode(GFX_AUTODETECT, _pendingState.width, _pendingState.height, 0, 0) == 0) {
		        _surface.create(_pendingState.width, _pendingState.height, _pendingState.format);
			// TODO: check RGB vs BGR differences
			_currentState = _pendingState;
			applyNormalPalette();
			_overlayVisible = false;
		} else {
			debug("Failed to start gfx: %s",  allegro_error);
			error |= OSystem::TransactionError::kTransactionFormatNotSupported;
		}
	}

	debug("endGFXTransaction done: %d", error);

	return (OSystem::TransactionError)error;
}

void DosGraphicsManager::drawWithSave(const void *buf, int pitch, int x, int y, int w, int h) {
	Graphics::Surface *saveSurface = _overlayVisible ? &_overlaySurface : &_surface;
	int bytesPerPixel = _overlayVisible ? 1 : _currentState.format.bytesPerPixel;
	if (x < 0) {
		buf = (const char*) buf + (-x) * bytesPerPixel;
		w += x;
		x = 0;
	}
	if (y < 0) {
		buf = (const char*) buf + (-y) * pitch;
		h += y;
		y = 0;
	}
	if (x + w >= saveSurface->w) {
		w = saveSurface->w - x;
	}
	if (y + h >= saveSurface->h) {
		h = saveSurface->h - y;
	}
	bmp_select(screen);
	for (int line = 0; line < h; line++) {
		unsigned long dst = bmp_write_line(screen, line + y) + x;
		uint8_t *cleanDst = (uint8_t *) saveSurface->getBasePtr(x, y + line);
		const uint8_t *src = (const uint8_t *) buf + pitch * line;
		for (int i = 0; i < w * bytesPerPixel; i++) {
			*cleanDst++ = *src;
			bmp_write8(dst++, *src++);
		}
		bmp_unwrite_line(screen);
	}
}

void DosGraphicsManager::drawMaskedNoSave(const void *buf, const byte *mask, int pitch, int maskPitch, int x, int y, int w, int h) {
	Graphics::Surface *saveSurface = _overlayVisible ? &_overlaySurface : &_surface;
	int bytesPerPixel = _overlayVisible ? 1 : _currentState.format.bytesPerPixel;
	if (x < 0) {
		buf = (const char*) buf + (-x) * bytesPerPixel;
		mask += (-x);
		w += x;
		x = 0;
	}
	if (y < 0) {
		buf = (const char*) buf + (-y) * pitch;
		mask += (-y) * maskPitch;
		h += y;
		y = 0;
	}
	if (x + w >= saveSurface->w) {
		w = saveSurface->w - x;
	}
	if (y + h >= saveSurface->h) {
		h = saveSurface->h - y;
	}
	bmp_select(screen);
	for (int line = 0; line < h; line++) {
		unsigned long dst = bmp_write_line(screen, line + y) + x * bytesPerPixel;
		const uint8_t *src = (const uint8_t *) buf + pitch * line;
		const uint8_t *maskPtr = (const uint8_t *) mask + maskPitch * line;		
		for (int i = 0; i < w; i++) {
			// TODO: other masks.
			if (*maskPtr) {
				for (int j = 0; j < bytesPerPixel; j++)
					bmp_write8(dst + j, src[j]);
			}
			src+=bytesPerPixel; dst+=bytesPerPixel; maskPtr++;
		}
		bmp_unwrite_line(screen);
	}
}

void DosGraphicsManager::redrawRect(int x, int y, int w, int h) {
	Graphics::Surface *saveSurface = _overlayVisible ? &_overlaySurface : &_surface;
	bmp_select(screen);
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (x + w >= saveSurface->w) {
		w = saveSurface->w - x;
	}
	if (y + h >= saveSurface->h) {
		h = saveSurface->h - y;
	}
	for (int line = 0; line < h; line++) {
		unsigned long dst = bmp_write_line(screen, line + y) + x * saveSurface->format.bytesPerPixel;
		const uint8_t *src = (const uint8_t *) saveSurface->getBasePtr(x, y + line);
		for (int i = 0; i < w * saveSurface->format.bytesPerPixel; i++) {
			bmp_write8(dst++, *src++);
		}
		bmp_unwrite_line(screen);
	}
}

void DosGraphicsManager::clearScreen(uint8_t col) {
	int bytesPerPixel = _overlayVisible ? 1 : _currentState.format.bytesPerPixel;
	int h = _overlayVisible ? kOverlayHeight : _currentState.height;
	int w = _overlayVisible ? kOverlayWidth : _currentState.width;
	bmp_select(screen);
	for (int line = 0; line < h; line++) {
		unsigned long dst = bmp_write_line(screen, line);
		for (int i = 0; i < w * bytesPerPixel; i++) {
			bmp_write8(dst++, col);
		}
		bmp_unwrite_line(screen);
	}
}

void DosGraphicsManager::copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) {
	if (!_overlayVisible) {
		drawWithSave(buf, pitch, x, y, w, h);
	} else {
		_surface.copyRectToSurface(buf, pitch, x, y, w, h);
	}
}

Graphics::Surface *DosGraphicsManager::lockScreen() {
	debug(__FILE__ ":%d", __LINE__);
	return &_surface;
}

void DosGraphicsManager::unlockScreen() { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::fillScreen(uint32 col) {
	if (!_overlayVisible)
		clearScreen(col);
	_surface.fillRect(Common::Rect (0, 0, _surface.w, _surface.h), col);	
}

void DosGraphicsManager::fillScreen(const Common::Rect &r, uint32 col) {
	_surface.fillRect(r, col);
	redrawRect(r.left, r.top, r.width(), r.height());
}

void DosGraphicsManager::updateScreen() { }
void DosGraphicsManager::setShakePos(int shakeXOffset, int shakeYOffset) { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::setFocusRectangle(const Common::Rect& rect) { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::clearFocusRectangle() { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::showOverlay(bool inGUI) {
	if (_overlayVisible) {
		return;
	}
	PALETTE rgb332;
	set_color_depth(8);
	set_gfx_mode(GFX_AUTODETECT, kOverlayWidth, kOverlayHeight, 0, 0);
	for (int i = 0; i < 256; i++) {
		rgb332[i] = desktop_palette[i];
		rgb332[i].r = (i & 0xe0) >> 2;
		rgb332[i].g = (i & 0x1c) << 1;
		rgb332[i].b = (i & 3) << 4;
	}
	set_palette(rgb332);
	_overlayVisible = true;

	redrawRect(0, 0, _overlaySurface.w, _overlaySurface.h);
}

void DosGraphicsManager::hideOverlay() {
	if (!_overlayVisible) {
		return;
	}

	set_color_depth(_currentState.format.bytesPerPixel * 8);
	if (set_gfx_mode(GFX_AUTODETECT, _currentState.width, _currentState.height, 0, 0) != 0) {
		debug("Unexpected mode set failure for mode[%dx%dx%d]: %s", _currentState.width, _currentState.height, _currentState.format.bytesPerPixel * 8, allegro_error);
	}
	applyNormalPalette();

	_overlayVisible = false;
	redrawRect(0, 0, _surface.w, _surface.h);
}

Graphics::PixelFormat DosGraphicsManager::getOverlayFormat() const {
	return kOverlayFormat;
}

void DosGraphicsManager::clearOverlay() {
	_overlaySurface.fillRect(Common::Rect (0, 0, kOverlayWidth, kOverlayHeight), 0);
	if (_overlayVisible) {
		clearScreen(0);
	}
}

void DosGraphicsManager::grabOverlay(Graphics::Surface &surface) const {
	surface.create(kOverlayWidth, kOverlayHeight, Graphics::PixelFormat(1, 3, 3, 2, 0, 5, 2, 0, 0));
	surface.copyRectToSurface(_overlaySurface, 0, 0, Common::Rect(0,0, kOverlayWidth, kOverlayHeight));
}

void DosGraphicsManager::copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) {
	if (_overlayVisible) {
		drawWithSave(buf, pitch, x, y, w, h);
	} else {
		_overlaySurface.copyRectToSurface(buf, pitch, x, y, w, h);
	}
}
bool DosGraphicsManager::showMouse(bool visible) {
	if (_mouseVisible == visible)
		return true;
	_mouseVisible = visible;
	if (_mouseVisible)
		drawCursor();
	else
		undrawCursor();
	return true;
}

void DosGraphicsManager::warpMouse(int x, int y) {
	position_mouse(x, y);
	moveCursor(x, y);
}

void DosGraphicsManager::undrawCursor() {
	redrawRect(_mouseX - _cursorHotspotX, _mouseY - _cursorHotspotY,
		   _cursorWidth, _cursorHeight);
}

void DosGraphicsManager::drawCursor() {
	if (!_mouseVisible)
		return;
	drawMaskedNoSave(_cursorBuf, _cursorMask,
			 _cursorWidth, _cursorWidth, _mouseX - _cursorHotspotX, _mouseY - _cursorHotspotY,
			 _cursorWidth, _cursorHeight);
}

void DosGraphicsManager::moveCursor(int x, int y) {
	if ((_mouseX == x && _mouseY == y) || !_mouseVisible)
		return;

	undrawCursor();
	_mouseX = x;
	_mouseY = y;
	drawCursor();
}

void DosGraphicsManager::setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor,
					bool dontScale, const Graphics::PixelFormat *format, const byte *mask) {
	debug("setMouseCursor hotspot=(%d, %d)", hotspotX, hotspotY);
	undrawCursor();

	// TODO: Support non-8bit cursors

	delete[] _cursorBuf;
	delete[] _cursorMask;
	_cursorBuf = new byte[w * h];
	_cursorMask = new byte[w * h];
	memcpy(_cursorBuf, buf, w * h);
	if (mask)
		memcpy(_cursorMask, mask, w * h);
	else
		memset(_cursorMask, kCursorMaskOpaque, w * h);
	for (uint i = 0; i < w * h; i++)
		if (((const byte *)buf)[i] == keycolor)
			_cursorMask[i] = kCursorMaskTransparent;
	_cursorWidth = w;
	_cursorHeight = h;
	_cursorHotspotX = hotspotX;
	_cursorHotspotY = hotspotY;
	drawCursor();
}

void DosGraphicsManager::setCursorPalette(const byte *colors, uint start, uint num) { debug(__FILE__ ":%d", __LINE__); }

Graphics::PixelFormat DosGraphicsManager::getScreenFormat() const {
	return _currentState.format;
}

Common::List<Graphics::PixelFormat> DosGraphicsManager::getSupportedFormats() const {
	Common::List<Graphics::PixelFormat> ret;
	// TODO: Fill this programmatically
	ret.push_back(Graphics::PixelFormat(2, 5, 6, 5, 0, 11, 5, 0, 0));
	return ret;
}
