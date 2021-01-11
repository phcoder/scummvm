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
	// 20006 is the end of statue animation that we handle as functor instead
	kAnimPigeonsEnd = 20009,
	kSpecialPigeonTick = 20014,
	kSpecialPigeonUpDownEnd = 20015
};

class PriamHandler : public Handler {
public:
	PriamHandler() {
		_philExitWarning = 0;
		_bigGuardCounter = false;
		_specialPigeonCounter = 0;
	}

	void handleClick(const Common::String &name) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();

		if (name == "Ares") {
			static const TranscribedSound videos[] = {
				{"T4240nA0", _s("I'm Ares, god of war. I love to fight. Scavengers of the battlefield like vultures and dogs are my favourite animals") },
				{"T4240nB0", _s("Athena is my half-sister but we don't quite see eye-to-eye when it comes to fighting battles. Take trojan war for example. I'm for Trojans, she's or the Greeks") },
				{"T4240nC0", _s("I'm not very popular with gods and goddesses on mount Olympus. But Aphrodite thinks I'm brave") },
				{ nullptr, nullptr }
			};

			room->playStatueSMK(kAresStatue,
					    "AnimAresGlow",
					    500,
					    videos, 25, 42);
			return;
		}

		if (name == "Aphrodite") {
			static const TranscribedSound videos[] = {
				// unclear
				{"T4250nA0", _s("I'm Aphrodite. The goddess of love and beauty. I was born from the foam of the see")},
				{"T4250nB0", _s("Oh that silly fighting in Troy started because I won the judgement of Paris. You see Paris gave me the golden apple, so I have to win Helen of Troy") },
				{ nullptr, nullptr }
			};

			room->playStatueSMK(kAphroditeStatue,
					    "AnimAphroditeGlow",
					    600,
					    videos, 25, 42);
			return;
		}

		if (name == "DoorGuard") {
			room->disableMouse();
			_ambients.hide("AmbSmallGuard");
			room->playVideoSpeech(TranscribedSound::make(
						      "MovDoorGuardNoPass",
						      "Only royal messengers on official business can leave the city. King Priam's orders"
						      ), 700, 20018, Common::Point(508, 414));
			return;
		}

		if (name == "TowerGuard") {
			if (persistent->_troyMessageIsDelivered) {
				_ambients.play("AmbBigGuard", true);
				return;
			}

			room->disableMouse();
			if (_bigGuardCounter) {
				// unclear
				playPhilVideo(TranscribedSound::make(
						      "PhilTowerGuard",
						      "You can't get up with that guard calling the place. That'll never happen. You've got to find another way to get that note to Helen"));
			} else {
				_ambients.hide("AmbBigGuard");
				room->playVideoSpeech(TranscribedSound::make(
							      "MovTowerGuard",
							      "Noone sees Helen of Troy. Forget about it"),
						      200, 20017, Common::Point(0, 58));
			}
			_bigGuardCounter = !_bigGuardCounter;
			return;
		}

