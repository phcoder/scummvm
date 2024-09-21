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
#define LONG_LONG 1
#define ALLEGRO_HAVE_STDINT_H 1

#include <allegro.h>
#include <time.h>
#include <dos.h>

#include "common/scummsys.h"

#include "backends/modular-backend.h"
#include "backends/mutex/null/null-mutex.h"
#include "base/main.h"

#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"
#include "backends/events/default/default-events.h"
#include "backends/mixer/null/null-mixer.h"
#include "backends/graphics/dos/dos-graphics.h"
#include "gui/debugger.h"

#include "backends/fs/posix/posix-fs-factory.h"

namespace {
byte transform_kbd_flags(int flags) {
	byte ret = 0;
	if (flags & KB_SHIFT_FLAG)
		ret |= Common::KBD_SHIFT;
	if (flags & KB_CTRL_FLAG)
		ret |= Common::KBD_CTRL;
	if (flags & KB_ALT_FLAG)
		ret |= Common::KBD_CTRL;
	if (flags & KB_NUMLOCK_FLAG)
		ret |= Common::KBD_NUM;
	if (flags & KB_CAPSLOCK_FLAG)
		ret |= Common::KBD_CAPS;
	if (flags & KB_SCROLOCK_FLAG)
		ret |= Common::KBD_SCRL;
	return ret;
}

char compute_ascii(Common::KeyCode kc, byte flags) {
	if (kc >= Common::KEYCODE_a && kc <= Common::KEYCODE_z) {
		if ((!!(flags & Common::KBD_CAPS)) ^ (!!(flags & Common::KBD_SHIFT)))
			return kc - Common::KEYCODE_a + 'A';
		return kc - Common::KEYCODE_a + 'a';
	}

	return 0;
}

Common::KeyCode AllegroToOSystemKeycode(int scancode) {
	switch (scancode) {
	case KEY_BACKSPACE: return Common::KEYCODE_BACKSPACE;
	case KEY_TAB: return Common::KEYCODE_TAB;
	case KEY_ENTER: return Common::KEYCODE_RETURN;
	case KEY_PAUSE: return Common::KEYCODE_PAUSE;
	case KEY_ESC: return Common::KEYCODE_ESCAPE;
	case KEY_SPACE: return Common::KEYCODE_SPACE;
	case KEY_QUOTE: return Common::KEYCODE_QUOTE;
	case KEY_OPENBRACE: return Common::KEYCODE_LEFTBRACKET;
	case KEY_CLOSEBRACE: return Common::KEYCODE_RIGHTBRACKET;
	case KEY_ASTERISK: return Common::KEYCODE_ASTERISK;
	case KEY_EQUALS: return Common::KEYCODE_PLUS;
	case KEY_COMMA: return Common::KEYCODE_COMMA;
	case KEY_MINUS: return Common::KEYCODE_MINUS;
	case KEY_STOP: return Common::KEYCODE_PERIOD;
	case KEY_SLASH: return Common::KEYCODE_SLASH;
	case KEY_0: return Common::KEYCODE_0;
	case KEY_1: return Common::KEYCODE_1;
	case KEY_2: return Common::KEYCODE_2;
	case KEY_3: return Common::KEYCODE_3;
	case KEY_4: return Common::KEYCODE_4;
	case KEY_5: return Common::KEYCODE_5;
	case KEY_6: return Common::KEYCODE_6;
	case KEY_7: return Common::KEYCODE_7;
	case KEY_8: return Common::KEYCODE_8;
	case KEY_9: return Common::KEYCODE_9;
	case KEY_COLON: return Common::KEYCODE_COLON;
	case KEY_SEMICOLON: return Common::KEYCODE_SEMICOLON;
	case KEY_AT: return Common::KEYCODE_AT;
	case KEY_BACKSLASH: return Common::KEYCODE_BACKSLASH;
	case KEY_BACKQUOTE: return Common::KEYCODE_BACKQUOTE;
	case KEY_A: return Common::KEYCODE_a;
	case KEY_B: return Common::KEYCODE_b;
	case KEY_C: return Common::KEYCODE_c;
	case KEY_D: return Common::KEYCODE_d;
	case KEY_E: return Common::KEYCODE_e;
	case KEY_F: return Common::KEYCODE_f;
	case KEY_G: return Common::KEYCODE_g;
	case KEY_H: return Common::KEYCODE_h;
	case KEY_I: return Common::KEYCODE_i;
	case KEY_J: return Common::KEYCODE_j;
	case KEY_K: return Common::KEYCODE_k;
	case KEY_L: return Common::KEYCODE_l;
	case KEY_M: return Common::KEYCODE_m;
	case KEY_N: return Common::KEYCODE_n;
	case KEY_O: return Common::KEYCODE_o;
	case KEY_P: return Common::KEYCODE_p;
	case KEY_Q: return Common::KEYCODE_q;
	case KEY_R: return Common::KEYCODE_r;
	case KEY_S: return Common::KEYCODE_s;
	case KEY_T: return Common::KEYCODE_t;
	case KEY_U: return Common::KEYCODE_u;
	case KEY_V: return Common::KEYCODE_v;
	case KEY_W: return Common::KEYCODE_w;
	case KEY_X: return Common::KEYCODE_x;
	case KEY_Y: return Common::KEYCODE_y;
	case KEY_Z: return Common::KEYCODE_z;
	case KEY_DEL: return Common::KEYCODE_DELETE;
	case KEY_DEL_PAD: return Common::KEYCODE_KP_PERIOD;
	case KEY_SLASH_PAD: return Common::KEYCODE_KP_DIVIDE;
	case KEY_MINUS_PAD: return Common::KEYCODE_KP_MINUS;
	case KEY_PLUS_PAD: return Common::KEYCODE_KP_PLUS;
	case KEY_ENTER_PAD: return Common::KEYCODE_KP_ENTER;
	case KEY_EQUALS_PAD: return Common::KEYCODE_KP_EQUALS;
	case KEY_UP: return Common::KEYCODE_UP;
	case KEY_DOWN: return Common::KEYCODE_DOWN;
	case KEY_RIGHT: return Common::KEYCODE_RIGHT;
	case KEY_LEFT: return Common::KEYCODE_LEFT;
	case KEY_INSERT: return Common::KEYCODE_INSERT;
	case KEY_HOME: return Common::KEYCODE_HOME;
	case KEY_END: return Common::KEYCODE_END;
	case KEY_PGUP: return Common::KEYCODE_PAGEUP;
	case KEY_PGDN: return Common::KEYCODE_PAGEDOWN;
	case KEY_F1: return Common::KEYCODE_F1;
	case KEY_F2: return Common::KEYCODE_F2;
	case KEY_F3: return Common::KEYCODE_F3;
	case KEY_F4: return Common::KEYCODE_F4;
	case KEY_F5: return Common::KEYCODE_F5;
	case KEY_F6: return Common::KEYCODE_F6;
	case KEY_F7: return Common::KEYCODE_F7;
	case KEY_F8: return Common::KEYCODE_F8;
	case KEY_F9: return Common::KEYCODE_F9;
	case KEY_F10: return Common::KEYCODE_F10;
	case KEY_F11: return Common::KEYCODE_F11;
	case KEY_F12: return Common::KEYCODE_F12;
	case KEY_CAPSLOCK: return Common::KEYCODE_CAPSLOCK;
	case KEY_RSHIFT: return Common::KEYCODE_RSHIFT;
	case KEY_LSHIFT: return Common::KEYCODE_LSHIFT;
	case KEY_RCONTROL: return Common::KEYCODE_RCTRL;
	case KEY_LCONTROL: return Common::KEYCODE_LCTRL;
	case KEY_ALT: return Common::KEYCODE_RALT;
	case KEY_MENU: return Common::KEYCODE_MENU;
	case KEY_SCRLOCK: return Common::KEYCODE_SCROLLOCK;
	case KEY_NUMLOCK: return Common::KEYCODE_NUMLOCK;
	case KEY_LWIN: return Common::KEYCODE_LSUPER;
	case KEY_RWIN: return Common::KEYCODE_RSUPER;
	case KEY_PRTSCR: return Common::KEYCODE_PRINT;
	case KEY_0_PAD: return Common::KEYCODE_KP0;
	case KEY_1_PAD: return Common::KEYCODE_KP1;
	case KEY_2_PAD: return Common::KEYCODE_KP2;
	case KEY_3_PAD: return Common::KEYCODE_KP3;
	case KEY_4_PAD: return Common::KEYCODE_KP4;
	case KEY_5_PAD: return Common::KEYCODE_KP5;
	case KEY_6_PAD: return Common::KEYCODE_KP6;
	case KEY_7_PAD: return Common::KEYCODE_KP7;
	case KEY_8_PAD: return Common::KEYCODE_KP8;
	case KEY_9_PAD: return Common::KEYCODE_KP9;
	case KEY_TILDE: return Common::KEYCODE_TILDE;
	default: return Common::KEYCODE_INVALID;
	}
}
}

