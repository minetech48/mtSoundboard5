#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "guiData.h"
#include "vec2i.h"
#include <yaml-cpp/yaml.h>
#include <unordered_map>

struct UIElement {
	vec2i position, defPosition;
	vec2i position2, defSize;
	std::string name;
	
	std::unordered_map<std::string, YAML::Node> metadata;
	std::unordered_map<std::string, UIElement> elements;
	
	bool focused = false;
	bool active = false;
	
	int scroll = 0;
	int listActive = -1;
	
	//metadata examples
	//onClick
	//type
	//text + textStyle
	//ect...
	
	
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
	
	bool containsData(std::string key) {return metadata.find(key) != metadata.end();}
	std::string getDataString(std::string key) {return containsData(key) ? 
		metadata[key].as<std::string>() : "";}
	int getDataInteger(std::string key) {return containsData(key) ? 
		metadata[key].as<int>() : 1;}
	
	//element type checks
	bool isContainer() {return !elements.empty();}
	bool isGridContainer() {return containsData("gridSize");}
	
	int getListElementHeight() {return getDataInteger("elementHeight");}
	int getListElementWidth() {return (position2.x-position.x) / getDataInteger("listWidth");}
	int getListSelected() {
		return focused ?
			(((GUIData::mouseY-GUIData::borderSize-position.y + scroll) 
			/ getListElementHeight()) * getDataInteger("listWidth")) +
			((GUIData::mouseX-GUIData::borderSize-position.x) / getListElementWidth()): -1;
	}
	
	bool isFocusable() {return !isContainer() || isList();}
	
	bool isButton() {return containsData("onClick") || containsData("onDoubleClick");}
	bool isSwitch() {return containsData("toggle");}
	bool isList() {return containsData("listName");}
	
	
	static void alignElement(UIElement* parentPtr, UIElement* elementPtr);
	
	void click();
	void unclick();
};

#endif