#include "uiElement.h"

void UIElement::alignElement(UIElement* parentPtr, UIElement* elementPtr) {
	
	// elementPtr->position.x = elementPtr->defPosition.x;
	// elementPtr->position.y = elementPtr->defPosition.y;
	elementPtr->position = elementPtr->defPosition;
	elementPtr->position2 = elementPtr->defSize;//+elementPtr->defPosition;
	
	//recursion
	if (!elementPtr->elements.empty()) {
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