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