#include "soundboard.h"

#include "engine/gui/gui.h"
#include "engine/gui/guiData.h"
#include "engine/util/fileIO.h"
#include "engine/logger.h"
#include "keyboardHook.h"

#include <fstream>
namespace fs = std::filesystem;

const int OPTIONAL_MODIFIER_MASK = (KMOD_CAPS | KMOD_NUM) << 8;

const std::string SOUNDBOARDS_DIR = "resources/soundboards/";

bimap<std::string, int> Soundboard::boardBindings;
bimap<std::string, int> Soundboard::soundBindings;
bimap<std::string, int> Soundboard::globalBindings;

std::string Soundboard::currentBoard;

bool Soundboard::boardBinding = false,
	 Soundboard::soundBinding = false,
	 Soundboard::globalBinding = false;


void Soundboard::initialize() {
	eventMap.insert({"SBPlaySound", SBPlaySound});
	eventMap.insert({"SBPlayCurrent", SBPlayCurrent});
	eventMap.insert({"SBSelectBoard", SBSelectBoard});
	eventMap.insert({"SBBoardBinding", SBBoardBinding});
	eventMap.insert({"SBBoardBindingR", SBBoardBindingR});
	eventMap.insert({"SBSoundBinding", SBSoundBinding});
	eventMap.insert({"SBSoundBindingR", SBSoundBindingR});
	eventMap.insert({"SBGlobalBinding", SBGlobalBinding});
	eventMap.insert({"SBGlobalBindingR", SBGlobalBindingR});
	eventMap.insert({"SBClearBinding", SBClearBinding});
	eventMap.insert({"SBStopSounds", SBStopSounds});
	eventMap.insert({"SetAudio1", SetAudio1});
	eventMap.insert({"SetAudio2", SetAudio2});
	
	eventMap.insert({"GUIReset", GUIReset});
	eventMap.insert({"Shutdown", Shutdown});
	
	GUI::addSDLEventHandler(SDLEventInput);
	
	GUIData::addStringHandler("SBBoardBind", boardStringHandler);
	GUIData::addStringHandler("SBSoundBind", soundStringHandler);
	GUIData::addStringHandler("keybindings", keybindingsHandler);
	
	loadBoards();
	
	logf("Starting audio player:\n");
	SBAudio::initialize();
	
	
	logf("Starting keyboard hook:\n");
	KeyboardHook::initialize();
	
	loadBindings("resources/config/keybindings" , &globalBindings);
}

void Soundboard::handleEvent(EngineEvent event) {
	if (eventMap.find(event.event) != eventMap.end())
		eventMap[event.event](event);
}

void Soundboard::update() {
	KeyboardHook::pollEvents();
	
	SBAudio::update();
}

void Soundboard::SDLEventInput(SDL_Event event) {
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
		event.key.padding2 = 0;//hijacking padding2 to determine where keypress originates
	SDLEventHandler(event);
}
void Soundboard::SDLEventHandler(SDL_Event event) {
	
	switch (event.type) {
		case SDL_KEYDOWN:
			int keyCode = event.key.keysym.sym;
			//logf("SDLKey %d: %d\n", event.key.padding2, keyCode);
			
			keyCode = keyCode | (event.key.keysym.mod << 8);
			//hijacking padding2 to determine where keypress originates
			bool localKey = !event.key.padding2;
			
			if (globalBinding) {
				if (localKey)
					setBinding(&globalBindings,
						GUIData::getElement("SettingsMenu.KeybindList")->getReturnValue(), keyCode);
			}else if (boardBinding) {
				if (localKey)
					setBinding(&boardBindings,
						GUIData::getElement("BoardsList")->getReturnValue(), keyCode);
				break;
			}else if (soundBinding) {
				if (localKey)
					setBinding(&soundBindings,
						GUIData::getElement("SoundsList")->getReturnValue(), keyCode);
				break;
			}
			if (localKey) break;
			
			if (globalBindings.keysMap.contains(keyCode) && globalBindings.getKey(keyCode) != "PushToTalk")
				EngineCore::broadcast(globalBindings.getKey(keyCode));
			else if (boardBindings.keysMap.contains(keyCode))
				EngineCore::broadcast("SBSelectBoard", boardBindings.getKey(keyCode));
			else {
				#define playsound(key) \
					if (soundBindings.keysMap.contains(keyCode)) { \
						SBAudio::playSound(SOUNDBOARDS_DIR + currentBoard + "/" + \
												soundBindings.getKey(keyCode)); \
						break; \
					}
				playsound(keyCode)//playing sound if it exists
				keyCode = keyCode & (~OPTIONAL_MODIFIER_MASK);
				playsound(keyCode)//else, play sound without modifiers
				keyCode = keyCode | KMOD_CAPS << 8;
				playsound(keyCode)//testing with caps
				keyCode = keyCode | KMOD_NUM << 8;
				playsound(keyCode)//testing with caps and num lock
				keyCode = keyCode & ~KMOD_CAPS << 8;
				playsound(keyCode)//testing with num lock
			}
			
			break;
	}
}


