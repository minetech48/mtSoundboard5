#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "vec2i.h"
#include "yaml-cpp/yaml.h"

struct UIElement {
	vec2i position, defPosition;
	vec2i position2, defSize;
	
	std::map<std::string, YAML::Node> metadata;
	std::map<std::string, UIElement> elements;
	
	//metadata examples
	//onClick
	//type
	//text + textStyle
	//ect...
	
	public:
		static void alignElement(UIElement* parentPtr, UIElement* elementPtr);
};

#endif