#pragma once

#include "engineEvent.h"
#include "engineSystem.h"

class EngineCore {
	public:
		static bool running;
		
		static void broadcast(EngineEvent event);
		static void broadcast(std::string eventTitle);
		static void broadcast(std::string eventTitle, std::string eventArg1);
		
		static void initSystem(EngineSystem* system);
		
		static void start();
	
	private:
		static void runLoop();
		static void processEvents();
};