void Soundboard::SBSelectBoard(EngineEvent event) {
	if (event.arg1 == "") return;
	
	currentBoard = event.arg1;
	loadSounds(event.arg1);
}

void Soundboard::SBPlaySound(EngineEvent event) {
	//logf("playing sound %s\n", event.arg1.c_str());
	SBAudio::playSound(SOUNDBOARDS_DIR + currentBoard + "/" + event.arg1);
}
void Soundboard::SBPlayCurrent(EngineEvent event) {
	event.arg1 = GUIData::getElement("Soundboard.SoundsList")->getReturnValue();
	SBPlaySound(event);
}
void Soundboard::SBStopSounds(EngineEvent event) {
	SBAudio::stopAll();
}

void Soundboard::SBBoardBindingR(EngineEvent event) {
	boardBinding = false;
	saveBindings(SOUNDBOARDS_DIR + "SBBind.txt", &boardBindings);
}
void Soundboard::SBSoundBindingR(EngineEvent event) {
	soundBinding = false;
	saveBindings(SOUNDBOARDS_DIR + currentBoard + "/SBBind.txt", &soundBindings);
}
void Soundboard::SBGlobalBindingR(EngineEvent event) {
	globalBinding = false;
	saveBindings("resources/config/keybindings.txt", &globalBindings);
}

void Soundboard::SBClearBinding(EngineEvent event) {
	if (GUIData::getElement("Soundboard.SoundsList")->getReturnValue() == "") {
		boardBindings.removeValue(GUIData::getElement("Soundboard.BoardsList")->getReturnValue());
		saveBindings(SOUNDBOARDS_DIR + "SBBind.txt", &boardBindings);
	}else {
		soundBindings.removeValue(GUIData::getElement("Soundboard.SoundsList")->getReturnValue());
		saveBindings(SOUNDBOARDS_DIR + currentBoard + "/SBBind.txt", &soundBindings);
	}
}

void Soundboard::SetAudio1(EngineEvent event) {
	SBAudio::engineIndex = GUIData::getElement("SettingsMenu.DeviceList")->listActive;
	GUIData::setString("AudioDevice1", SBAudio::playbackInfos[SBAudio::engineIndex].name);
}
void Soundboard::SetAudio2(EngineEvent event) {
	SBAudio::engineIndex2 = GUIData::getElement("SettingsMenu.DeviceList")->listActive;
	GUIData::setString("AudioDevice2", SBAudio::playbackInfos[SBAudio::engineIndex2].name);
}

void Soundboard::GUIReset(EngineEvent event) {
	boardBindings.clear();
	soundBindings.clear();
	
	SBAudio::stopAll();
	
	loadBoards();
	if (currentBoard != "")
		loadSounds(currentBoard);
	
	KeyboardHook::uninitialize();
	KeyboardHook::initialize();

	SBAudio::initialize();
}

void Soundboard::Shutdown(EngineEvent event) {
	if (boardBinding)
		saveBindings(SOUNDBOARDS_DIR + "SBBind.txt", &boardBindings);
	if (soundBinding)
		saveBindings(SOUNDBOARDS_DIR + currentBoard + "/SBBind.txt", &soundBindings);
	if (globalBinding)
		saveBindings("resources/config/keybindings.txt", &globalBindings);
	
	KeyboardHook::uninitialize();
}


void Soundboard::loadBoards() {
	GUIData::addList("sbBoards", {});
	
	std::vector<std::string>* boardsList = GUIData::getList("sbBoards");
	
	for (const auto& dir : fs::directory_iterator(SOUNDBOARDS_DIR)) {
		if (FileIO::getFileExtention(dir.path().generic_string()) != "") continue;
		
		logf("Soundboard directory found: %s\n", dir.path().c_str());
		
		boardsList->push_back(FileIO::removeFilePath(dir.path().generic_string()));
	}
	loadBindings(SOUNDBOARDS_DIR + "SBBind", &boardBindings);
}

void Soundboard::loadSounds(std::string boardName) {
	GUIData::addList("sbSounds", {});
	
	std::vector<std::string>* soundsList = GUIData::getList("sbSounds");
	
	for (const auto& dir : fs::directory_iterator(SOUNDBOARDS_DIR + boardName)) {
		//logf("sound file found: %s\n", dir.path().c_str());
		
		soundsList->push_back(FileIO::removeFilePath(dir.path().generic_string()));
	}
	loadBindings(SOUNDBOARDS_DIR + boardName + "/SBBind", &soundBindings);
}

void Soundboard::loadBindings(std::string filePath, bimap<std::string, int>* bindingMap) {
	std::ifstream file(FileIO::findFile(filePath, ".txt"));
	bindingMap->clear();
	
	if (file.is_open()) {
		logf("Loading bindings file: %s.txt\n", filePath.c_str());
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
		logf("No binding file found: %s.txt\n", filePath.c_str());
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
std::string Soundboard::keybindingsHandler(std::string str) {
	return keyToString(globalBindings.getValue(str));
}