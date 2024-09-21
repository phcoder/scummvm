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
	((DefaultTimerManager *)getTimerManager())->checkTimers();
	((NullMixerManager *)_mixerManager)->update(1);

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
