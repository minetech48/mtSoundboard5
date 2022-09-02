#pragma once

#include "../engine/engineSystem.h"

#include <functional>
#include <map>

class Soundboard : public EngineSystem {
	public:
		void initialize();
		void handleEvent(EngineEvent event);
		void update();
};


static std::map<std::string, std::function<void(EngineEvent)>> eventMap;


static void SBPlaySound(EngineEvent event);