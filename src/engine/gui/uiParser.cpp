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
			findFont(i->second["fontName"].as<std::string>()).c_str(),
			i->second["fontSize"].as<int>());
			
		if (font == NULL) {
			printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
			continue;
		}
		
		Renderer::addFont(i->first.as<std::string>(), font);
		
		//printf("test: %s : %s\n", i->first.as<std::string>().c_str(), i->second["fontName"].as<std::string>().c_str());
	}
	
	for (YAML::const_iterator i = root["Colors"].begin(); i != root["Colors"].end(); i++) {
		if (i->second.IsSequence())
			Renderer::addColor(i->first.as<std::string>(), i->second.as<SDL_Color>());
		else//scalar/ string
			Renderer::addColor(i->first.as<std::string>(),
							root["Colors"][i->second.as<std::string>()].as<SDL_Color>());
	}
}

#define checkFile(name) if (std::filesystem::exists({name})) return name;
std::string UIParser::findFont(std::string fileName) {
	namespace fs = std::filesystem;
	fileName+=".ttf";
	
	//if (fs::exists({fileName})) return fileName;
	
	//if (checkFile(fileName)) return fileName;
	
	checkFile(fileName);
	checkFile("resources/" + fileName);
	checkFile("resources/fonts/" + fileName);
	
	return fileName;
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