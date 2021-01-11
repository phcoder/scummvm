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

static const char *kBackground = "background";
static const char *kHotspots = "HallTrph.hot";
static const char *kArgo = "argo";

static const TranscribedSound kZeusYooHoo = {
	"zeus yoo-hoo",
	_s("Phil! Oh Phily-titty! Yoo-hoo! Hey!")
};
static const TranscribedSound kZeusYellsPhil = {
	"zeus yells phil",
	_s("Phil!")
};
static const TranscribedSound kZeusVacationIsOver = {
	"zeus vacation's over",
	_s("Ha-ha. Sorry, vacation's over. I need a new hero. "
	   "Have you any new candidates?")
};
static const TranscribedSound kZeusFineChoiceHero = {
	// unclear
	"zeus fine choice hero",
	_s("Uh, fine choice. Now put away the loud voice and get this hero ready for action")
};
static const char *kZeusShaftOfLightLeft = "zeus shaft of light left";

static const char *kRope = "rope";
static const char *kHerculesLintel = "hercules lintel";

static const char *kApplicationHotspots = "applicat.hot";
static const char *kApplication = "application";
static const char *kApplicationHeroine = "application heroine";
static const char *kApplicationHeroineHotzone = "heroine";
static const char *kApplicationHero = "application hero";
static const char *kApplicationHeroHotzone = "hero";
static const char *kApplicationEnterButton = "application enter button";
static const char *kApplicationEnterHotzone = "enter";
static const char *kApplicationBurntHole = "application burnt hole";
static const char *kApplicationText = "application text";
static const char *kApplicationHeroineText = "application heroine text";
static const char *kApplicationHeroText = "application hero text";
static const char *kApplicationNameText = "application name text";
static const char *kApplicationUnfurls = "application unfurls";
static const char *kApplicationUnfurlsSound = "application unfurls sound";
static const char *kApplicationRollsUpHeroine = "application rolls up heroine";
static const char *kApplicationRollsUpHero = "application rolls up hero";
static const char *kApplicationChooseHeroSound = "application choose hero sound";
static const char *kApplicationChooseHeroineSound = "application choose heroine sound";

static TranscribedSound kHadesBurstsIn = {
	"hades bursts in",
	// unclear
	_s("Hey-hey. Woo-hoo. Not so fast, kid. "
	   "Sure mr blowheart, mr high and mighty, sure, he is desperate enough to enlist just about anyone. "
	   "But the job is a lot tougher than it looks. "
	   "My basement is full of hero wannabees who thought they had the stuff. "
	   "Oh sure, Hercules squeaked through but that was an oversight that won't be repeated")
};

static const TranscribedSound kHadesNoMoreHeroes = {
	"hades no more heroes got it",
	_s("No more heroes, got it?")
};

static TranscribedSound kHadesNoMoreHeroines = {
	"hades no more heroines got it",
	_s("No more heroines, got it?")
};

#define kPhilWalksCenterToLeft "phil walks center to left"
#define kPhilSighsLeft "phil sighs left"

#define kPhilSnores "phil snores"
static const char *kPhilSnoresSound = "phil snores sound";
#define kPhilRollsOver "phil rolls over"
#define kPhilJumpsOffPillow "phil jumps off pillow"
static const TranscribedSound kPhilOfCourseIdo = {
	"phil of course i do",
	_s("Of course, I do. I've got someone who looks like hero material right here")
};
static const TranscribedSound kPhilWhatAHothead = {
	"phil what a hothead",
	_s("Phew, what a hothead. Hades never makes it easy for new recruits")
};
static const TranscribedSound kPhilTimeToGetStarted = {
	"phil time to get started",
	_s("Anyway, time to get started")
};
static const TranscribedSound kPhilTreasuresOfHercules = {
	"phil treasures of hercules",
	_s("These are the treasures of Hercules, "
	   "the most decorated hero of all time. "
	   "This is his hall of trophies")
};
static const TranscribedSound kPhilBasicInformation = {
	"phil basic information",
	_s("First, I need you to give me some basic information")
};
static const TranscribedSound kPhilFlakyPlaster = {
	"phil flaky plaster",
	_s("This lonely decrepid stack of bricks is your hall of flaky plaster")
};
static const TranscribedSound kPhilGettinStarted = {
	"phil gettin started",
	_s("But you're just getting started. "
	   "And you know the hero business, kid. It's full of surprises")
};
static const TranscribedSound kPhilToolsOfTrade = {
	"phil tools of the trade",
	_s("Before going off on your first quest I need you to be prepared. "
	   "First off, you're going to need some tools of the trade")
};
static const TranscribedSound kPhilHeroBelt = {
	"phil hero belt",
	_s("You can store the things you pick up along the way in this hero belt")
};
static const TranscribedSound kPhilHeroBeltBuckle = {
	"phil left belt buckle",
	_s("To see what items you're carying just look at the left side of the buckle")
};
static const TranscribedSound kPhilClickAnItem = {
	"phil click an item",
	_s("Click on an item if you want to use it or give it to someone")
};
static const TranscribedSound kPhilHeroPowers = {
	"phil hero powers",
	_s("Your hero powers will be stored on your belt buckle. You don't have any yet but you'll learn some soon or I'll be out of business")
};
static const TranscribedSound kPhilRightNotches = {
	"phil right notches",
	_s("Those notches on the right will let you save your adventure and leave... Or take a look at your quest scroll again")
};
static const TranscribedSound kPhilEyeOfFates = {
	"phil eye of fates",
	_s("Don't get the creeps, kid. This is the other eye of fate. "
	   "It will allow me to keep an eye on you since I have "
	   "to stay here and train new recruits. You'll be hearing "
	   "from me through this eye")
};
static const TranscribedSound kPhilBadNews = {
	"phil bad news",
	_s("Hey, these two mean bad news, bad times and bad breath. "
	   "Their names are pain and panic and they've got an eternal "
	   "contract with Hades to make life miserable for young recruits "
	   "like you. Watch out, they can change shape")
};
static const TranscribedSound kPhilFirstQuest = {
	"phil first quest",
	// unclear
	_s("Ready for your first quest? Alright then. Here is the scoop-troop")
};
#define kPhilTakesScroll "phil takes scroll"
#define kPhilDropsScroll "phil drops scroll"
static const TranscribedSound kPhilDaedalusNeedsHelp = {
	"phil daedalus needs help",
	_s("Some guy named Daedalus needs your help. Oh boy, does he. "
	   "There is an awful beast named Minotaur and it's terrorizing "
	   "the people of Crete. Good luck with this one, kid")
};
static const TranscribedSound kPhilOffToCrete = {
	"phil off to crete",
	_s("You're off to Crete. Make it there, you'll make it anywhere")
};
static const TranscribedSound kPhilArgo = {
	"phil argo",
	_s("Oh another thing: that boat out there, the Argo, it's yours to get around on")
};
#define kPhilTapsFootLeft "phil taps foot left"