		if (name == "Helen") {
			playPhilVideo(TranscribedSound::make(
					      "PhilSheCantHearYou",
					      "Sorry, kid. She can't hear you from there"));
			return;
		}
	}

	bool handleClickWithItem(const Common::String &name, InventoryItem item) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		if (name == "DoorGuard" && item == kDecree) {
			room->disableMouse();
			if (!persistent->_troyMessageIsDelivered && _philExitWarning < (persistent->_hintsAreEnabled ? 2 : 1)) {
				playPhilVideo(_philExitWarning
					      ? TranscribedSound::make(
						      "PhilHint",
						      "Hey, I've got an idea: why don't you use the pigeons to fly the message up to Helen?")
					      : TranscribedSound::make(
						      "PhilNoDuckingOut",
						      "You're not ducking out without delivering the message to Helen, are you?"));
				_philExitWarning++;
				return true;
			}
			_ambients.hide("AmbSmallGuard");
			room->playVideoSpeech(TranscribedSound::make("MovDoorGuardPass", "Oh, so you're a new messenger, hm. Sorry, I didn't recognize you. Go ahead and watch out for those Greeks. They're everywhere."), 700, 20019, Common::Point(508, 414));
			return true;
		}

		if (name == "Helen" && item == kMessage) {
			playPhilVideo(
				persistent->_gender == kMale
				? TranscribedSound::make("PhilEvenAHero", "Even a hero can't jump that high. Sorry, you'll have to find another way")
				: TranscribedSound::make("PhilEvenAHeroine", "Even a heroine can't jump that high. Sorry, you'll have to find another way"));
			return true;
		}

		if (name == "SpecialPigeon" && item == kMessage) {
			g_vm->getHeroBelt()->removeFromInventory(kMessage);
			persistent->_troyMessageIsDelivered = true;
			room->disableMouse();
			room->disableHotzone("Helen");
			room->disableHotzone("SpecialPigeon");
			room->playVideoSFX("MovSpecialPigeonNote", 500, 20016);
			_ambients.hide("AmbSpecialPigeon");
			_ambients.hide("AmbHelen");
			_specialPigeonIsBusy = true;
			return true;
		}

		return false;
	}

	void handleEvent(int eventId) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		switch (eventId) {
		case 20001:
			_ambients.tick();
			break;
		case 20003:
			room->enableMouse();
			break;
		case kAnimPigeonsEnd:
			if (!persistent->_troyMessageIsDelivered)
				room->playAnim("AnimPigeons", 600, PlayAnimParams::disappear(), kAnimPigeonsEnd);
			else
				room->playAnim("AnimPigeonsFlyAway", 600, PlayAnimParams::disappear(), 20010);
			break;
		case kSpecialPigeonTick: {
			if (_specialPigeonIsBusy || persistent->_troyMessageIsDelivered)
				break;
			if (_specialPigeonIsDown) {
				_specialPigeonCounter++;
				if (_specialPigeonCounter > 2) {
					room->playAnimWithSFX(
						"AnimSpecialPigeonUp",
						"SndSpecialPigeonUp",
						500,
						PlayAnimParams::disappear(),
						kSpecialPigeonUpDownEnd);
					_ambients.hide("AmbSpecialPigeon");
					room->disableHotzone("SpecialPigeon");
					_specialPigeonIsBusy = true;
					_specialPigeonCounter = 0;
					return;
				}
			} else {
				room->playAnimWithSFX(
						"AnimSpecialPigeonDown",
						"SndSpecialPigeonDown",
						500,
						PlayAnimParams::disappear(),
						kSpecialPigeonUpDownEnd);
				_specialPigeonIsBusy = true;
				return;
			}
			break;
		}
		case kSpecialPigeonUpDownEnd:
			_specialPigeonIsDown = !_specialPigeonIsDown;
			_specialPigeonIsBusy = false;
			if (_specialPigeonIsDown) {
				_ambients.unpauseAndFirstFrame("AmbSpecialPigeon");
				room->enableHotzone("SpecialPigeon");
			}
			break;
		case 20016:
			room->playMusic("HelenMusic", 20022);
			room->playAnimKeepLastFrame("AnimCrackedWallOverlay",
						    999, 20011,
						    Common::Point(-10, -10));
			g_vm->addTimer(20021, 2000, 1);
			break;
		case 20017:
			_ambients.unpauseAndFirstFrame("AmbBigGuard");
			room->enableMouse();
			break;
		case 20018:
			room->enableMouse();
			_ambients.unpauseAndFirstFrame("AmbSmallGuard");
			break;
		case 20019:
			_ambients.unpauseAndFirstFrame("AmbSmallGuard");
			room->playAnimKeepLastFrame("AnimGuardDoorOpen", 701, 20020);
			break;
		case 20020:
			g_vm->moveToRoom(kTroyRoom);
			break;
		case 20021:
			room->playAnimLoop("AnimHelenScarf", 600);
			break;
		case 20022:
			playPhilVideo(TranscribedSound::make("PhilYouDidIt", "Hooray, you did it. You delivered the message. Now get out of here as quick as you can. Move it"), 20023);
			break;
		case 20023:
			// TODO: repeated 20024 timer 1/frame
			_collapseCounter = 2;
			room->playAnim("AnimCollapseL", 111,
				       PlayAnimParams::disappear(), 20026);
			room->playAnim("AnimCollapseR", 111,
				       PlayAnimParams::disappear(), 20027);
			room->playSFX("BigCollapseSnd");
			break;
		case 20026:
		case 20027:
			// TODO: timer 20024
			_collapseCounter--;
			if (_collapseCounter == 0) {
				playPhilVideo(TranscribedSound::make(
						      "PhilOhCollapse",
						      "Oh great, that's just great. "
						      "The catacombs caved in so there is no going back the way you came. Any ideas?"));
			}
			break;
		}
	}

	void prepareRoom() override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		room->loadHotZones("priam.HOT");
		room->addStaticLayer("Background", 10000, Common::Point(-10, -10));
		room->playMusic("IntroMusic");
		room->playMusicLoop("T4010eA0");
		TextTable prAmb = TextTable(
			Common::SharedPtr<Common::SeekableReadStream>(room->openFile("PrAmb.txt")), 6);
		_ambients.readTableFilePriamSFX(prAmb);
		g_vm->addTimer(20001, 100, -1);
		g_vm->addTimer(20014, 3000, -1);
		_ambients.firstFrame();
		room->selectFrame("AnimGuardDoorOpen", 701, 0);
		g_vm->getHeroBelt()->setColour(HeroBelt::kCool);
		_specialPigeonIsDown = true;
		_specialPigeonIsBusy = false;
		room->playAnim("AnimPigeons", 600, PlayAnimParams::disappear(), kAnimPigeonsEnd);
	}

	bool handleCheat(const Common::String &cheat) override {
		// TODO: flybox
		return false;
	}

private:
	void playPhilVideo(TranscribedSound name, int callback = 20003) {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		room->playVideoSpeech(name, 0, callback, Common::Point(102, 216));
	}

	AmbientAnimWeightedSet _ambients;
	int _philExitWarning;
	int _collapseCounter;
	bool _bigGuardCounter;
	bool _specialPigeonIsBusy;
	bool _specialPigeonIsDown;
	int _specialPigeonCounter;
};

Common::SharedPtr<Hadesch::Handler> makePriamHandler() {
	return Common::SharedPtr<Hadesch::Handler>(new PriamHandler());
}

}
