#pragma once

#include <string>

#include "../engine/engineSystem.h"
#include "../engine/engineEvent.h"
#include "../engine/util/bimap.h"

#include <functional>
#include <map>
#include <SDL.h>

class Soundboard : public EngineSystem {
		
	static bimap<std::string, int> boardBindings;
	static bimap<std::string, int> soundBindings;
	
	public:
		Soundboard() {title = "Soundboard";}
		
		void initialize();
		void handleEvent(EngineEvent event);
		void update();

		static void SBSelectBoard(EngineEvent event);
		static void SBPlaySound(EngineEvent event);

	static void SDLEventHandler(SDL_Event event);
	
	
	static void loadBoards();
	static void loadSounds(std::string boardName);
};


static std::map<std::string, std::function<void(EngineEvent)>> eventMap;