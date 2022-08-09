#include "engineCore.h"
#include <stdio.h>
#include <queue>
#include <thread>

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
	broadcast(EngineEvent(eventTitle));
}

//initializing systems
void EngineCore::initSystem(EngineSystem* system) {
	printf("Engine//Initializing System...\n");
	
	system->initialize();
	
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
}

void EngineCore::processEvents() {
		while (!eventQueue.empty()) {
			EngineEvent event = eventQueue.front();
			//printf("Event: %s\n", event.event.c_str());
			printf("%s\n", event.toString().c_str());
			
			for (EngineSystem* system : systems) {
				system->handleEvent(event);
			}
			
			eventQueue.pop();
		}
}