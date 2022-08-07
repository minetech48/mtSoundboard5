#ifndef ENGINE_SYSTEM_H
#define ENGINE_SYSTEM_H

class EngineSystem {
	public:
		virtual void initialize() {};
		virtual void update() {};
		virtual void handleEvent(EngineEvent event) {};
};

#endif