class OSystem_DOS : public ModularMixerBackend, public ModularGraphicsBackend, Common::EventSource {
public:
	OSystem_DOS(bool silenceLogs);
	virtual ~OSystem_DOS();

	virtual void initBackend();

	virtual bool pollEvent(Common::Event &event);

	virtual Common::MutexInternal *createMutex();
	virtual uint32 getMillis(bool skipRecord = false);
	virtual void delayMillis(uint msecs);
	virtual void getTimeAndDate(TimeDate &td, bool skipRecord = false) const;

	virtual void quit();

	virtual void logMessage(LogMessageType::Type type, const char *message);

	virtual void addSysArchivesToSearchSet(Common::SearchSet &s, int priority);

private:
	uint64_t _startUclock; // TODO: Use TSC if available
	bool _silenceLogs;
};

OSystem_DOS::OSystem_DOS(bool silenceLogs) :
	_silenceLogs(silenceLogs) {
	// Not really, but for now.
	_fsFactory = new POSIXFilesystemFactory();
}

OSystem_DOS::~OSystem_DOS() {
}

void OSystem_DOS::initBackend() {
	_startUclock = uclock();

	if (allegro_init() != 0) {
		debug("Allegro init failed");
		printf("Allegro init failed\n");
		exit(1);
	}

	if (install_mouse() < 0) {
		debug("Mouse init failed");
		printf("Mouse init failed\n");
		exit(1);
	}

	if (install_keyboard() < 0) {
		debug("Keyboard init failed");
		printf("Keyboard init failed\n");
		exit(1);
	}

	if (install_timer() < 0) {
		debug("Timer init failed");
		printf("Timer init failed\n");
		exit(1);
	}

	_timerManager = new DefaultTimerManager();
	_eventManager = new DefaultEventManager(this);
	_savefileManager = new DefaultSaveFileManager();
	_graphicsManager = new DosGraphicsManager();
	_mixerManager = new NullMixerManager();
	// Setup and start mixer
	_mixerManager->init();

	BaseBackend::initBackend();
}

