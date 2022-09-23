#include "engineCore.h"
#include <stdio.h>
#include <queue>
#include <thread>

#include "util/stringReader.h"
#include "engine/logger.h"

std::vector<EngineSystem*> systems;
std::queue<EngineEvent> eventQueue;

bool EngineCore::running = true;

//broadcast overloads
void EngineCore::broadcast(EngineEvent event) {	
	eventQueue.push(event);
}
void EngineCore::broadcast(std::string eventTitle, std::string eventArg1) {
	broadcast(EngineEvent(eventTitle, eventArg1));
}
void EngineCore::broadcast(std::string eventTitle) {
	if (eventTitle == "") return;
	
	StringReader reader(eventTitle.c_str());
	std::string event = reader.advanceTo('(');
	std::string arg1 = reader.advanceTo(')');
	
	broadcast(event, arg1);
}

//initializing systems
void EngineCore::initSystem(EngineSystem* system) {
	logf("Engine//Initializing System...\n");
	
	logf("--System %s Initializing\n", system->title.c_str());
	
	system->initialize();
	
	logf("--System %s Initialized.\n", system->title.c_str());
	
	systems.push_back(system);
}

//initializing engine
void EngineCore::start() {
	//std::thread(runLoop).detach();
	
	runLoop();
	
	//broadcast("testEvent 2 lol, pog");
}

//update loop
void EngineCore::runLoop() {
	while (running) {
		//setting time for thread to wait for to execute next update (+20 ms/ 1/50 seconds)
		std::chrono::steady_clock::time_point nextTick = 
			std::chrono::steady_clock::now() +
			std::chrono::milliseconds(20);
		
		//sending events
		processEvents();
		
		//updating systems
		for (EngineSystem* system : systems) {
			system->update();
		}
		
		std::this_thread::sleep_until(nextTick);
		
	}
	
	
	broadcast(EngineEvent("Shutdown"));
	processEvents();
	
	for (EngineSystem* system : systems) {
		delete system;
	}
}

void EngineCore::processEvents() {
		while (!eventQueue.empty()) {
			EngineEvent event = eventQueue.front();
			//logf("Event: %s\n", event.event.c_str());
			logf("%s\n", event.toString().c_str());
			
			for (EngineSystem* system : systems) {
				system->handleEvent(event);
			}
			
			eventQueue.pop();
		}
}