#include "soundboard.h"

#include "engine/gui/gui.h"
#include "engine/gui/guiData.h"
#include "engine/util/fileIO.h"
#include "keyboardHook.h"

#include <fstream>
namespace fs = std::filesystem;

const std::string SOUNDBOARDS_DIR = "resources/soundboards/";

bimap<std::string, int> Soundboard::boardBindings;
bimap<std::string, int> Soundboard::soundBindings;

std::string Soundboard::currentBoard;

bool Soundboard::boardBinding = false,
	 Soundboard::soundBinding = false;


void Soundboard::initialize() {
	eventMap.insert({"SBPlaySound", SBPlaySound});
	eventMap.insert({"SBSelectBoard", SBSelectBoard});
	eventMap.insert({"SBBoardBinding", SBBoardBinding});
	eventMap.insert({"SBBoardBindingR", SBBoardBindingR});
	eventMap.insert({"SBSoundBinding", SBSoundBinding});
	eventMap.insert({"SBSoundBindingR", SBSoundBindingR});
	
	eventMap.insert({"GUIReset", GUIReset});
	eventMap.insert({"Shutdown", Shutdown});
	
	GUI::addSDLEventHandler(SDLEventHandler);
	
	GUIData::addStringHandler("SBBoardBind", boardStringHandler);
	GUIData::addStringHandler("SBSoundBind", soundStringHandler);
	
	loadBoards();
	
	printf("Starting audio player:\n");
	SBAudio::initialize();
	
	printf("Starting keyboard hook:\n");
	KeyboardHook::initialize();
}

void Soundboard::handleEvent(EngineEvent event) {
	if (eventMap.find(event.event) != eventMap.end())
		eventMap[event.event](event);
}

void Soundboard::update() {
	KeyboardHook::pollEvents();
}

void Soundboard::SDLEventHandler(SDL_Event event) {
	
	switch (event.type) {
		case SDL_KEYDOWN:
			int keyCode = event.key.keysym.sym;
			
			keyCode = keyCode | (event.key.keysym.mod << 8);
			//hijacking padding2 to determine where keypress originates
			bool localKey = !event.key.padding2;
			
			if (boardBinding) {
				if (localKey)
					setBinding(&boardBindings,
						GUIData::getElement("BoardsList")->getReturnValue(), keyCode);
				return;
			}else if (soundBinding) {
				if (localKey)
					setBinding(&soundBindings,
						GUIData::getElement("SoundsList")->getReturnValue(), keyCode);
				return;
			}
			if (localKey) return;
			
			if (boardBindings.keysMap.contains(keyCode))
				EngineCore::broadcast("SBSelectBoard", boardBindings.getKey(keyCode));
			else if (soundBindings.keysMap.contains(keyCode))
				SBAudio::playSound(SOUNDBOARDS_DIR + currentBoard + "/" +
										 soundBindings.getKey(keyCode));
			
			break;
	}
}


void Soundboard::SBSelectBoard(EngineEvent event) {
	if (event.arg1 == "") return;
	
	currentBoard = event.arg1;
	loadSounds(event.arg1);
}

void Soundboard::SBPlaySound(EngineEvent event) {
	//printf("playing sound %s\n", event.arg1.c_str());
	SBAudio::playSound(SOUNDBOARDS_DIR + currentBoard + "/" + event.arg1);
}

void Soundboard::SBBoardBindingR(EngineEvent event) {
	boardBinding = false;
	saveBindings(SOUNDBOARDS_DIR + "SBBind.txt", &boardBindings);
}
void Soundboard::SBSoundBindingR(EngineEvent event) {
	soundBinding = false;
	saveBindings(SOUNDBOARDS_DIR + currentBoard + "/SBBind.txt", &soundBindings);
}


void Soundboard::GUIReset(EngineEvent event) {
	boardBindings.clear();
	soundBindings.clear();
	
	loadBoards();
	if (currentBoard != "")
		loadSounds(currentBoard);
	
	KeyboardHook::initialize();
}

