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

#ifndef BACKENDS_GRAPHICS_DOS_H
#define BACKENDS_GRAPHICS_DOS_H

#include "backends/graphics/graphics.h"
#include "common/events.h"
#include "graphics/surface.h"

struct BITMAP;

class DosGraphicsManager : public GraphicsManager/*, Common::EventObserver*/ {
public:
	DosGraphicsManager();
	void setPalette(const byte *colors, uint start, uint num) override;
	void grabPalette(byte *colors, uint start, uint num) const override;
	bool hasFeature(OSystem::Feature f) const override;
	void setFeatureState(OSystem::Feature f, bool enable) override;
	bool getFeatureState(OSystem::Feature f) const override;
	void initSize(uint, uint, const Graphics::PixelFormat*) override;
	int getScreenChangeID() const override;
	void beginGFXTransaction() override;
	OSystem::TransactionError endGFXTransaction() override;
	int16 getHeight() const override { return _currentState.height; }
	int16 getWidth() const override { return _currentState.width; }
	void copyRectToScreen(const void *buf, int pitch, int x, int y, int w, int h) override;
	Graphics::Surface *lockScreen() override;
	void unlockScreen() override;
	void fillScreen(uint32 col) override;
	void fillScreen(const Common::Rect &r, uint32 col) override;
	void updateScreen() override;
	void setShakePos(int shakeXOffset, int shakeYOffset) override;
	void setFocusRectangle(const Common::Rect& rect) override;
	void clearFocusRectangle() override;
	void showOverlay(bool inGUI) override;
	void hideOverlay() override;
	bool isOverlayVisible() const override { return _overlayVisible; }
	Graphics::PixelFormat getOverlayFormat() const override;
	void clearOverlay() override;
	void grabOverlay(Graphics::Surface &surface) const override;
	void copyRectToOverlay(const void *buf, int pitch, int x, int y, int w, int h) override;
	int16 getOverlayHeight() const override { return kOverlayHeight; }
	int16 getOverlayWidth() const override { return kOverlayWidth; }
	bool showMouse(bool visible) override;
	void warpMouse(int x, int y) override;
	void setMouseCursor(const void *buf, uint w, uint h, int hotspotX, int hotspotY, uint32 keycolor, bool dontScale = false, const Graphics::PixelFormat *format = nullptr, const byte *mask = nullptr) override;
	void setCursorPalette(const byte *colors, uint start, uint num) override;
	void moveCursor(int x, int y);

private:
	void applyNormalPalette();
	void drawWithSave(const void *buf, int pitch, int x, int y, int w, int h);
	void clearScreen(uint8_t col);
	void redrawRect(int x, int y, int w, int h);
	void drawMaskedNoSave(const void *buf, const byte *mask, int pitch, int maskPitch, int x, int y, int w, int h);
	void undrawCursor();
	void drawCursor();

	enum class GraphicsMode : int {
		Unknown			= -1,
		DirectRendering = 0,
		SingleBuffering = 1,
		TripleBuffering = 3
	};
	struct GraphicsState {
		GraphicsMode mode = GraphicsMode::Unknown;
		int width = 0;
		int height = 0;
		Graphics::PixelFormat format;
		bool aspectRatioCorrection = false;

		enum PendingScreenChange {
			kNone					= 0,
			kVideoMode				= 1<<0,
			kScreenAddress			= 1<<1,
			kPalette				= 1<<2,
			kAspectRatioCorrection	= 1<<3,
			kShakeScreen            = 1<<4,
			kAll					= kVideoMode | kScreenAddress | kPalette | kAspectRatioCorrection | kShakeScreen,
		};
		int change = kNone;
	};

	static constexpr int kOverlayWidth = 640;
	static constexpr int kOverlayHeight = 480;

	GraphicsState _pendingState;
	GraphicsState _currentState;
	bool _overlayVisible = false;
	Graphics::Surface _overlaySurface;
	Graphics::Surface _surface;
	byte _normalPalette[3 * 256];
	BITMAP *_mouseCursorBitmap = nullptr;

	int _cursorWidth, _cursorHeight, _cursorHotspotX, _cursorHotspotY;
	int _mouseX, _mouseY;
	byte *_cursorBuf, *_cursorMask;
	bool _mouseVisible = false;
};

#endif
