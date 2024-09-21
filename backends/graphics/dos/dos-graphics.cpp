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

DosGraphicsManager::DosGraphicsManager() {
	memset (_overlay, 0, kOverlayHeight * kOverlayWidth);
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

int DosGraphicsManager::getScreenChangeID() const { debug(__FILE__ ":%d", __LINE__); return 0; }

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

void DosGraphicsManager::copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) {
	if (!_overlayVisible) {
		bmp_select(screen);
		scare_mouse_area(x, y, w, h);
		for (int line = 0; line < h; line++) {
			unsigned long dst = bmp_write_line(screen, line + y) + x;
			uint8_t *cleanDst = (uint8_t *) _surface.getBasePtr(x, y + line);
			const uint8_t *src = (const uint8_t *) buf + pitch * line;
			for (int i = 0; i < w * _currentState.format.bytesPerPixel; i++) {
				*cleanDst++ = *src;
				bmp_write8(dst++, *src++);
			}
			bmp_unwrite_line(screen);
		}
		unscare_mouse();
	} else {
		_surface.copyRectToSurface(buf, pitch, x, y, w, h);
	}
}

Graphics::Surface *DosGraphicsManager::lockScreen() {
	debug(__FILE__ ":%d", __LINE__);
	return &_surface;
}

void DosGraphicsManager::unlockScreen() { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::fillScreen(uint32 col) { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::fillScreen(const Common::Rect &r, uint32 col) { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::updateScreen() { debug(__FILE__ ":%d", __LINE__); }
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

	bmp_select(screen);
	scare_mouse();
	for (int line = 0; line < kOverlayHeight; line++) {
		unsigned long dst = bmp_write_line(screen, line);
		const uint8_t *src = (const uint8_t *) _overlay + line * kOverlayWidth;
		for (int i = 0; i < kOverlayWidth; i++) {
			bmp_write8(dst++, *src++);
		}
		bmp_unwrite_line(screen);
	}
	unscare_mouse();
}

void DosGraphicsManager::hideOverlay() {
	if (!_overlayVisible) {
		return;
	}

	set_color_depth(_currentState.format.bytesPerPixel * 8);
	set_gfx_mode(GFX_AUTODETECT, _currentState.width, _currentState.height, 0, 0);
	applyNormalPalette();

	_overlayVisible = false;

	bmp_select(screen);
	scare_mouse();
	for (int line = 0; line < _currentState.height; line++) {
		unsigned long dst = bmp_write_line(screen, line);
		const uint8_t *src = (const uint8_t *) _surface.getBasePtr(0, line);
		for (int i = 0; i < _currentState.width * _currentState.format.bytesPerPixel; i++) {
			bmp_write8(dst++, *src++);
		}
		bmp_unwrite_line(screen);
	}
	unscare_mouse();
}

Graphics::PixelFormat DosGraphicsManager::getOverlayFormat() const {
	return Graphics::PixelFormat(1, 3, 3, 2, 0, 5, 2, 0, 0);
}

void DosGraphicsManager::clearOverlay() {
	memset(_overlay, 0, kOverlayWidth * kOverlayHeight);
}

void DosGraphicsManager::grabOverlay(Graphics::Surface &surface) const {
	surface.create(kOverlayWidth, kOverlayHeight, Graphics::PixelFormat(1, 3, 3, 2, 0, 5, 2, 0, 0));
	surface.copyRectToSurface(_overlay, kOverlayWidth, 0, 0, kOverlayWidth, kOverlayHeight);
}

void DosGraphicsManager::copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) {
	if (_overlayVisible) {
		bmp_select(screen);
		scare_mouse_area(x, y, w, h);
		for (int line = 0; line < h; line++) {
			unsigned long dst = bmp_write_line(screen, line + y) + x;
			uint8_t *cleanDst = _overlay + (line + y) * kOverlayWidth + x;
			const uint8_t *src = (const uint8_t *) buf + pitch * line;
			for (int i = 0; i < w; i++) {
				*cleanDst++ = *src;
				bmp_write8(dst++, *src++);
			}
			bmp_unwrite_line(screen);
		}
		unscare_mouse();
	} else {
		for (int line = 0; line < h; line++) {
			memcpy(_overlay + (y + line) * kOverlayWidth + x, (const uint8_t *) buf + pitch * line, w);
		}
	}
}
bool DosGraphicsManager::showMouse(bool visible) {
	show_mouse(visible ? screen : NULL);
	return true;
}

void DosGraphicsManager::warpMouse(int x, int y) {
//	position_mouse(x, y);
}
void DosGraphicsManager::setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor,
					bool dontScale, const Graphics::PixelFormat *format, const byte *mask) {
	debug("setMouseCursor hotspot=(%d, %d)", hotspotX, hotspotY);
	BITMAP *old_mouse_cursor = _mouseCursorBitmap;
	_mouseCursorBitmap = create_bitmap(w, h);
	for (int line = 0; line < h; line++) {
		memcpy(_mouseCursorBitmap->line[line], buf + w * line, w);
	}
	scare_mouse();
	set_mouse_sprite(_mouseCursorBitmap);
	set_mouse_sprite_focus(hotspotX, hotspotY);
	unscare_mouse();
	if (old_mouse_cursor) {
		destroy_bitmap(old_mouse_cursor);
	}
}

void DosGraphicsManager::setCursorPalette(const byte *colors, uint start, uint num) { debug(__FILE__ ":%d", __LINE__); }
