#pragma once

#include <string>

#include "../engine/engineSystem.h"
#include "../engine/engineEvent.h"

#include <functional>
#include <map>
#include <SDL.h>

class Soundboard : public EngineSystem {
	public:
		void initialize();
		void handleEvent(EngineEvent event);
		void update();

		static void SBSelectBoard(EngineEvent event);
		static void SBPlaySound(EngineEvent event);

	static void SDLEventHandler(SDL_Event event);
};


static std::map<std::string, std::function<void(EngineEvent)>> eventMap;