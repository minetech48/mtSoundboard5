#ifndef ENGINE_SYSTEM_H
#define ENGINE_SYSTEM_H

#include "engineEvent.h"

class EngineSystem {
	public:
		std::string title;
		
		virtual void initialize() {};
		virtual void update() {};
		virtual void handleEvent(EngineEvent event) {};
};

#endif