#define kPhilYapsWithoutSound "phil yaps without sound"

static const char *kMugshot = "mug shot";
static const char *kMugshotSound = "mug shot sound";
static const char *kPanicAndPainSting = "pain and panic sting";

#define kHeroStatue "hero statue"
#define kHeroineStatue "heroine statue"
static const char *kScroll = "scroll";
static const char *kScrollSound = "scroll sound";
	
static const char *kHeroBelt = "hero belt";
static const char *kHeroBeltUpSound = "hero belt up sound";
static const char *kHeroBeltDownSound = "hero belt down sound";

static struct {
	const char *name;
	TranscribedSound zeusComment;
} herculesRoomElements[] = {
	// TODO: fill these
	{"mares", {"zeus mares", _s("")}},
	{"cattle", {"zeus cattle", _s("")}},
	{"girdle", {"zeus girdle", _s("")}},
	{"horns", {"zeus horns", _s("")}},
	{"hydra", {"zeus hydra", _s("")}},
	{"bull", {"zeus bull", _s("")}},
	{"lion", {"zeus lion", _s("")}},
	{"apples", {"zeus apples", _s("")}},
	{"stables", {"zeus stables", _s("")}},
	{"boar", {"zeus boar", _s("")}},
	{"birds", {"zeus birds", _s("")}},
	// unclear
	{
		"cerberus",
		{"zeus cerberus",
		 _s("Cerberus, the three-headed dog, who guards the underworld. "
		    "Hercules had to catch the beast without any weapons. But that didn't stop my boy. "
		    "He has just grapped the hound with his bare hand and dragged him to the king.")
		}
	},
	{
		"hercules",
		{
			"zeus hercules",
			_s("Hercules, what a hero. King Euristias demanded he "
			   "prove himself by performing twelve labors. As you can see he cam through with flying colors")
		}
	}
};

static const char *allPhils[] = {
	"phil walks left to center",
	kPhilWalksCenterToLeft,
	"phil faces backward left",
	kPhilSighsLeft,
	kPhilTapsFootLeft,
	"phil dusts left",
	"phil dusts center",
	kPhilSnores,
	kPhilRollsOver,
	kPhilJumpsOffPillow,
	kPhilYapsWithoutSound,
	kPhilTakesScroll,
	kPhilDropsScroll,
	kPhilSighsLeft,
	"phil scans left"
};

static struct {
	const char *powerName;
	TranscribedSound zeusComment;
} powerLevelNames[] = {
	"thunderbolt", {"zeus thunderbolt", _s("Oh the mighty thunderbolt. The power of strength, my personal favourite")},
	"trojan horse", {"zeus trojan horse", _s("A trojan horse and thepower of stealth. That's something to be proud of")},
	"owl", {"zeus owl", _s("You're wise beyond your ears. There is your owl and the power of wisdom to prove it")}
};

static struct {
	const char *hotName;
	TranscribedSound zeusComment;
} zeusFriezeComments[] = {
	{
		"labyrinth frieze",
		{
			"zeus labyrinth frieze",
			_s("You want to trap the Minotaur again? Click on the frieze")
		}
	},
	{
		"trojan horse frieze",
		{
			"zeus trojan horse frieze",
			_s("You want to load trojan horse again? Click the frieze")
		}
	},
	{
		"medusa frieze",
		{
			"zeus medusa frieze",
			_s("You want to slay medusa again? Click on the frieze")
		}
	},
	{
		"ferryman frieze",
		{
			"zeus ferryman frieze",
			_s("You want to help Charon again? Click on that frieze")
		}
	},
	{
		"monster frieze",
		{
			"zeus monster frieze",
			_s("You want to battle Hades' monsters again? Click on that frieze")
		}
	}
};


static struct {
	const char *hotName;
	TranscribedSound zeusComment;
} zeusComments[] = {
	{
		"minotaur",
		{
			"zeus minotaur",
			_s("The Minotaur, a ratchid beast. "
			   "Without your help he'd still be devouring people of Crete")
		}
	},
	{
		"medusa",
		{
			"zeus medusa",
			_s("The Gorgon Medusa. Without your help she'd still be growing her rock "
			   "garden with Perseus as a centerpiece.")
		}
	},
	{
		"trojan armor",
		{
			"zeus trojan armor",
			_s("Ten long year of work with the Trojans and you have brought it to the end. Great Work")
		}
	},
	{
		"cyclops",
		{
			"zeus cyclops",
			_s("You sure got by old one-eye. They'll be writing about that one for centuries")
		}
	},
	{
		kHeroStatue,
		{
			"zeus hero statue",
			_s("That's just the greatest hero of all time")
		}
	},
	{
		kHeroineStatue,
		{
			"zeus heroine statue",
			_s("Fantastic work! Now, that's what being the heroine is all about")
		}
	}
};

enum {
	kPhilJokeEvent = 19003,
	kPhilNewQuestScroll = 1019020,
	kPhilBecomesIdle = 1019025,
	kPhilJokeEventCleanup = 1019029,
	kPhilRereadDropScroll = 1019030
};

enum {
	kBackgroundZ = 10000,
	kQuestOverlayZ = 925,
	kMugshotZ = 900,
	kScrollZ = 900,
	kShaftOfLightLeftZ = 500,
	kShaftOfLightRightZ = 500,
	kTrophyZ = 450,
	kPhilZ = 400,
	kApplicationZ = 300,
	kApplicationButtonZ = 299,

	kSoundOnlyZ = 0,

	// TODO: figure proper values
	kHadesVideoZ = 0,
	kHeroBeltZ = 0,
	kGlowZ = 0
};

