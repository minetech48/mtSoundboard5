#include <yaml-cpp/yaml.h>

#include "uiElement.h"

class UIParser {
	public:
		static void parseUIElement(UIElement* elementPtr, YAML::Node root);
		static UIElement parseUIElement(YAML::Node root);
};