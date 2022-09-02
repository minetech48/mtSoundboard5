#include "soundboard.h"

#include "engine/gui/gui.h"

//static std::map<std::string, std::function<void(EngineEvent)>> eventMap;

void Soundboard::initialize() {
	eventMap.insert({"SBPlaySound", SBPlaySound});
}

void Soundboard::handleEvent(EngineEvent event) {
	if (eventMap.find(event.event) != eventMap.end())
		eventMap[event.event](event);
}

void Soundboard::update() {}


void Soundboard::SBSelectBoard(EngineEvent event) {
	
}

void Soundboard::SBPlaySound(EngineEvent event) {
	printf("playing sound %s\n", event.arg1.c_str());
}