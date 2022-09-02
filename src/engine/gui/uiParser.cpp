#include "uiElement.h"

#include "uiParser.h"

void UIParser::parseUIElement(UIElement* elementPtr, YAML::Node root) {
	if (root["position"]) elementPtr->defPosition = root["position"].as<vec2i>();
	if (root["size"]) elementPtr->defSize = root["size"].as<vec2i>();
	
	if (root["elements"]) {
		YAML::Node elements = root["elements"];
		
		for (YAML::const_iterator i = elements.begin(); i != elements.end(); i++) {
			elementPtr->elements[i->first.as<std::string>()] = parseUIElement(i->second);
			elementPtr->elements[i->first.as<std::string>()].name = i->first.as<std::string>();
		}
	}
	
	root.remove("position");
	root.remove("size");
	
	root.remove("elements");
	
	for (YAML::const_iterator i = root.begin(); i != root.end(); i++) {
		if (i->IsDefined()) continue;
		
		//printf("test:%s\n", i->first.as<std::string>().c_str());
		elementPtr->metadata[i->first.as<std::string>()] = i->second;
	}
}

UIElement UIParser::parseUIElement(YAML::Node root) {
	UIElement element;
	
	parseUIElement(&element, root);
	
	return element;
}


void UIParser::loadTheme(YAML::Node root) {
	for (YAML::const_iterator i = root["Fonts"].begin(); i != root["Fonts"].end(); i++) {
		
		TTF_Font* font = TTF_OpenFont(
			findFile(i->second["fontName"].as<std::string>(), ".ttf").c_str(),
			i->second["fontSize"].as<int>());
			
		if (font == NULL) {
			printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
			continue;
		}
		
		GUIData::addFont(i->first.as<std::string>(), font);
		
		//printf("test: %s : %s\n", i->first.as<std::string>().c_str(), i->second["fontName"].as<std::string>().c_str());
	}
	
	for (YAML::const_iterator i = root["Colors"].begin(); i != root["Colors"].end(); i++) {
		if (i->second.IsSequence())
			GUIData::addColor(i->first.as<std::string>(), i->second.as<SDL_Color>());
		else//scalar/ string
			GUIData::addColor(i->first.as<std::string>(),
							root["Colors"][i->second.as<std::string>()].as<SDL_Color>());
	}
	
	YAML::Node langRoot = YAML::LoadFile(findFile(root["Language"].as<std::string>(), ".yml"));
	for (YAML::const_iterator i = langRoot.begin(); i != langRoot.end(); i++) {
		GUIData::setString(i->first.as<std::string>(), i->second.as<std::string>());
	}
	
	Renderer::updateDefaultFont();
}

//finding file on system
#define checkFile(name) if (std::filesystem::exists({name})) return name;
std::string UIParser::findFile(std::string fileName, std::string extension) {
	namespace fs = std::filesystem;
	fileName+= extension;
	
	switch (hash(extension)) {
		case hash(".ttf"):
			checkFile("resources/fonts/" + fileName);
			break;
			
		case hash(".yml"):
			checkFile("resources/gui/" + fileName);
			checkFile("resources/gui/menus/" + fileName);
			checkFile("resources/gui/lang/" + fileName)
			break;
			
		case hash(".txt"):
			checkFile("resources/gui/" + fileName);
			checkFile("resources/gui/lists/" + fileName);
			break;
	}
	
	checkFile("resources/" + fileName);
	checkFile(fileName);
	
	return "FILE_NOT_FOUND: " + fileName;
}
#undef checkFile

std::string UIParser::getFileName(std::string filePath) {
	int start = filePath.find_last_of('/')+1;
	int end = filePath.find_last_of('.');
	
	return filePath.substr(start, end-start);
}

//YAML Object Conversions
namespace YAML {
	//SDL_Color
		template<>
		struct convert<SDL_Color> {
		static Node encode(const SDL_Color& rhs) {
			Node node;
			
			node.push_back(rhs.r);
			node.push_back(rhs.g);
			node.push_back(rhs.b);
			
			if (rhs.a != 255)
				node.push_back(rhs.a);
			
			return node;
		}

		static bool decode(const Node& node, SDL_Color& rhs) {
			if(!node.IsSequence() || node.size() < 3) {
				return false;
			}
			
			rhs.r = node[0].as<Uint8>();
			rhs.g = node[1].as<Uint8>();
			rhs.b = node[2].as<Uint8>();
			
			if(node.size() == 4)
				rhs.a = node[3].as<Uint8>();
			else
				rhs.a = 255;
			
			return true;
		}
	};
}