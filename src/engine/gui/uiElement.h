#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "vec2i.h"
#include <yaml-cpp/yaml.h>
#include <unordered_map>

struct UIElement {
	vec2i position, defPosition;
	vec2i position2, defSize;
	
	std::unordered_map<std::string, YAML::Node> metadata;
	std::unordered_map<std::string, UIElement> elements;
	
	//metadata examples
	//onClick
	//type
	//text + textStyle
	//ect...
	
	//container functions
	bool isContainer() {return !elements.empty();}
	bool isGridContainer() {return metadata.find("gridSize") != metadata.end();}
	
	//grid functions
	vec2i getGridSize() {return metadata["gridSize"].as<vec2i>();}
	vec2i getGridTileSize() {
		vec2i gridSize = getGridSize();
		
		return {getSizeX() / gridSize.x, getSizeY() / gridSize.y};
	}
	
	//element getters
	int getSizeX() {return position2.x - position.x;}
	int getSizeY() {return position2.y - position.y;}
	vec2i getSize() {return {getSizeX(), getSizeY()};}
	
	public:
		static void alignElement(UIElement* parentPtr, UIElement* elementPtr);
};

#endif