static const TranscribedSound kPhilReread[] = {
	{ "phil reread crete quest",
	  _s("Some guy named Daedalus needs your help. Oh boy, does he. "
	     "There is an awful beast named Minotaur and it's terrorizing "
	     "the people of Crete. Good luck with this one, kid")
	},
	{ "phil reread troy quest",
	  _s("Holy Hera. This one has a lot of dust on it. "
	     "No wonder that they call this place the ancient world. "
	     "The war is still raging in Troy all over the beautiful Helen who is being held captive there. "
	     "Odysseus, the Greek general in charge, has a new plan that he needs some help with") },
	{ "phil reread medusa quest",
	  _s("Who writes these things? The penmanship. Oy, it's like reading the hieroglyphics. "
	     "Perseus needs your help in Seriphos. So you're off to help slay the Medusa")}
};

Common::String nameQuest() {
	Persistent *persistent = g_vm->getPersistent();
	switch(persistent->_quest) {
	case kNoQuest:
		return "";
	case kCreteQuest:
		return "crete quest";
	case kTroyQuest:
		return "troy quest";
	case kMedusaQuest:
		return "medusa quest";
	case kRescuePhilQuest:
		return "rescue phil quest";
	case kEndGame:
		return "finished game";
	// To silence warning
	case kNumQuests:
		return "";
	}

	return "";
}

Common::String getShaftOfLightRight() {
	Persistent *persistent = g_vm->getPersistent();
	if (persistent->_quest == kEndGame)
		return "zeus shaft of light right";
	return "zeus shaft of light " + nameQuest();
}

class WallOfFameHandler : public Handler {
public:
	WallOfFameHandler() {
		bool isNewGame = !g_vm->isRoomVisited(kWallOfFameRoom);
		Persistent *persistent = g_vm->getPersistent();
		if (isNewGame) {
			_gender = kUnknown;
			_philIsOnTheRight = false;
		} else {
			_gender = persistent->_gender;
			_philIsOnTheRight = true;
		}
		_isEnterMouseover = false;
		_philWalkPhase = 0;
		_philIsBusy = isNewGame;
		_endGameOutro = false;
		_applicationIsActive = false;
		_philSaidDoingGreat = false;
	}

	void handleClick(const Common::String &hotname) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		Quest quest = persistent->_quest;
		if (hotname == kApplicationHeroineHotzone) {
			room->selectFrame(kApplicationHeroine, kApplicationButtonZ, 1);
			room->selectFrame(kApplicationHero, kApplicationButtonZ, 0);
			room->playSFX(kApplicationChooseHeroineSound);
			_gender = kFemale;
			computeEnter();
			return;
		}
		if (hotname == kApplicationHeroHotzone) {
			room->selectFrame(kApplicationHeroine, kApplicationButtonZ, 0);
			room->selectFrame(kApplicationHero, kApplicationButtonZ, 1);
			room->playSFX(kApplicationChooseHeroSound);
			_gender = kMale;
			computeEnter();
			return;
		}
		if (hotname == kApplicationEnterHotzone) {
			pressEnter();
			return;
		}

		if (hotname == "argo") {
			g_vm->moveToRoom(kArgoRoom);
			return;
		}

		for (unsigned i = 0; i < ARRAYSIZE(herculesRoomElements); i++)
			if (hotname == herculesRoomElements[i].name) {
				room->disableMouse();
				room->playAnimKeepLastFrame(hotname + " glow", kGlowZ);
				room->playSFX("click");
				room->playVideoSpeech(herculesRoomElements[i].zeusComment, kSoundOnlyZ, 1019026);
				return;
			}

		for (unsigned power = 0; power < ARRAYSIZE(powerLevelNames); power++) {
			if (hotname.matchString(Common::String::format("%s#", powerLevelNames[power].powerName))) {
				room->playSFX("click");
				if (strcmp(powerLevelNames[power].powerName, "thunderbolt") == 0)
					room->playSFX("thunder sound");
				zeusCommentRight(powerLevelNames[power].zeusComment);
				return;
			}
		}

		for (unsigned i = 0; i < ARRAYSIZE(zeusComments); i++)
			if (hotname == zeusComments[i].hotName) {
				room->playSFX("click");
				zeusCommentRight(zeusComments[i].zeusComment);
				return;
			}

		if (hotname == "typhon") {
			room->playSFX("click");
			zeusCommentRight(persistent->_gender == kFemale
					 ? TranscribedSound::make("zeus typhon heroine", "You faced the typhoon and lived to tell about it. Now that's what being a heroine is all about")
					 : TranscribedSound::make("zeus typhon hero", "You faced the typhoon and lived to tell about it. Now that's what being a hero is all about"));
			return;
		}

		if (hotname == "phil" && quest >= kCreteQuest && quest <= kMedusaQuest) {
			philBecomesListening();
			playPhilVideo(kPhilReread[quest-kCreteQuest], kPhilRereadDropScroll,
				      Common::Point(22, 301));
			return;
		}

		if (hotname == "hades note") {
			g_vm->cancelTimer(19048);
			handleEvent(19048);
			return;
		}

		if (hotname == "hades note popup background" ||
		    hotname == "hades note popup background2") {
			room->popHotZones();
			room->setPannable(true);
			room->stopAnim("hades note popup");
			room->stopAnim("hades note text");
			room->stopAnim("hades note");
			if (persistent->_hintsAreEnabled) {
				zeusCommentRight(TranscribedSound::make(
							 "zeus counting on ya",
							 "Looks like your friend Phil is going to need your help. It's going to be dangerous but he's counting on you and so am I."));
			}
			room->setHotzoneEnabled("argo", true);
			return;
		}

		if (hotname == "monster frieze") {
			g_vm->moveToRoom(kMonsterPuzzle);
			return;
		}

		if (hotname == "ferryman frieze") {
			g_vm->moveToRoom(kFerrymanPuzzle);
			return;
		}

