#include "soundboard.h"

#include "engine/gui/gui.h"

void Soundboard::initialize() {
	eventMap.insert({"SBPlaySound", SBPlaySound});
	
	GUI::addSDLEventHandler(SDLEventHandler);
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
	
}

void Soundboard::SBPlaySound(EngineEvent event) {
	printf("playing sound %s\n", event.arg1.c_str());
}