bool OSystem_DOS::pollEvent(Common::Event &event) {
	static int prev_mouse_b;
	static int prev_mouse_x;
	static int prev_mouse_y;
	static char prev_key[KEY_MAX + 1];
	static char cur_key[KEY_MAX + 1];
	((DefaultTimerManager *)getTimerManager())->checkTimers();
	((NullMixerManager *)_mixerManager)->update(1);
	((DosGraphicsManager *)_graphicsManager)->moveCursor(mouse_x, mouse_y);

	int cur_mouse_b = mouse_b;
	int cur_mouse_x = mouse_x;
	int cur_mouse_y = mouse_y;

	for (int i = 0; i < 3; i++)
		if (prev_mouse_b & ~cur_mouse_b & (1 << i)) {
			Common::EventType types[] = {Common::EventType::EVENT_LBUTTONUP, Common::EventType::EVENT_RBUTTONUP, Common::EventType::EVENT_MBUTTONUP};
			
			event.type = types[i];
			event.mouse.x = cur_mouse_x;
			event.mouse.y = cur_mouse_y;
			prev_mouse_b &= ~(1 << i);
			prev_mouse_x = cur_mouse_x;
			prev_mouse_y = cur_mouse_y;
			return true;
		}

	for (int i = 0; i < 3; i++)
		if (~prev_mouse_b & cur_mouse_b & (1 << i)) {
			Common::EventType types[] = {Common::EventType::EVENT_LBUTTONDOWN, Common::EventType::EVENT_RBUTTONDOWN, Common::EventType::EVENT_MBUTTONDOWN};
			event.type = types[i];
			event.mouse.x = cur_mouse_x;
			event.mouse.y = cur_mouse_y;
			prev_mouse_b |= (1 << i);
			prev_mouse_x = cur_mouse_x;
			prev_mouse_y = cur_mouse_y;
			return true;
		}

	if (prev_mouse_x != cur_mouse_x || prev_mouse_y != cur_mouse_y) {
		event.type = Common::EventType::EVENT_MOUSEMOVE;
		event.mouse.x = cur_mouse_x;
		event.mouse.y = cur_mouse_y;
		prev_mouse_x = cur_mouse_x;
		prev_mouse_y = cur_mouse_y;
		return true;
	}

	memcpy(cur_key, const_cast<const char*>(key), sizeof(cur_key));

	for (uint i = 0; i <= KEY_MAX; i++)
		if (prev_key[i] != cur_key[i]) {
			prev_key[i] = cur_key[i];
			debug("Press %d->%d", i, cur_key[i]);
			Common::KeyCode kc = AllegroToOSystemKeycode(i);
			if (i == Common::KEYCODE_INVALID)
				continue;
			event.type = cur_key[i] ? Common::EVENT_KEYDOWN : Common::EVENT_KEYUP;
			event.kbd.keycode = kc;
			event.kbd.flags = transform_kbd_flags(key_shifts);
			event.kbd.ascii = compute_ascii(event.kbd.keycode, event.kbd.flags);
			return true;
		}

	return false;
}

