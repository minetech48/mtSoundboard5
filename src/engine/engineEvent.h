#ifndef ENGINE_EVENT_H
#define ENGINE_EVENT_H

#include <string>

struct EngineEvent {
	EngineEvent(std::string eventTitle)
	 : event(eventTitle){}
	EngineEvent(std::string eventTitle, std::string eventArg1)
	 : event(eventTitle) 
	 , arg1(eventArg1) {}
	
	std::string event;
	std::string arg1;
	
	//toString function
	std::string toString() {
		std::string toReturn;
		
		toReturn+= "[Event: " + event;
		
		if (arg1.length() > 0) {
			toReturn+= ", " + arg1;
		}
		
		toReturn+= "]";
		
		return toReturn;
	}
};

#endif