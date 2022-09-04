#include "soundboard.h"

#include "engine/gui/gui.h"
#include "engine/gui/guiData.h"
#include "engine/util/fileIO.h"

#include <filesystem>
namespace fs = std::filesystem;

const std::string SOUNDBOARDS_DIR = "resources/soundboards/";

void Soundboard::initialize() {
	eventMap.insert({"SBPlaySound", SBPlaySound});
	eventMap.insert({"SBSelectBoard", SBSelectBoard});
	
	GUI::addSDLEventHandler(SDLEventHandler);
	
	loadBoards();
}

void Soundboard::handleEvent(EngineEvent event) {
	if (eventMap.find(event.event) != eventMap.end())
		eventMap[event.event](event);
}

void Soundboard::update() {}

void Soundboard::SDLEventHandler(SDL_Event event) {
	switch (event.type) {
		case SDL_KEYDOWN:
			printf("sbeventhandle\n");
			break;
	}
}


void Soundboard::SBSelectBoard(EngineEvent event) {
	loadSounds(event.arg1);
}

void Soundboard::SBPlaySound(EngineEvent event) {
	printf("playing sound %s\n", event.arg1.c_str());
}


void Soundboard::loadBoards() {
	GUIData::addList("sbBoards", {});
	
	std::vector<std::string>* boardsList = GUIData::getList("sbBoards");
	
	for (const auto& dir : fs::directory_iterator(SOUNDBOARDS_DIR)) {
		printf("soundboard directory found: %s\n", dir.path().c_str());
		
		boardsList->push_back(FileIO::getFileName(dir.path()));
	}
}

void Soundboard::loadSounds(std::string boardName) {
	GUIData::addList("sbSounds", {});
	
	std::vector<std::string>* soundsList = GUIData::getList("sbSounds");
	
	for (const auto& dir : fs::directory_iterator(SOUNDBOARDS_DIR + boardName)) {
		printf("sound file found: %s\n", dir.path().c_str());
		
		soundsList->push_back(FileIO::getFileName(dir.path()));
	}
}