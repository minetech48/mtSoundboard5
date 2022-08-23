#include <yaml-cpp/yaml.h>
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <filesystem>

#include "uiElement.h"
#include "renderer.h"
#include "guiData.h"

class UIParser {
	public:
		static void parseUIElement(UIElement* elementPtr, YAML::Node root);
		static UIElement parseUIElement(YAML::Node root);
		
		static void loadTheme(YAML::Node root);
		
		static std::string findFile(std::string fileName, std::string extension);
		static std::string getFileName(std::string filePath);
};
