#include "uiElement.h"
#include "engine/logger.h"

#include <regex>
#include <chrono>

#include "../engineCore.h"

void UIElement::alignElement(UIElement* parentPtr, UIElement* elementPtr) {
	//make containers fill parent if dimensions undefined
	bool fill = false;
	if (elementPtr->defSize.x == 0 && elementPtr->defSize.y == 0
			&& elementPtr->isContainer()) {
		fill = true;
		
		if (parentPtr->isGridContainer())
			elementPtr->defSize = {1, 1};
		else if (elementPtr->isContainer())
			elementPtr->defSize = parentPtr->position2-parentPtr->position;
	}
	
	vec2i parentSize;
	if (parentPtr->isGridContainer())
		parentSize = parentPtr->getGridSize();
	else
		parentSize = parentPtr->getSize();
	
	//initializing positional data
	elementPtr->position = elementPtr->defPosition;
	elementPtr->position2 = elementPtr->defSize; //position 2 is equal to size until end of alignment
	
	
	//handling 'stretching'
	if (elementPtr->defSize.x <= 0)
		elementPtr->position2.x = parentSize.x + elementPtr->defSize.x+1 - abs(elementPtr->position.x);
	if (elementPtr->defSize.y <= 0)
		elementPtr->position2.y = parentSize.y + elementPtr->defSize.y+1 - abs(elementPtr->position.y);
	
	//handling right, bottom, and center alignment
	if (elementPtr->metadata.find("centerX") != elementPtr->metadata.end())
		elementPtr->position.x+= parentSize.x/2;
	else if (elementPtr->defPosition.x < 0)
		elementPtr->position.x+= parentSize.x - elementPtr->position2.x+1;
		
	if (elementPtr->metadata.find("centerY") != elementPtr->metadata.end())
		elementPtr->position.y+= parentSize.y/2;
	else if (elementPtr->defPosition.y < 0)
		elementPtr->position.y+= parentSize.y - elementPtr->position2.y+1;
		
	//handling grid container alignment
	if (parentPtr->isGridContainer()) {
		vec2i gridTileSize = parentPtr->getGridTileSize();
		
		elementPtr->position*= gridTileSize;
		elementPtr->position2*= gridTileSize;
	}
	
	//finalizing data
	// if (elementPtr->position2.x < 0) {
	// 	elementPtr->position2.x*= -1;
	// 	elementPtr->position.x-= elementPtr->position2.x;
	// }
	// if (elementPtr->position2.y < 0) {
	// 	elementPtr->position2.y*= -1;
	// 	elementPtr->position.y-= elementPtr->position2.y;
	// }
	
	elementPtr->position+= parentPtr->position;
	elementPtr->position2+= elementPtr->position;
	
	//recursion
	if (elementPtr->isContainer()) {
		for (auto const& child : elementPtr->elements) {
			alignElement(elementPtr, &(elementPtr->elements[child.first]));
		}
	}
	
	// logf("Element Alignment: %s: %d:%d:%d:%d\n",
	// 	elementPtr->metadata["name"].as<std::string>().c_str(),
	// 	elementPtr->position.x,
	// 	elementPtr->position.y,
	// 	elementPtr->position2.x,
	// 	elementPtr->position2.y
	// );
	if (fill) {
		elementPtr->defSize.x = 0;
		elementPtr->defSize.y = 0;
	}
}

void UIElement::click(int clicks) {
	//testing for double click
	bool doubleClick = false;
	std::string action;
	if (clicks > 1 && containsData("onDoubleClick")) {
		doubleClick = true;
		action = getDataString("onDoubleClick");
	}else{
		action = getDataString("onClick");
	}
	
	action = GUIData::convertString(action);
	
	if (isList()) {
		listActive = getListSelected();
		
		action = std::regex_replace(action, std::regex("\\|ELEMENT\\|"),
			getReturnValue());
		
	}
	
	if ((isToggleButton() || isSwitch()) && active) {
		if (isToggleButton())
			EngineCore::broadcast(action+"R");
		else if (isSwitch()) {
			*(bool*) bindPtr = false;
			
			if (isButton()) {
				EngineCore::broadcast(action);
			}
		}
		
		active = false;
	}else{
		if (isSwitch())
			*(bool*) bindPtr = true;
		
		if (isButton())
			EngineCore::broadcast(action);
		
		active = true;
	}
}
void UIElement::unclick() {
	if (!isButton()) return;
	
	if (!isToggleButton() && !isSwitch())
		active = false;
		
	if (isList() && getDataString("listMode") == "single")
		listActive = -1;
}

std::string UIElement::getReturnValue() {
	if (isList()) {
		std::vector<std::string>* list = GUIData::getList(getDataString("listName"));
		
		if (listActive >= 0 && list->size() > listActive)
			return list->operator[](listActive);
	}
	
	return "";
}