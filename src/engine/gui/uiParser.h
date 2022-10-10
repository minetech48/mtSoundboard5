#include <yaml-cpp/yaml.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <filesystem>

#include "uiElement.h"
#include "renderer.h"
#include "guiData.h"
#include "../util/fileIO.h"
#include "engine/util/configHandler.h"

class UIParser {
	static ConfigFile *globalConfig;
	
	public:
		static void parseUIElement(UIElement* elementPtr, YAML::Node root);
		static UIElement parseUIElement(YAML::Node root);
		
		static void loadTheme(YAML::Node root);
};
