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

#include "common/config-manager.h"

#include "tetraedge/detection.h"
#include "tetraedge/metaengine.h"
#include "tetraedge/detection_tables.h"

const DebugChannelDef TetraedgeMetaEngineDetection::debugFlagList[] = {
	{ Tetraedge::kDebugGraphics, "Graphics", "Graphics debug level" },
	{ Tetraedge::kDebugPath, "Path", "Pathfinding debug level" },
	{ Tetraedge::kDebugFilePath, "FilePath", "File path debug level" },
	{ Tetraedge::kDebugScan, "Scan", "Scan for unrecognised games" },
	{ Tetraedge::kDebugScript, "Script", "Enable debug script dump" },
	DEBUG_CHANNEL_END
};

TetraedgeMetaEngineDetection::TetraedgeMetaEngineDetection() : AdvancedMetaEngineDetection(Tetraedge::GAME_DESCRIPTIONS,
	sizeof(ADGameDescription), Tetraedge::GAME_NAMES) {
	_flags = kADFlagMatchFullPaths;
}

Common::String TetraedgeMetaEngineDetection::customizeGuiOptionsLanguages(const Common::String &optionsString, const Common::String &domain) const {
	Common::String result;

	struct {
		Common::Language id;
		const char *code;
	} languages[] = {
		{ Common::EN_ANY, "en" },
		{ Common::FR_FRA, "fr" },
		{ Common::DE_DEU, "de" },
		{ Common::IT_ITA, "it" },
		{ Common::ES_ESP, "es" },
		{ Common::RU_RUS, "ru" },
		{ Common::HE_ISR, "he" }  // This is a Fan-translation, which requires additional patch
	};

	bool hasLang[ARRAYSIZE(languages)];

	memset(hasLang, 0, sizeof(hasLang));

	const Common::Platform platform = Common::parsePlatform(ConfMan.get("platform", domain));

	Common::FSNode dir(ConfMan.get("path", domain));

	if (platform == Common::Platform::kPlatformMacintosh)
		dir = dir.getChild("Resources");

	for (uint i = 0; i < ARRAYSIZE(languages); i++)
		if (dir.getChild("texts").getChild(Common::String::format("%s.xml", languages[i].code).c_str()).exists())
			hasLang[i] = true;

	for (uint i = 0; i < ARRAYSIZE(languages); i++)
		if(hasLang[i])
			result += " " + Common::getGameGUIOptionsDescriptionLanguage(languages[i].id);

	return result;
}

REGISTER_PLUGIN_STATIC(TETRAEDGE_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, TetraedgeMetaEngineDetection);
