#include "uiElement.h"

#include "uiParser.h"
#include "engine/logger.h"

ConfigFile *UIParser::globalConfig;

void UIParser::parseUIElement(UIElement* elementPtr, YAML::Node root) {
	if (root["position"]) elementPtr->defPosition = root["position"].as<vec2i>();
	if (root["size"]) elementPtr->defSize = root["size"].as<vec2i>();
	
	if (root["globalConfig"]) {
		globalConfig = ConfigHandler::getConfig(root["globalConfig"].as<std::string>());
	}
	
	if (root["bind"]) {//config bound switch
		if (root["config"])
			elementPtr->bindPtr = &ConfigHandler::getConfig(root["config"].as<std::string>())->
				boolMap[root["bind"].as<std::string>()];
		else if (globalConfig != NULL)
			elementPtr->bindPtr = &globalConfig->
				boolMap[root["bind"].as<std::string>()];
		else
			logf("Error parsing UIElement: globalConfig not found!\n");
			
		if (elementPtr->bindPtr != NULL) {
			elementPtr->active = *(bool*) elementPtr->bindPtr;
		}
	}
	
	if (root["elements"]) {
		YAML::Node elements = root["elements"];
		
		for (YAML::const_iterator i = elements.begin(); i != elements.end(); i++) {
			elementPtr->elements[i->first.as<std::string>()] = parseUIElement(i->second);
			
			elementPtr->elements[i->first.as<std::string>()].name = i->first.as<std::string>();
		}
	}
	
	root.remove("position");
	root.remove("size");
	
	root.remove("globalConfig");
	root.remove("config");
	root.remove("bind");
	
	root.remove("elements");
	
	for (YAML::const_iterator i = root.begin(); i != root.end(); i++) {
		if (i->IsDefined()) continue;
		
		//logf("test:%s\n", i->first.as<std::string>().c_str());
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
			FileIO::findFile(i->second["fontName"].as<std::string>(), ".ttf").c_str(),
			i->second["fontSize"].as<int>());
			
		if (font == NULL) {
			logf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
			continue;
		}
		
		GUIData::addFont(i->first.as<std::string>(), font);
		
		//logf("test: %s : %s\n", i->first.as<std::string>().c_str(), i->second["fontName"].as<std::string>().c_str());
	}
	
	for (YAML::const_iterator i = root["Colors"].begin(); i != root["Colors"].end(); i++) {
		if (i->second.IsSequence())
			GUIData::addColor(i->first.as<std::string>(), i->second.as<SDL_Color>());
		else//scalar/ string
			GUIData::addColor(i->first.as<std::string>(),
							root["Colors"][i->second.as<std::string>()].as<SDL_Color>());
	}
	
	YAML::Node langRoot = YAML::LoadFile(FileIO::findFile(root["Language"].as<std::string>(), ".yml"));
	for (YAML::const_iterator i = langRoot.begin(); i != langRoot.end(); i++) {
		GUIData::setString(i->first.as<std::string>(), i->second.as<std::string>());
	}
	
	Renderer::updateDefaultFont();
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