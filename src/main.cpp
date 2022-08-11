#include "main.h"
#include <stdio.h>

#include <engine/engineCore.cpp>
#include <engine/gui/gui.cpp>

int main(int argc, char* args[]) {
	EngineEvent engineEvent("testEvent");
	
	// EngineSystem* sysObj;
	// GUI guiObj;
	// sysObj = &guiObj;
	
	GUI guiObj;
	EngineSystem* sysObj = &guiObj;
	
	EngineCore::initSystem(&guiObj);
	
	EngineCore::broadcast(engineEvent);
	
	EngineCore::broadcast("GUISetTheme", "gui/theming/DefaultTheme.yml");
	EngineCore::broadcast("GUIShow", "gui/Soundboard.yml");
	
	EngineCore::start();
	
	return 0;
}
