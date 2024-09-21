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

void DosGraphicsManager::setPalette(const byte *colors, uint start, uint num) { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::grabPalette(byte *colors, uint start, uint num) const { debug(__FILE__ ":%d", __LINE__); }
bool DosGraphicsManager::hasFeature(OSystem::Feature f) const {
	debug(__FILE__ ":%d", __LINE__);
	return false;
}
void DosGraphicsManager::setFeatureState(OSystem::Feature f, bool enable) { debug(__FILE__ ":%d", __LINE__); }
bool DosGraphicsManager::getFeatureState(OSystem::Feature f) const { debug(__FILE__ ":%d", __LINE__); return false; }
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
			set_palette(desktop_palette);
			clear_to_color(screen, makecol(0xff,0xff,0xff));
			bmp_select(screen);
			unsigned long addr = bmp_write_line(screen, 0);
			bmp_write8(addr, 0xff);
			bmp_unwrite_line(screen);
		} else {
			debug("Failed to start gfx: %s",  allegro_error);
			error |= OSystem::TransactionError::kTransactionFormatNotSupported;
		}
	}

	debug("endGFXTransaction done: %d", error);

	return (OSystem::TransactionError)error;
}

void DosGraphicsManager::copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) { debug(__FILE__ ":%d", __LINE__); }
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
	PALETTE rgb332;
	set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);
	for (int i = 0; i < 256; i++) {
		rgb332[i].r = i & 0xe0;
		rgb332[i].g = (i & 0x1c) << 3;
		rgb332[i].b = (i & 3) << 6;
		rgb332[i].filler = i;
	}
	set_palette(rgb332);
	_overlayVisible = true;
}
void DosGraphicsManager::hideOverlay() { debug(__FILE__ ":%d", __LINE__); }
Graphics::PixelFormat DosGraphicsManager::getOverlayFormat() const {
	return Graphics::PixelFormat(1, 3, 3, 2, 0, 5, 2, 0, 0);
}
void DosGraphicsManager::clearOverlay() { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::grabOverlay(Graphics::Surface &surface) const { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) {
	debug("copyRectToOverlay pitch=%d %dx%d + %dx%d", pitch, x, y, w, h);
	set_color_depth(8);
	bmp_select(screen);
	uint32_t sum = 0;
	for (int line = 0; line < h; line++) {
		unsigned long dst = bmp_write_line(screen, line + y) + x;
		const uint8_t *src = (const uint8_t *) buf + pitch * line;
		for (int i = 0; i < w; i++) {
			sum += *src;
			bmp_write8(dst++, *src++);
		}
		bmp_unwrite_line(screen);
	}
	debug("sum=%x", sum);
}
bool DosGraphicsManager::showMouse(bool visible) { debug(__FILE__ ":%d", __LINE__); return false; }
void DosGraphicsManager::warpMouse(int x, int y) { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, bool dontScale, const Graphics::PixelFormat *format, const byte *mask) { debug(__FILE__ ":%d", __LINE__); }
void DosGraphicsManager::setCursorPalette(const byte *colors, uint start, uint num) { debug(__FILE__ ":%d", __LINE__); }
