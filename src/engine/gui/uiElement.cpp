#include "uiElement.h"

void UIElement::alignElement(UIElement* parentPtr, UIElement* elementPtr) {
	//make containers fill parent if dimensions undefined
	if (elementPtr->defSize.x == 0) {
		
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
	if (elementPtr->defSize.x < 0)
		elementPtr->position2.x = parentSize.x + elementPtr->defSize.x+1 - abs(elementPtr->position.x);
	if (elementPtr->defSize.y < 0)
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
	elementPtr->position+= parentPtr->position;
	elementPtr->position2+= elementPtr->position;
	
	//recursion
	if (elementPtr->isContainer()) {
		for (auto const& child : elementPtr->elements) {
			alignElement(elementPtr, &(elementPtr->elements[child.first]));
		}
	}
	
	// printf("Element Alignment: %s: %d:%d:%d:%d\n",
	// 	elementPtr->metadata["name"].as<std::string>().c_str(),
	// 	elementPtr->position.x,
	// 	elementPtr->position.y,
	// 	elementPtr->position2.x,
	// 	elementPtr->position2.y
	// );
}