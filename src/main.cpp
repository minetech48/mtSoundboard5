#include <stdio.h>

#include <engine/engineCore.h>
#include <engine/gui/gui.h>
#include <soudboard/soundboard.h>

#include "engine/logger.h"

int main(int argc, char* args[]) {
	Logger::init();
	

	GUI::WINDOW_NAME = "mtSoundboard V5";
	EngineCore::initSystem(new GUI());
	EngineCore::initSystem(new Soundboard());
	
	EngineCore::broadcast("GUISetTheme", "gui/theming/DefaultTheme");
	EngineCore::broadcast("GUIShow", "gui/Soundboard");
	
	EngineCore::start();
	
	return 0;
}