		/*
		  TODO:
		  MNSH: medusa frieze
		  MNSH: trojan horse frieze
		  MNSH: labyrinth frieze
		*/
	}

	void handleMouseOver(const Common::String &hotname) override {
		Persistent *persistent = g_vm->getPersistent();

		if (hotname == kApplicationEnterHotzone) {
			_isEnterMouseover = true;
			computeEnter();
			return;
		}
		if (persistent->_quest <= kMedusaQuest
		    && (hotname == "labyrinth frieze"
			|| hotname == "trojan horse frieze")) {
			if (_philIsBusy)
				return;
			_philIsBusy = true;
			if (hotname == "labyrinth frieze")
				playPhilVideo(TranscribedSound::make(
						      "phil labyrinth frieze",
						      "You want to trap the Minotaur again? Click on that frieze"), kPhilBecomesIdle, Common::Point(40, 324)); // state 51
			else
				playPhilVideo(TranscribedSound::make(
						      "phil trojan horse frieze",
						      "You want to load trojan horse again? Click that frieze"),
					      kPhilBecomesIdle, Common::Point(14, 320)); // state 52
			return;
		}

		for (uint i = 0; i < ARRAYSIZE(zeusFriezeComments); i++)
			if (hotname == zeusFriezeComments[i].hotName) {
				zeusCommentRight(zeusFriezeComments[i].zeusComment, true);
				return;
			}
	}

	void handleMouseOut(const Common::String &hotname) override {
		if (hotname == kApplicationEnterHotzone) {
			_isEnterMouseover = false;
			computeEnter();
			return;
		}
	}

	void handleEvent(int eventId) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		Quest quest = persistent->_quest;
		switch (eventId) {
		case 19004:
			g_vm->addSkippableTimer(19005, 7000);
			room->playAnim(kZeusShaftOfLightLeft, kShaftOfLightLeftZ,
				       PlayAnimParams::keepLastFrame().speed(500));
			room->playVideoSpeech(kZeusYooHoo, kSoundOnlyZ, 19701); // zeus yoo-hoo
			break;
		case 19005:
			playPhilAnimSFX(kPhilRollsOver, 1019001, Common::Point(-26, 2)); // state 1
			break;
		case 19011:
			room->playVideoSpeech(kZeusVacationIsOver, kSoundOnlyZ, 19012);
			break;
		case 19012:
			playPhilVideo(kPhilOfCourseIdo, 1019002, Common::Point(30, 304)); // state 5
			break;
		case 1019002:
			displayPhilIdle();
			room->playVideoSpeech(kZeusFineChoiceHero, kSoundOnlyZ, 19013);
			break;
		case 19013:
			room->playAnim(kZeusShaftOfLightLeft, kShaftOfLightLeftZ, PlayAnimParams::disappear().backwards().speed(500));
			room->setLayerEnabled(kRope, false);
			playPhilVideo(kPhilBasicInformation, 1019003, Common::Point(-4, 2)); // state 6
			break;
		case 19016:
			room->addStaticLayer(kApplicationBurntHole, kApplicationButtonZ);
			room->playVideoSpeech(persistent->_gender == kFemale ? kHadesNoMoreHeroines : kHadesNoMoreHeroes,
					   kHadesVideoZ, 19018,
					   Common::Point(207, 103));
			break;
		case 19018:
			applicationClose();
			break;
		case 19019:
			playPhilVideo(kPhilWhatAHothead, 1019005, Common::Point(40, 327)); // state 7
			break;
		case 1019001:
			cancelAllPhils();
			playPhilAnimSFX(kPhilJumpsOffPillow, -1, Common::Point(-26, 2)); // state 4
			room->playVideoSpeech(kZeusYellsPhil, kSoundOnlyZ, 19011);
			break;
		case 1019003:
			room->setLayerEnabled(kRope, true);
			displayPhilIdle();
			room->playAnimWithSFX(
				kApplicationUnfurls, kApplicationUnfurlsSound,
				kApplicationZ, PlayAnimParams::disappear().speed(200), 1019004);
			break;
		case 1019004:
			room->enableMouse();
			startApplication();
			break;
		case 1019005:
			playPhilVideo(kPhilTimeToGetStarted, 1019006,
					    Common::Point(36, 319)); // state 8
			break;
		case 1019006:
			playPhilVideo(kPhilTreasuresOfHercules, 1019007,
				      Common::Point(40, 323)); // state 9
			break;
		case 1019007:
			displayPhilIdle();
			room->panRightAnim(1019008);
			break;
		case 1019008:
			_philIsOnTheRight = true;
			updatePhilHotzone();
			playPhilVideo(kPhilFlakyPlaster, 1019009,
				      Common::Point(0, 0)); //state 10
			break;
		case 1019009:
			playPhilVideo(kPhilGettinStarted, 1019010,
					Common::Point(14, 324)); //state 11
			break;
		case 1019010:
			room->disableHeroBelt();
			playPhilVideo(kPhilToolsOfTrade, 1019011,
				      Common::Point(40, 320)); //state 12
			break;
		case 1019011:
			displayPhilYap();
			room->playAnimWithSFX(
				kHeroBelt, kHeroBeltUpSound, kHeroBeltZ,
				PlayAnimParams::keepLastFrame().partial(0, 9),
				19022, kOffsetRightRoom);
			break;
		case 19022:
			room->playVideoSpeech(kPhilHeroBelt, kPhilZ, 1019012); // state 13
			break;
		case 1019012:
			room->playVideoSpeech(kPhilHeroBeltBuckle, kPhilZ, 1019013); // state 14
			break;
		case 1019013:
			room->selectFrame(kHeroBelt, kHeroBeltZ, 10,
					     kOffsetRightRoom);
			room->playSFX("hero belt items highlight sound");
			room->playVideoSpeech(kPhilClickAnItem, kPhilZ, 1019014); // state 15
			break;
		case 1019014:
			room->selectFrame(kHeroBelt, kHeroBeltZ, 11,
					     kOffsetRightRoom);
			room->playSFX("hero belt items highlight sound");
			room->playVideoSpeech(kPhilHeroPowers, kPhilZ, 1019015); // state 16
			break;
		case 1019015:
			room->selectFrame(kHeroBelt, kHeroBeltZ, 12,
					     kOffsetRightRoom);
			room->playSFX("hero belt items highlight sound");
			g_vm->addTimer(19029, 4000);
			room->playVideoSpeech(kPhilRightNotches, kPhilZ, 1019016); // state 17
			break;
		case 1019016:
			g_vm->cancelTimer(19029);
			room->playAnimWithSFX(
				kHeroBelt, kHeroBeltDownSound, kHeroBeltZ, PlayAnimParams::disappear().partial(9, 0),
				1019017, kOffsetRightRoom);
			break;
		case 1019017:
			room->enableHeroBelt();
			playPhilVideo(kPhilEyeOfFates, 1019018,
				      Common::Point(-8, 144)); // state 18
			break;
		case 1019018:
			room->playSFX(kPanicAndPainSting);
			playPhilVideo(kPhilBadNews, 1019019, Common::Point(14, 320)); // state 19
			break;
		case 1019019:
			displayPhilIdle();
			room->playAnimWithSFX(kMugshot, kMugshotSound,
					      kMugshotZ, PlayAnimParams::keepLastFrame(),
					      19006, kOffsetRightRoom);
			break;
		case 19006:
			room->playAnimWithSFX(kScroll, kScrollSound,
					      kScrollZ, PlayAnimParams::keepLastFrame(),
					      19009, kOffsetRightRoom);
			break;
		case 19009:
			playPhilVideo(kPhilFirstQuest, kPhilNewQuestScroll, Common::Point(40, 324)); // state 21
			break;
		case kPhilNewQuestScroll:
			room->selectFrame(kScroll, kScrollZ, 0, kOffsetRightRoom);
			room->playMusic("quest intro music");
			playPhilAnimSFX(kPhilTakesScroll, 1019021); // state 22
			break;
		case 1019021:
			switch (quest) {
			default:
			case kCreteQuest:
				playPhilVideo(kPhilDaedalusNeedsHelp, 1019022, Common::Point(28, 312)); // state 24
				break;
			case kTroyQuest:
				playPhilVideo(TranscribedSound::make("phil holy hera", "Holy Hera. This one has a lot of dust on it. No wonder that they call this place the ancient world. The war is still raging in Troy all over the beautiful Helen who is being held captive there. Odysseus, the Greek general in charge, has a new plan that he needs some help with"), 1019022, Common::Point(28, 312)); // state 29
				break;
			case kMedusaQuest:
				playPhilVideo(TranscribedSound::make("phil who writes these things", "Who writes these things? The penmanship. Oy, it's like reading the hieroglyphics. Perseus needs your help in Seriphos. So you're off to help slay the Medusa"), 1019022, Common::Point(28, 312)); // state 36
				break;
			}
			break;
		case 1019022:
			playPhilAnimSFX(kPhilDropsScroll, 1019023); // state 23
			break;
		case 1019023:
			switch (quest) {
			default:
			case kCreteQuest:
				playPhilVideo(kPhilOffToCrete, 1019024, Common::Point(40, 324)); // state 25
				break;
			case kTroyQuest:
				playPhilVideo(TranscribedSound::make(
						      "phil hourly rates",
						      "You're off to Troy, kid. A very dark and violent place. "
						      "It's so dangerous they've got hourly rates on life insurance"),
					      1019033, Common::Point(28, 312)); // state 30
				break;
			case kMedusaQuest:
				playPhilVideo(TranscribedSound::make(
						      "phil off to seriphos",
						      "You're off to Seriphos. Talk about a boring place. "
						      "I've spent two weeks there in one afternoon"),
					      kPhilBecomesIdle, Common::Point(14, 320)); // state 37
			}
			break;
		case 1019024:
			playPhilVideo(kPhilArgo, kPhilBecomesIdle, Common::Point(40, 324)); // state 26
			break;
		case kPhilBecomesIdle:
			philBecomesIdle();
			break;
		case 1019026:
			room->enableMouse();
			for (unsigned i = 0; i < ARRAYSIZE(herculesRoomElements); i++)
				room->stopAnim(Common::String(herculesRoomElements[i].name) + " glow");
			break;
		case 1019027:
			if (!_philIsBusy)
				schedulePhilJoke();
			break;
		case 1019028:
			g_vm->cancelTimer(kPhilJokeEvent);
			break;
		case kPhilJokeEvent: {
			if (_philIsBusy) {
				schedulePhilJoke();
				break;
			}

			if (!_philSaidDoingGreat && quest != kCreteQuest) {
				_philSaidDoingGreat = true;
				// state 50
				playPhilVideo(TranscribedSound::make(
						      "phil doing great",
						      "You're doing great but you could always use more practice. "
						      "You can play the activity by clicking on the frieze"), kPhilJokeEventCleanup,
					      Common::Point(14, 320));
				break;
			}
				
			int rnd = g_vm->getRnd().getRandomNumberRng(1, 10);
			debug("rnd = %d", rnd);
			switch(rnd) {
			case 1:
				// state 40
			    playPhilVideo(TranscribedSound::make(
						      "phil break time over",
						      "The break time is just about over. You need to get back to your quest, kiddo"
						      ), kPhilJokeEventCleanup,
					      Common::Point(40, 320));
				break;
			case 2:
				// state 41
			    playPhilVideo(TranscribedSound::make("phil back to work",
							       "We're not getting paid by the hour here, kid. "
							       "Let's get back to work"),
					      kPhilJokeEventCleanup,
					Common::Point(14, 320));
				break;
			case 3:
				// state 42
			    playPhilVideo(TranscribedSound::make("phil lets go", "Hey, I'm not getting any younger here. Let's go"),
					      kPhilJokeEventCleanup,
					Common::Point(16, 320));
				break;
			case 4:
				// state 43
				playPhilAnimWithoutSound("phil scans left", kPhilJokeEventCleanup,
							 Common::Point(-26, 2));
				break;
			case 5:
				// state 44, go right
				if (_philWalkPhase == 3) {
					schedulePhilJoke();
					break;
				}
				_philWalkPhase++;
				updatePhilHotzone();
				playPhilAnimSFX("phil walks left to center", kPhilJokeEventCleanup,
						Common::Point(-166, +2));
				break;
			case 6:
				// state 45, go left
				if (_philWalkPhase == 0) {
					schedulePhilJoke();
					break;
				}
				_philWalkPhase--;
				updatePhilHotzone();
				playPhilAnimSFX(kPhilWalksCenterToLeft, kPhilJokeEventCleanup);
				break;
			case 7:
				// state 46
				playPhilAnimSFX("phil faces backward left", kPhilJokeEventCleanup, Common::Point(-640, 0));
				break;
			case 8:
				// state 47
				playPhilAnimSFX(kPhilSighsLeft, kPhilJokeEventCleanup);
				break;
			case 9:
				// state 48
				playPhilAnimSFX(kPhilTapsFootLeft, kPhilJokeEventCleanup);
				break;
			case 10:
				// state 49, dusts
				switch (_philWalkPhase) {
				case 0:
					playPhilAnimSFX("phil dusts left", kPhilJokeEventCleanup);
					break;
				case 1:
				case 2:
					playPhilAnimSFX("phil dusts center", kPhilJokeEventCleanup, Common::Point(-166, 2));
					break;
				case 3:
					schedulePhilJoke();
					break;
				}
				break;
			}
			break;
		}
		case 19029:
			room->selectFrame(kHeroBelt, kHeroBeltZ, 13,
					  kOffsetRightRoom);
			room->playSFX("hero belt items highlight sound");
			break;
		case kPhilJokeEventCleanup:
			schedulePhilJoke();
			break;
		case 19045:
			philBecomesIdle();
			room->playAnim(
				getShaftOfLightRight(),
				kShaftOfLightRightZ,
				PlayAnimParams::disappear().speed(500));
			room->enableMouse();
			break;
		case kPhilRereadDropScroll:
			playPhilAnimSFX(kPhilDropsScroll, 19047);
			break;
		case 19047:
			philBecomesIdle();
			break;
		case 19048:
			room->disableMouse();
			room->disableHotzone("hades note");
			room->enableHotzone("hades note popup");
			room->disableHotzone("hades note popup background");
			room->enableHotzone("hades note popup background2");
			room->selectFrame("hades note popup", 150, 0, kOffsetRightRoom);
			room->selectFrame("hades note text", 149, 0, kOffsetRightRoom);
			room->playVideoSpeech(TranscribedSound::make(
						"hades reads note",
						"Dear victim, forget about Phil. "
						"He's mine now and he's going to stay that way. "
						"We do have room for one more though "
						"if you'd like to join him. "
						"Hey, you know what, if I were you, I'd quit while I was ahead or rather while I had a head. He-hey, what a comic"), 0, 19049);
			break;
		case 19049:
			room->enableMouse();
			break;
		case 19051:
			_hercules = AmbientAnim("herc blinks", "", 0, 2000, 5000,
						AmbientAnim::KEEP_LOOP, kOffsetRightRoom,
						AmbientAnim::PAN_ANY);
			_hercules.start();
			playPhilVideo(TranscribedSound::make(
					      "phil that was awesome",
					      _s("That was awesome, kid."
						 "You were brilliant. You're going to go down in history. "
						 "We make a heck of a good team. I'll see you on our next adventure")
					      ), 19052,
				      Common::Point(14, 320)); // state 56
			break;
		case 19052:
			philBecomesListening();
			room->fadeOut(250, 19053);
			break;
		case 19053:
			_hercules.hide();
			room->resetFade();
			room->disableHeroBelt();
			room->resetLayers();
			room->disableMouse();
			room->setPannable(false);
			room->addStaticLayer("black background", kBackgroundZ, kOffsetRightRoom);
			room->playVideoSFX("hades eye of fates", 0, 19054, kOffsetRightRoom);
			room->playSFX("herc laughs");
			room->playSFX("phil laughs");
			break;
		case 19054:
			g_vm->moveToRoom(kCreditsRoom);
			break;
		case 1019031:
			_philWalkPhase--;
			updatePhilHotzone();
			playPhilAnimSFX(kPhilWalksCenterToLeft, 1019032);
			break;
		case 1019032:
		    playPhilVideo(TranscribedSound::make("phil lots more to do", "Ok, we've got lots more to do"),
				      kPhilNewQuestScroll, Common::Point(14, 312)); // state 28
			break;
		case 1019033:
			playPhilVideo(persistent->_gender == kFemale
				      ? TranscribedSound::make(
					      "phil go to it heroine", "Go to it, heroine")
				      : TranscribedSound::make(
					      "phil go to it hero", "Go to it, hero"),
				      1019034, Common::Point(40, 324)); // state 32/31
			break;
		case 1019034:
		    playPhilVideo(TranscribedSound::make("phil off to troy", "You're off to Troy"),
				      kPhilBecomesIdle, Common::Point(40, 324)); // state 33
			break;
		case 1019035:
		    playPhilVideo(TranscribedSound::make("phil sneaky devil", "He-he-he you, sneaky devil, you earned the power of stealth."
						       "You're making me proud, kid"),
				      1019036, Common::Point(40, 324)); // state 34
			break;
		case 1019036:
			_philWalkPhase--;
			updatePhilHotzone();
			playPhilAnimSFX(kPhilWalksCenterToLeft, 1019037);
			break;
		case 1019037:
		    playPhilVideo(TranscribedSound::make("phil good work", "Good work but we're not through yet"), kPhilNewQuestScroll, Common::Point(40, 324)); // state 35
			break;
		case 19900:
			if (_gender != kUnknown && !_heroName.empty()) {
				break;
			}
			if (!_heroName.empty()) {
			    room->playVideoSpeech(TranscribedSound::make("phil pick a statue", "What are you waiting for? Pick a statue"), 0, 19901);
				break;
			}

			if (g_vm->getRnd().getRandomBit()) {
			    room->playVideoSpeech(TranscribedSound::make("phil not a mind reader", "Hey kid, I'm not a mind-reader"), 0, 19901);
			} else {
			    room->playVideoSpeech(TranscribedSound::make("phil type in your name", "So type in your name"), 0, 19901);
			}
			break;
		case 19901:
			g_vm->addTimer(19900, 12000);
			break;
		}
	}

	void handleKeypress(uint32 code) override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		if (_applicationIsActive && room->isMouseEnabled()) {
			if (_heroName.size() < 18 &&
			    (('a' <= code && code <= 'z')
			     || ('A' <= code && code <= 'Z')
			     || code == '.' || code == '-' || code == ' ')
			    && room->computeStringWidth("application sign-in text", _heroName + code) <= 318) {
				_heroName += code;
				room->playSFX("application click");
				renderNameInApplication();
				computeEnter();
				return;
			}

			if (_heroName.size() > 0 && code == '\b') {
				_heroName.deleteLastChar();
				room->playSFX("application click");
				renderNameInApplication();
				computeEnter();
				return;
			}

			if (_heroName.size() > 0 && (code == '\r' || code == '\n')) {
				pressEnter();
				return;
			}
		}
	}

	void prepareRoom() override {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		bool newGame = !g_vm->isRoomVisited(kWallOfFameRoom);
		Persistent *persistent = g_vm->getPersistent();
		room->loadHotZones(kHotspots, false);
		room->addStaticLayer(kBackground, kBackgroundZ);
		room->addStaticLayer(kHerculesLintel, 900);
		room->addStaticLayer(kRope, 900);
		Quest quest = persistent->_quest; 
		if (quest != kEndGame) {
			room->addStaticLayer(kArgo, 950, kOffsetRightRoom);
			room->enableHotzone("argo");
			room->playAnimLoop("argo ambient", 949, kOffsetRightRoom);
		}
		room->playAnimLoop("water ambient", 975, kOffsetRightRoom);
		room->playAnimLoop("hercules hall ambients", 900);
		room->addStaticLayer(nameQuest() + " overlay", kQuestOverlayZ, kOffsetRightRoom);
		if (quest == kEndGame) {
			const char *heroStatue = persistent->_gender == kFemale ? kHeroineStatue : kHeroStatue;
			room->selectFrame(heroStatue, 450, 0, kOffsetRightRoom);
			room->enableHotzone(heroStatue);
			room->setHotZoneOffset(heroStatue, kOffsetRightRoom);
			room->enableHotzone("monster frieze");
			room->enableHotzone("ferryman frieze");
			room->enableHotzone("cyclops");
			room->enableHotzone("typhon");
		}

		if (quest > kMedusaQuest) {
			room->enableHotzone("medusa frieze");
			room->enableHotzone("medusa");
		}
		if (quest > kTroyQuest) {
			room->enableHotzone("trojan horse frieze");
			room->enableHotzone("trojan armor");
		}
		if (quest > kCreteQuest) {
			room->enableHotzone("labyrinth frieze");
			room->enableHotzone("minotaur");
		}

		if (quest < kRescuePhilQuest) {
			room->enableHotzone("phil");
			room->setHotZoneOffset("phil", kOffsetRightRoom);
		}

		// Hercules room
		for (unsigned i = 0; i < ARRAYSIZE(herculesRoomElements); i++)
			room->enableHotzone(herculesRoomElements[i].name);

		for (unsigned power = 0; power < ARRAYSIZE(powerLevelNames); power++) {
			int level = persistent->_powerLevel[power];
			if (level > 0) {
				int off = (4 - level) * 30;
				Common::String hz = Common::String::format("%s%d", powerLevelNames[power].powerName, level);
				room->playAnim(powerLevelNames[power].powerName, kTrophyZ,
					       PlayAnimParams::loop().partial(off, off + 29),
					       -1, kOffsetRightRoom);
				room->enableHotzone(hz);
				room->setHotZoneOffset(hz, kOffsetRightRoom);
			}
		}
		
		room->selectFrame(kScroll, kScrollZ, 0, kOffsetRightRoom);
		room->setPannable(true);
		if (newGame) {
			room->disableMouse();
			room->playAnimWithSFX(kPhilSnores, kPhilSnoresSound, kPhilZ, PlayAnimParams::loop()); //state 0
			room->selectFrame(kMugshot, kMugshotZ, 0, kOffsetRightRoom);
			g_vm->addSkippableTimer(19004, 4000);
		} else if (persistent->_doQuestIntro) {
			persistent->_doQuestIntro = false;
			room->disableMouse();
			switch (quest) {
			case kTroyQuest:
				_philWalkPhase = 1;
				playPhilVideo(TranscribedSound::make(
						      "phil congrats trapped minotaur",
						      "Way to go kid. I saw it all. When you trapped the Minotaur I got so excited. "
						      "Phew. A couple more like that and we're going to have to throw you a party"),
					      1019031, Common::Point(14, 320)); // state 27
				room->playMusic("done crete quest theme");
				break;
			case kMedusaQuest:
				_philWalkPhase = 1;
				playPhilVideoSFX("phil dances", 1019035, Common::Point(36, 257)); // state 38
				room->playMusic("done troy quest theme");
				break;
			case kRescuePhilQuest:
				g_vm->addTimer(19048, 5000, 1);
				room->pushHotZones("HTNote.HOT", true, kOffsetRightRoom);
				room->disableHotzone("hades note popup background2");
				room->setPannable(false);
				room->enableMouse();
				room->playAnimWithSFX("hades note", "hades note sound", 150, PlayAnimParams::loop(),
						      EventHandlerWrapper(), kOffsetRightRoom);
				room->setHotzoneEnabled("argo", false);
				room->playMusic("done medusa quest theme");
				_endGameOutro = true;
				break;

			case kNumQuests: // To silence warning
			case kEndGame:
			case kNoQuest:
				_philWalkPhase = 1;
				philBecomesListening();
				room->playVideoSpeech(TranscribedSound::make(
						    "herc congratulates",
						    "Sorry I wasn't able to help you out. But look at you, you did it all on your own. You proved that being a true hero isn't about the size of your strength but the strength of your heart. Congrats, kid. You're all heart"),
						0, 19051, kOffsetRightRoom);
				break;
			case kCreteQuest:
				break;
			}

			room->panRightInstant();
		} else {
			if (persistent->_quest < kRescuePhilQuest)
				schedulePhilJoke();
			room->panRightInstant();
		}

		renderLintel();

		room->playMusicLoop("C1010eA0");

		g_vm->getHeroBelt()->setColour(HeroBelt::kWarm);
	}

	~WallOfFameHandler() override {}

