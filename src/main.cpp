#include "main.h"
#include <stdio.h>

#include <engine/engineCore.cpp>
#include <engine/gui/gui.cpp>
#include <soudboard/soundboard.h>

/*
	Todo:
		GUI Lists: auto load list file
		GUI: init script (lua?)
		GUI: system fonts
*/


int main(int argc, char* args[]) {
	EngineEvent engineEvent("testEvent");
	
	// EngineSystem* sysObj;
	// GUI guiObj;
	// sysObj = &guiObj;
	
	GUI guiObj;
	Soundboard sbObj;
	
	EngineCore::initSystem(&guiObj);
	EngineCore::initSystem(&sbObj);
	
	EngineCore::broadcast(engineEvent);
	
	EngineCore::broadcast("GUISetTheme", "gui/theming/DefaultTheme");
	EngineCore::broadcast("GUILoadList", "lists/testList");
	EngineCore::broadcast("GUIShow", "gui/Soundboard");
	
	
	EngineCore::start();
	
	return 0;
}
