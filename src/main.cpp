#include "main.h"
#include <stdio.h>

#include <engine/engineCore.h>
#include <engine/gui/gui.h>
#include <soudboard/soundboard.h>

#include "engine/logger.h"

/*
	Todo:
		GUI Lists: auto load list file
		GUI: init script (lua?)
		GUI: system fonts
		
		SB: audio playback
		SB: Windows keyboard hook
		SB: ensure .mp3 and .ogg files can be played
*/


int main(int argc, char* args[]) {
	Logger::init();
	
	EngineCore::initSystem(new GUI());
	EngineCore::initSystem(new Soundboard());
	
	EngineCore::broadcast("GUISetTheme", "gui/theming/DefaultTheme");
	EngineCore::broadcast("GUILoadList", "lists/testList");
	EngineCore::broadcast("GUIShow", "gui/Soundboard");
	
	EngineCore::start();
	
	return 0;
}