void Soundboard::Shutdown(EngineEvent event) {
	if (boardBinding)
		saveBindings(SOUNDBOARDS_DIR + "SBBind.txt", &boardBindings);
	if (soundBinding)
		saveBindings(SOUNDBOARDS_DIR + currentBoard + "/SBBind.txt", &soundBindings);
}


void Soundboard::loadBoards() {
	GUIData::addList("sbBoards", {});
	
	std::vector<std::string>* boardsList = GUIData::getList("sbBoards");
	
	for (const auto& dir : fs::directory_iterator(SOUNDBOARDS_DIR)) {
		if (FileIO::getFileExtention(dir.path().generic_string()) != "") continue;
		
		printf("Soundboard directory found: %s\n", dir.path().c_str());
		
		boardsList->push_back(FileIO::removeFilePath(dir.path().generic_string()));
	}
	loadBindings(SOUNDBOARDS_DIR + "SBBind", &boardBindings);
}

void Soundboard::loadSounds(std::string boardName) {
	GUIData::addList("sbSounds", {});
	
	std::vector<std::string>* soundsList = GUIData::getList("sbSounds");
	
	for (const auto& dir : fs::directory_iterator(SOUNDBOARDS_DIR + boardName)) {
		//printf("sound file found: %s\n", dir.path().c_str());
		
		soundsList->push_back(FileIO::removeFilePath(dir.path().generic_string()));
	}
	loadBindings(SOUNDBOARDS_DIR + boardName + "/SBBind", &soundBindings);
}

void Soundboard::loadBindings(std::string filePath, bimap<std::string, int>* bindingMap) {
	std::ifstream file(FileIO::findFile(filePath, ".txt"));
	bindingMap->clear();
	
	if (file.is_open()) {
		printf("Loading bindings file: %s.txt\n", filePath.c_str());
		std::string line;
		int delimiterIndex;
		
		while (std::getline(file, line)) {
			delimiterIndex = line.find_last_of(':');
			
			setBinding(bindingMap,
				line.substr(0, delimiterIndex),
				std::stoi(line.substr(delimiterIndex+1)));
			
		}
		
		
		file.close();
	}else{
		printf("No binding file found: %s.txt\n", filePath.c_str());
	}
}

void Soundboard::setBinding(bimap<std::string, int>* bindingMap, std::string action, int trigger) {
	bindingMap->insert(action, trigger);
}

void Soundboard::saveBindings(std::string filePath, bimap<std::string, int>* bindingMap) {
	std::ofstream file;
	file.open(filePath);
	
	for (auto& entry : bindingMap->valuesMap) {
		file << entry.first + ":" + std::to_string(entry.second) + '\n';
	}
	file.close();
}

#define keyMod(enumName, text) if (modifierMasks & enumName) toReturn+= text;
std::string Soundboard::keyToString(int keyCode) {
	int modifierMasks = (keyCode & 0xFFFF00) >> 8;
	keyCode = keyCode & 0xFF0000FF;
	
	std::string toReturn;
	
	if (modifierMasks != 0) {
		keyMod(KMOD_LCTRL, "Ctrl+ ")
		keyMod(KMOD_RCTRL, "RCtrl+ ")
		keyMod(KMOD_LALT, "Alt+ ")
		keyMod(KMOD_RALT, "RAlt+ ")
		keyMod(KMOD_LSHIFT, "Shift+ ")
		keyMod(KMOD_RSHIFT, "RShift+ ")
		keyMod(KMOD_LGUI, "Win+ ")
		keyMod(KMOD_RGUI, "RWin+ ")
		
		keyMod(KMOD_CAPS, "^")
		keyMod(KMOD_NUM, "#")
		keyMod(KMOD_SCROLL, "|")
	}
	
	toReturn+= SDL_GetKeyName(keyCode);
	
	return toReturn;
}
#undef keyMod

std::string Soundboard::boardStringHandler(std::string str) {
	return keyToString(boardBindings.getValue(str));
}
std::string Soundboard::soundStringHandler(std::string str) {
	return keyToString(soundBindings.getValue(str));
}