private:
	void clearNameInApplication() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		// One character more to handle possible backspace clicks.
		room->hideString("application sign-in text", _heroName.size() + 1);
	}

	void renderNameInApplication() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		clearNameInApplication();
		room->renderString("application sign-in text", _heroName, Common::Point(202, 338), 298);
	}

	void philBecomesIdle() {
		_philIsBusy = false;
		schedulePhilJoke();
	}

	void philBecomesBusy() {
		_philIsBusy = true;
		g_vm->cancelTimer(kPhilJokeEvent);
	}

	void philBecomesListening() {
		philBecomesBusy();
		displayPhilIdle();
	}

	void schedulePhilJoke() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		displayPhilIdle();
		room->enableMouse();
		g_vm->addTimer(kPhilJokeEvent, g_vm->getRnd().getRandomNumberRng(10000, 15000));
		room->setUserPanCallback(1019028, -1, -1, 1019027);
	}

	void pressEnter() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();

		// originally event 19015
		if (_gender != kUnknown /*&& name != ""*/) {
			persistent->_gender = _gender;
			persistent->_heroName = _heroName;
			room->disableMouse();
			// Or should it be music? Unclear to me
			room->playSFX("hades evil intro theme");
			room->playVideoSpeech(kHadesBurstsIn, kHadesVideoZ, 19016);
		}
	}

	void zeusCommentRight(TranscribedSound sound, bool mouseIsEnabled = false) {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		if (!mouseIsEnabled)
			room->disableMouse();
		room->playVideoSpeech(sound, kSoundOnlyZ, 19045);
		room->playAnim(
			getShaftOfLightRight(), kShaftOfLightRightZ, PlayAnimParams::keepLastFrame().speed(500), -1, kOffsetRightRoom);
		philBecomesListening();
	}

	Common::Point getPhilBase() const {
		Common::Point v;
		if (_philIsOnTheRight)
			v = Common::Point(640, 0);
		else
			v = Common::Point(26, -2);
		if (_philWalkPhase >= 0 && _philWalkPhase <= 3)
			v += Common::Point(166 * _philWalkPhase, -2 * _philWalkPhase);
		return v;
	}

	void playPhilVideo(TranscribedSound name, int callback, const Common::Point videoOffset) {
		Persistent *persistent = g_vm->getPersistent();
		cancelAllPhils();
		if (persistent->_quest == kRescuePhilQuest)
			return;
		g_vm->getVideoRoom()->playVideoSpeech(name, kPhilZ, callback,
						      videoOffset + getPhilBase());
	}

	void playPhilVideoSFX(const Common::String &name, int callback, const Common::Point videoOffset) {
		Persistent *persistent = g_vm->getPersistent();
		cancelAllPhils();
		if (persistent->_quest == kRescuePhilQuest)
			return;
		g_vm->getVideoRoom()->playVideoSFX(name, kPhilZ, callback,
						   videoOffset + getPhilBase());
	}

	void playPhilAnimSFX(const Common::String &name,
			     int callback, const Common::Point videoOffset = Common::Point(0, 0)) {
		Persistent *persistent = g_vm->getPersistent();
		cancelAllPhils();
		if (persistent->_quest == kRescuePhilQuest)
			return;
		g_vm->getVideoRoom()->playAnimWithSFX(
			name, name + " sound", kPhilZ, PlayAnimParams::keepLastFrame(), callback,
			videoOffset + getPhilBase());
	}

	void playPhilAnimWithoutSound(const Common::String &name,
				      int callback, const Common::Point videoOffset = Common::Point(0, 0)) {
		Persistent *persistent = g_vm->getPersistent();
		cancelAllPhils();
		if (persistent->_quest == kRescuePhilQuest)
			return;
		g_vm->getVideoRoom()->playAnimKeepLastFrame(
			name, kPhilZ, callback,
			videoOffset + getPhilBase());
	}

	void displayPhilIdle() {
		Persistent *persistent = g_vm->getPersistent();
		cancelAllPhils();
		if (persistent->_quest == kRescuePhilQuest
		    || (!_endGameOutro && persistent->_quest == kEndGame))
			return;
		g_vm->getVideoRoom()->selectFrame(kPhilSighsLeft, kPhilZ, 0,
						  getPhilBase());
	}

	void displayPhilYap() {
		cancelAllPhils();
		g_vm->getVideoRoom()->playAnimLoop(
			kPhilYapsWithoutSound, kPhilZ, Common::Point(640-166, 0));
	}

	void computeEnter() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		if (_gender == kUnknown /*|| name == ""*/) {
			room->selectFrame(kApplicationEnterButton, kApplicationButtonZ, 0);
			room->disableHotzone(kApplicationEnterHotzone);
			return;
		}
		room->selectFrame(kApplicationEnterButton, kApplicationButtonZ, _isEnterMouseover ? 2 : 1);
		room->enableHotzone(kApplicationEnterHotzone);
	}

	void startApplication() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		room->addStaticLayer(kApplication, kApplicationZ);
		room->selectFrame(kApplicationHeroine, kApplicationButtonZ, 0); // heroine
		room->selectFrame(kApplicationHero, kApplicationButtonZ, 0); // hero
		room->selectFrame(kApplicationEnterButton, kApplicationButtonZ, 0);
		room->addStaticLayer(kApplicationText, kApplicationButtonZ);
		room->addStaticLayer(kApplicationHeroineText, kApplicationButtonZ);
		room->addStaticLayer(kApplicationHeroText, kApplicationButtonZ);
		room->addStaticLayer(kApplicationNameText, kApplicationButtonZ);
		room->pushHotZones(kApplicationHotspots);
		room->disableHotzone(kApplicationEnterHotzone);
		_applicationIsActive = true;
		g_vm->addTimer(19900, 5000);
	}

	void applicationClose() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		room->setLayerEnabled(kApplication, false);
		room->setLayerEnabled(kApplicationHeroine, false);
		room->setLayerEnabled(kApplicationHero, false);
		room->setLayerEnabled(kApplicationEnterButton, false);
		room->setLayerEnabled(kApplicationText, false);
		room->setLayerEnabled(kApplicationHeroineText, false);
		room->setLayerEnabled(kApplicationHeroText, false);
		room->setLayerEnabled(kApplicationNameText, false);
		room->setLayerEnabled(kApplicationBurntHole, false);
		room->setLayerEnabled(kApplicationUnfurls, false);
		room->popHotZones();
		clearNameInApplication();
		_applicationIsActive = false;
		// TODO: does it have any sound?
		room->playAnim(persistent->_gender == kFemale ? kApplicationRollsUpHeroine : kApplicationRollsUpHero,
			       kApplicationZ, PlayAnimParams::disappear(), 19019);
		renderLintel();
	}

	void cancelAllPhils() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();

		for (uint i = 0; i < ARRAYSIZE(allPhils); i++)
			room->stopAnim(allPhils[i]);
	}

	void updatePhilHotzone() {
		g_vm->getVideoRoom()->setHotZoneOffset(
			"phil", getPhilBase());
	}

	void renderLintel() {
		Common::SharedPtr<VideoRoom> room = g_vm->getVideoRoom();
		Persistent *persistent = g_vm->getPersistent();
		room->renderStringCentered("player lintel text", persistent->_heroName, Common::Point(942, 19), 900, -1);
	}

	Gender _gender;
	bool _isEnterMouseover;
	bool _philIsOnTheRight;
	int _philWalkPhase;
	bool _philIsBusy;
	Common::U32String _heroName;
	AmbientAnim _hercules;
	bool _endGameOutro;
	bool _applicationIsActive;
	bool _philSaidDoingGreat;
};

Common::SharedPtr<Hadesch::Handler> makeWallOfFameHandler() {
	return Common::SharedPtr<Hadesch::Handler>(new WallOfFameHandler());
}

}