Common::MutexInternal *OSystem_DOS::createMutex() {
	return new NullMutexInternal();
}

uint32 OSystem_DOS::getMillis(bool skipRecord) {
	return (uclock() - _startUclock) / (UCLOCKS_PER_SEC / 1000);
}

void OSystem_DOS::delayMillis(uint msecs) {
	delay(msecs);
}

void OSystem_DOS::getTimeAndDate(TimeDate &td, bool skipRecord) const {
	time_t curTime = time(0);
	struct tm t = *localtime(&curTime);
	td.tm_sec = t.tm_sec;
	td.tm_min = t.tm_min;
	td.tm_hour = t.tm_hour;
	td.tm_mday = t.tm_mday;
	td.tm_mon = t.tm_mon;
	td.tm_year = t.tm_year;
	td.tm_wday = t.tm_wday;
}

void OSystem_DOS::quit() {
	exit(0);
}

void OSystem_DOS::logMessage(LogMessageType::Type type, const char *message) {
	if (_silenceLogs)
		return;

	static FILE *output = 0;
	if (!output)
	  output = fopen ("scummvm.log", "a");
	
	if (!output)
	  return;
	fputs(message, output);
	fflush(output);
}

void OSystem_DOS::addSysArchivesToSearchSet(Common::SearchSet &s, int priority) {
	s.add("test/engine-data", new Common::FSDirectory("test/engine-data", 4), priority);
	s.add("gui/themes", new Common::FSDirectory("gui/themes", 4), priority);
}

OSystem *OSystem_DOS_create(bool silenceLogs) {
	return new OSystem_DOS(silenceLogs);
}

int main(int argc, char *argv[]) {
	g_system = OSystem_DOS_create(false);
	assert(g_system);

	// Invoke the actual ScummVM main entry point:
	int res = scummvm_main(argc, argv);
	g_system->destroy();
	return res;
}
