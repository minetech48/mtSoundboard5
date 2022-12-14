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
const std::string CONFIG_DIR = "resources/config/";

bimap<std::string, int> Soundboard::boardBindings;
bimap<std::string, int> Soundboard::soundBindings;
bimap<std::string, int> Soundboard::globalBindings;

std::string Soundboard::currentBoard;

bool holdingPTT = false;

bool Soundboard::boardBinding = false,
	 Soundboard::soundBinding = false,
	 Soundboard::globalBinding = false;

ConfigFile *Soundboard::config;

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
	eventMap.insert({"SBConfigSet", SBConfigSet});
	
	eventMap.insert({"GUIReset", GUIReset});
	eventMap.insert({"Shutdown", Shutdown});
	
	GUI::addSDLEventHandler(SDLEventInput);
	
	GUIData::addStringHandler("SBBoardBind", boardStringHandler);
	GUIData::addStringHandler("SBSoundBind", soundStringHandler);
	GUIData::addStringHandler("keybindings", keybindingsHandler);
	
	config = ConfigHandler::loadConfig("SBConfig");
	
	#ifdef unix
		if (config->intMap["SecondaryKeyboardID"] != 0)
			logf("Using secondary keyboard (id): %d\n", config->intMap["SecondaryKeyboardID"]);
	#endif
	
	ConfigHandler::saveConfig(config);
	
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
	
	if (SBAudio::activeSounds.size() > 0) {
		if (!holdingPTT) {
			holdingPTT = true;
			KeyboardHook::pressPTTKey();
		}
	}else{
		if (holdingPTT) {
			holdingPTT = false;
			KeyboardHook::releasePTTKey();
		}
	}
}

void Soundboard::SDLEventInput(SDL_Event event) {
	if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
		event.key.padding2 = 0;//hijacking padding2 to determine where keypress originates
	SDLEventHandler(event);
}
void Soundboard::SDLEventHandler(SDL_Event event) {
	switch (event.type) {
		case SDL_KEYDOWN:{
			int keyCode = event.key.keysym.sym;
			//logf("SDLKey %d: %d\n", event.key.padding2, keyCode);
			
			//disabling locks as per congfig
			if (!config->boolMap["UseCapslock"])
				event.key.keysym.mod&= ~KMOD_CAPS;
			if (!config->boolMap["UseNumlock"])
				event.key.keysym.mod&= ~KMOD_NUM;
			if (!config->boolMap["UseScrolllock"])
				event.key.keysym.mod&= ~KMOD_SCROLL;
			
			keyCode = keyCode | (event.key.keysym.mod << 8);
			int naitiveKey = event.key.padding3 | (event.key.keysym.mod << 8);
			//hijacking padding2 to determine where keypress originates
			bool localKey = !event.key.padding2;
			
			if (globalBinding) {
				if (!localKey)
					setBinding(&globalBindings,
						GUIData::getElement("SettingsMenu.KeybindList")->getReturnValue(), 
							naitiveKey);
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
			
			if (naitiveKey != 0 &&globalBindings.keysMap.contains(naitiveKey)
					&& globalBindings.getKey(naitiveKey) != "PushToTalk")
				EngineCore::broadcast(globalBindings.getKey(naitiveKey));
			
			if (localKey) break;
			
			if (boardBindings.keysMap.contains(keyCode))
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
			
			}break;
		case SDL_DROPFILE:
			logf("File: \"%s\" dropped, attempting to copy to selected soundboard directory: \"%s\"\n",
					event.drop.file, (SOUNDBOARDS_DIR + currentBoard).c_str());
			
			if (currentBoard != "" && std::filesystem::exists(SOUNDBOARDS_DIR + currentBoard)) {
				std::filesystem::copy(event.drop.file, (SOUNDBOARDS_DIR + currentBoard).c_str());
				GUIData::getList("sbSounds")
						->push_back(FileIO::removeFilePath(event.drop.file));
				
				logf("Copy successful!\n");
			}else{
				logf("Failed to copy file!");
			}
			break;
	}
}
bool KeyboardHook::naitiveKeySet(int keyCode) {
	ConfigFile* config = ConfigHandler::getConfig("runtime");
	
	if (config->boolMap["bindPTT"]) {
		Soundboard::config->intMap["PTTKey"] = keyCode;
		
		config->boolMap["bindPTT"] = false;
		
		return true;
	}
	
	return false;
}


void Soundboard::SBSelectBoard(EngineEvent event) {
	if (event.arg1 == "") return;
	
	currentBoard = event.arg1;
	loadSounds(event.arg1);
	
	UIElement *soundsList = GUIData::getElement("Soundboard.SoundsList");
	soundsList->scroll = 0;
	soundsList->listActive = -1;
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
	SBAudio::deviceIndex = GUIData::getElement("SettingsMenu.DeviceList")->listActive;
	
	GUIData::setString("AudioDevice1", SBAudio::playbackInfos[SBAudio::deviceIndex].name);
	config->stringMap["AudioDevice1"] = SBAudio::playbackInfos[SBAudio::deviceIndex].name;
	
	ConfigHandler::saveConfig(config);
}
void Soundboard::SetAudio2(EngineEvent event) {
	SBAudio::deviceIndex2 = GUIData::getElement("SettingsMenu.DeviceList")->listActive;
	
	GUIData::setString("AudioDevice2", SBAudio::playbackInfos[SBAudio::deviceIndex2].name);
	config->stringMap["AudioDevice2"] = SBAudio::playbackInfos[SBAudio::deviceIndex2].name;
	
	ConfigHandler::saveConfig(config);
}

void Soundboard::SBConfigSet(EngineEvent event) {
	if (config == NULL)
		config = ConfigHandler::getConfig("SBConfig");
		
	ConfigHandler::saveConfig(config);
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
	
	SBAudio::uninit();
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
	SBAudio::uninit();
}




void Soundboard::loadBoards() {
	if (!fs::exists(SOUNDBOARDS_DIR))
		return;
	
	GUIData::addList("sbBoards", {});
	
	std::vector<std::string>* boardsList = GUIData::getList("sbBoards");
	
	for (const auto& dir : fs::directory_iterator(SOUNDBOARDS_DIR)) {
		if (FileIO::getFileExtention(dir.path().generic_string()) != "") continue;
		
		logf("Soundboard directory found: %s\n", dir.path().c_str());
		
		boardsList->push_back(FileIO::removeFilePath(dir.path().generic_string()));
	}
	loadBindings(SOUNDBOARDS_DIR + "SBBind", &boardBindings);
	
	if (boardsList->size() == 0)
		GUIData::lists.erase("sbBoards");
}

void Soundboard::loadSounds(std::string boardName) {
	if (!fs::exists(SOUNDBOARDS_DIR + boardName)) {
		GUIData::getElement("Soundboard.BoardsList")->listActive = -1;
		return;
	}
	
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
	return KeyboardHook::toString(globalBindings.getValue(str));
}