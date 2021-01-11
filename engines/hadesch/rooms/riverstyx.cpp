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
 * Copyright 2020 Google
 *
 */
#include "hadesch/hadesch.h"
#include "hadesch/video.h"
#include "hadesch/ambient.h"

namespace Hadesch {

enum {
	kBackgroundZ = 10000
};

enum {
	kDeadManEndAnim = 28014
};

static const TranscribedSound kAlchemistIntro = {
	"alchemist intro",
	_s("In Crete they call me the great alchemist. You see, that was my name, Alchemist. "
	   "I came up with the famous death potion. You splash it on someone and they think youre dead")
};

struct StyxShadeInternal {
	StyxShadeInternal(Common::String name) {
		_name = name;
		_counter = 0;
	}

	void resume() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		_ambient.unpause();
		room->enableMouse();
	}
	
	Common::String _name;
	int _counter;
	AmbientAnim _ambient;
	Common::Array<TranscribedSound> _sounds;
};

class StyxShadeEndSound : public EventHandler {
public:
	StyxShadeEndSound(Common::SharedPtr<StyxShadeInternal> internal) {
		_internal = internal;
	}
	void operator()() {
		_internal->resume();
	}
private:
	Common::SharedPtr<StyxShadeInternal> _internal;
};

// TODO: transparency and shimmering
class StyxShade {
public:
	StyxShade(const Common::String &name, int zVal, int minInt, int maxInt,
		  const Common::String &ambient) {
		_internal = makeInternal(name, zVal, minInt, maxInt, ambient);
	}

	StyxShade(const Common::String &name, int zVal, int minInt, int maxInt) {
		_internal = makeInternal(name, zVal, minInt, maxInt, name + " ambient");
	}

	StyxShade() {
	}

	static Common::SharedPtr<StyxShadeInternal> makeInternal(const Common::String &name, int zVal, int minInt, int maxInt,
							     const Common::String &ambient) {
		Common::SharedPtr<StyxShadeInternal> ret(new StyxShadeInternal(name));
		ret->_ambient = AmbientAnim(ambient, ambient + " sound", zVal, minInt, maxInt,
					    AmbientAnim::KEEP_LOOP, Common::Point(0, 0), AmbientAnim::PAN_ANY);
		return ret;
	}

	void start() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		_internal->_ambient.start();
		room->enableHotzone(_internal->_name);
	}

	void click() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		if (_internal->_sounds.empty())
			return;
		_internal->_ambient.pause();
		room->playVideoSpeech(_internal->_sounds[_internal->_counter % _internal->_sounds.size()],
				800, EventHandlerWrapper(Common::SharedPtr<EventHandler>(new StyxShadeEndSound(_internal))));
		_internal->_counter++;
		room->disableMouse();
	}

	void addSound(TranscribedSound snd) {
		_internal->_sounds.push_back(snd);
	}
private:
	Common::SharedPtr<StyxShadeInternal> _internal;
};

class RiverStyxHandler : public Handler {
public:
	RiverStyxHandler() {
		_charonSound = false;
		_cameraMovingDown = false;
		_cameraMovingUp = false;
	}

	void handleClick(const Common::String &name) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		if (name == "volcano top") {
			room->disableMouse();
			room->playAnimWithSFX("morphing gems", "morphing gems sound",
					      1000, PlayAnimParams::keepLastFrame().backwards(), 28018);
			return;
		}

		if (name == "charon") {
			// Originally it goes through event 28002, 1
			if (persistent->_styxCharonUsedPotion) {
				room->playVideoSpeech(TranscribedSound::make(
							      "charon assumes you have gold sound",
							      "Charon assumes you have a gold death coin to pay for your journey"
							      ), 0, 28004);
			} else {
				
				room->playVideoSpeech(_charonSound
						? TranscribedSound::make(
							"charon says away 2 sound", "Charon said Charon only ferries dead people")
						: TranscribedSound::make(
							"charon says away 1 sound",
							"Away! Charon only ferries the dead"), 0, 28004);
				_charonSound = !_charonSound;
			}
			return;
		}

		if (name == "ax head") {
			_axHead.click();
			return;
		}

		if (name == "statue") {
			_statue.click();
			return;
		}

		if (name == "pillar") {
			_pillar.click();
			return;
		}

		if (name == "dog") {
			_dog.click();
			return;
		}

		if (name == "drowned man") {
			_drownedMan.click();
			return;
		}

		if (name == "trojan soldier") {
			_trojanSoldier.click();
			return;
		}

		if (name == "greek soldier") {
			_greekSoldier.click();
			return;
		}

		if (name == "alchemist") {
			_alchemist.click();
			return;
		}
	}

	bool handleClickWithItem(const Common::String &name, InventoryItem item) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		if (name == "charon" && item == kPotion) {
			// Originally event 28002, kPotion
			room->disableMouse();
			g_vm->getHeroBelt()->removeFromInventory(item);
			_charon.hide();
			room->playVideoSFX("charon glow", 549, 28005, Common::Point(516, 93));
			g_vm->addTimer(28006, 2000, 1);
			persistent->_styxCharonUsedPotion = true;
			return true;
		}
		if (name == "charon" && item == kCoin) {
			// Originally event 28002, kCoin
			room->disableMouse();
			g_vm->getHeroBelt()->removeFromInventory(item);
			_charon.hide();
			room->playVideoSFX("change purse", 549, 28010, Common::Point(524, 100));
			g_vm->addTimer(28008, 1000, 1);
			persistent->_styxCharonUsedCoin = true;
			return true;
		}

		return false;
	}

	void handleEvent(int eventId) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		switch(eventId) {
		case 28004:
			stopCharonTalk();
			if (persistent->_styxCharonUsedPotion && persistent->_styxCharonUsedCoin) {
				_charon.hide();
				room->playVideoSpeech(TranscribedSound::make(
							      "charon asks for help",
							      "Charon's got a beastly mess to cleanup before Charon can go across. "
							      "Charon can use your help"
							      ), 549, 28011, Common::Point(452, 96));
			} else {
				room->enableMouse();
			}
			break;
		case 28005:
			playCharonTalk(TranscribedSound::make(
					       "charon says quite dead sound",
					       "Yes, you're quite dead"), 28004);
			break;
		case 28006:
			room->playMusic("charon glow sting", 28007);
			break;
		case 28008:
			room->playMusic("charon accepts coin sting", 28009);
			break;
		case 28009:
			if (persistent->_styxCharonUsedPotion && persistent->_styxCharonUsedCoin) {
				handleEvent(28004);
			} else {
				playCharonTalk(TranscribedSound::make(
						       "charon takes an advance sound",
						       "Charon will take this as an advance. Come back when you're dead"
						       ), 28004);
			}
			break;
		case 28010:
			_charon.unpauseAndFirstFrame();
			break;
		case 28011:
			_charon.unpauseAndFirstFrame();
			g_vm->moveToRoom(kFerrymanPuzzle);
			break;
		case 28017:
			if (persistent->_quest == kRescuePhilQuest && !persistent->_styxAlchemistSaidIntro) {
				persistent->_styxAlchemistSaidIntro = true;
				_alchemist.click();
			} else
				room->enableMouse();
			break;
		case kDeadManEndAnim:
			break;
		case 28018:
			room->selectFrame("overlay fade to volcano top", 1000, 0, Common::Point(0, -50));
			room->stopAnim("overlay fade from volcano top");
			_cameraMovingUp = true;
			_cameraMovingStart = g_vm->getCurrentTime();
			break;
		case 28019:
			g_vm->moveToRoom(kVolcanoRoom);
			break;
		}
	}

	void prepareRoom() override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		Quest quest = persistent->_quest;
		room->loadHotZones("NearRivr.HOT", false);
		room->enableHotzone("volcano top");
		room->addStaticLayer("background", kBackgroundZ, Common::Point(0, -50));
		room->selectFrame("sign text", 900, 0, Common::Point(0, -50));

		AmbientAnim("bats 1", "bats 1 sound", 925, 8000, 12000,
			    AmbientAnim::DISAPPEAR, Common::Point(0, 0), AmbientAnim::PAN_ANY)
			.start();
		AmbientAnim("bats 2", "bats 2 sound", 925, 8000, 12000,
			    AmbientAnim::DISAPPEAR, Common::Point(0, 0), AmbientAnim::PAN_ANY)
			.start();
		AmbientAnim("terminal bats", "terminal bats sound", 925, 8000, 12000,
			    AmbientAnim::BACK_AND_FORTH, Common::Point(0, 0), AmbientAnim::PAN_ANY)
			.start();
		room->playAnimLoop("mist 1", 950);
		room->playAnimLoop("mist 2", 950);
		room->playAnimLoop("water", 951);
		room->playAnimLoop("group of shades", 950);
		room->selectFrame("morphing gems", 1000, -1);

		if (quest == kRescuePhilQuest) {
			room->selectFrame("ferry", 975, 0, Common::Point(0, -50));
			_charon = AmbientAnim("charon", "charon sound", 550,
				    5000, 10000, AmbientAnim::KEEP_LOOP, Common::Point(0, 0),
				    AmbientAnim::PAN_ANY);
			_charon.start();
			room->enableHotzone("charon");
		}

		room->playMusicLoop(quest == kRescuePhilQuest ? "V4010eB0" : "V4010eA0");
		_axHead = StyxShade("ax head", 800, 5000, 10000);
		_axHead.addSound(TranscribedSound::make(
					 "ax head click sound 1", "My uncle was lying in the bed dying. "
					 "So he confessed to his business partner that he had stolen 15000 drachmas. "
					 "His partner said: \"I know, that's why I poisoned you\""));
		_axHead.addSound(TranscribedSound::make("ax head click sound 2", "I don't know about you but I've got a splitting headache."));
		_axHead.addSound(TranscribedSound::make("ax head click sound 3", "We were having an argument and I suggested we just burry the hatchet."));
		_axHead.start();

		if (quest == kRescuePhilQuest || quest == kCreteQuest) {
			_pillar = StyxShade("pillar", 550, 8000, 12000);
			if (quest == kRescuePhilQuest)
				_pillar.addSound(TranscribedSound::make(
							 "pillar quest speech", "You know it was some goat-man who came trough earlier "
							 "kicking and screaming and saying he had a friend who'd come save him. "
							 "Yeah, alright"));
			_pillar.addSound(TranscribedSound::make(
						 "pillar click sound", "There is a soldier down here who was black belt in karate. "
						 "He killed himself saluting. Ouch"));
			_pillar.start();
		}

		if (quest == kCreteQuest || quest == kTroyQuest || quest == kMedusaQuest) {
			_dog = StyxShade("dog", 600, 5000, 10000);
			if (quest == kCreteQuest)
				_dog.addSound(TranscribedSound::make("dog quest speech", "Woof! It was a chicken bone, alright. "
							       "I choked on a lousy chicken bone. It was a ferocious beast. "
							       "The biggest chicken I ever saw"));
			_dog.addSound(TranscribedSound::make("dog click sound 1", "Ergh. The underworld is not so bad. Everybody's dying to get in"));
			_dog.addSound(TranscribedSound::make("dog click sound 2", "Woof! Woof! Woof! Woof! Woof! Woof! Woof!"));
			_dog.start();
		}

		if (quest == kCreteQuest || quest == kTroyQuest) {
			_greekSoldier = StyxShade("greek soldier", 550, 5000, 10000);
			if (quest == kTroyQuest)
				_greekSoldier.addSound(TranscribedSound::make(
							       "greek soldier quest speech",
							       "I had a tought job in the Greek army. They made me an arrow-catcher"));
			_greekSoldier.addSound(TranscribedSound::make(
						       "greek soldier click sound",
						       "I don't mind being dead. It's like being in a math class"
						       ));
			_greekSoldier.start();
		}

		if (quest == kTroyQuest) {
			_trojanSoldier = StyxShade("trojan soldier", 650, 5000, 10000);
			// unclear
			_trojanSoldier.addSound(TranscribedSound::make(
							"trojan soldier quest speech",
							"Yeah when I was on the top of the cat. When the flaming arrow landed next to us. "
							"Without blinking I pointed at it and screamed \"Fire!\""
							));
			_trojanSoldier.start();
		}

		if (quest == kMedusaQuest) {
			_statue = StyxShade("statue", 700, 5000, 10000);
			_statue.addSound(TranscribedSound::make(
						 "statue quest speech",
						 "You know what I hated most about being turned to stone by Medusa? "
						 "Blasted pigeons"));
			_statue.start();

			_drownedMan = StyxShade("drowned man", 550, 5000, 10000);
			_drownedMan.addSound(TranscribedSound::make(
						     "drowned man click sound 1",
						     "You want to know how I drowned? My swimming instructor told me to "
						     "take a deep breath and then jump in the pool. I got it mixed up. "
						     "I jumped into the pool and then I took a deep breath"
						     ));
			_drownedMan.addSound(TranscribedSound::make(
						     "drowned man click sound 2",
						     "One fellow here died when he rolled out of bed. "
						     "He lived in a treehouse"
						     ));
			_drownedMan.start();
		}

		if (quest == kRescuePhilQuest) {
			_alchemist = StyxShade("alchemist", 750, 5000, 10000, "alchemist");
                        if (!persistent->_styxAlchemistSaidIntro)
				_alchemist.addSound(kAlchemistIntro);
			if (persistent->_hintsAreEnabled) {
				if ((persistent->isInInventory(kCoin) || persistent->_styxCharonUsedCoin)
				    && (persistent->isInInventory(kPotion) || persistent->_styxCharonUsedPotion)) {
					_alchemist.addSound(TranscribedSound::make(
								    "alchemist hint 2", "Charon is not going to take you across that river "
								    "unless he has a goldcoin from you and he thinks yo're dead"));
					// unclear
					_alchemist.addSound(TranscribedSound::make("alchemist hint 3", "Gove the death potion and coin to Charon"));
				} else if (persistent->_creteVisitedAfterAlchemistIntro) {
					_alchemist.addSound(TranscribedSound::make(
								    "alchemist hint 1",
								    "The box, look inside the box to find the death potion and the gold coin. "
								    "If you can figure out how to open it"));
				}
			}
                        if (persistent->_styxAlchemistSaidIntro)
				_alchemist.addSound(kAlchemistIntro);
			_alchemist.addSound(TranscribedSound::make("alchemist click",
							     "Splash a lit bit of death potion on somebody, bang-bang, "
							     "they're going to think you're dead"));
			_alchemist.start();
			if (!persistent->_styxAlchemistSaidIntro)
				room->disableMouse();
		}

		// TODO: condition it on not restoring from save
		room->setViewportOffset(Common::Point(0, -50));
		_cameraMovingDown = true;
		_cameraMovingStart = g_vm->getCurrentTime();
		room->selectFrame("overlay fade from volcano top", 1000, 0, Common::Point(0, -50));

		g_vm->getHeroBelt()->setColour(HeroBelt::kCold);
	}

	void frameCallback() override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();

		if (_cameraMovingDown) {
			int pos = -50 + 50 * (g_vm->getCurrentTime() - _cameraMovingStart) / 4000;
			if (pos >= 0) {
				handleEvent(28017);
				pos = 0;
			}
			room->setViewportOffset(Common::Point(0, pos));
		}

		if (_cameraMovingUp) {
			int pos = -50 * (g_vm->getCurrentTime() - _cameraMovingStart) / 4000;
			if (pos < -50) {
				handleEvent(28019);
				pos = -50;
			}
			room->setViewportOffset(Common::Point(0, pos));
		}
	}

private:
	void playCharonTalk(TranscribedSound name, int event) {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		room->playVideoSpeech(name, 0, event);
		_charon.hide();
		room->playAnimLoop("charon talks", 550);
	}

	void stopCharonTalk() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		_charon.unpauseAndFirstFrame();
		room->stopAnim("charon talks");
	}
	AmbientAnim _charon;
	bool _charonSound;
	bool _cameraMovingDown;
	bool _cameraMovingUp;
	int _cameraMovingStart;

	StyxShade _axHead;
	StyxShade _pillar;
	StyxShade _dog;
	StyxShade _drownedMan;
	StyxShade _statue;
	StyxShade _greekSoldier;
	StyxShade _trojanSoldier;
	StyxShade _alchemist;
};

Common::SharedPtr<Hadesch::Handler> makeRiverStyxHandler() {
	return Common::SharedPtr<Hadesch::Handler>(new RiverStyxHandler());
}

}
