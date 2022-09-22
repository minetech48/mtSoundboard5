#include <unordered_map>

#include "guiData.h"
#include "gui.h"
#include "../util/stringReader.h"


std::unordered_map<std::string, UIElement*> GUIData::elementsMap;

std::unordered_map<std::string, SDL_Color> GUIData::colors;
std::unordered_map<std::string, TTF_Font*> GUIData::fonts;
std::unordered_map<std::string, std::vector<std::string>> GUIData::lists;
std::unordered_set<std::string> GUIData::loadedLists;

std::unordered_map<std::string, std::string> GUIData::strings;

std::unordered_map<std::string, std::function<std::string(std::string)>> GUIData::stringHandlers;

int GUIData::mouseX = 0, GUIData::mouseY = 0;
int GUIData::borderSize = 2;

//getter functions
UIElement* GUIData::getElement(std::string elementName) {
	if (elementsMap.contains(elementName))
		return elementsMap[elementName];
	
	StringReader reader(elementName);
	
	std::string key = reader.advanceTo('.');
	if (GUI::menus.contains(key)) {
		UIElement* element = &(GUI::menus[key]);
		std::string readString;
		
		while (reader.hasNext()) {
			readString = reader.advanceTo('.');
			
			if (element->elements.contains(readString))
				element = &(element->elements[readString]);
			else return NULL;
		}
		
		return element;
	}
	
	return NULL;
}

//color handling
void GUIData::addColor(std::string colorName, SDL_Color color) {
	colors.insert({colorName, color});
}
SDL_Color GUIData::getColor(std::string colorName) {
	return colors[colorName];
}

//font handling
void GUIData::addFont(std::string fontName, TTF_Font* font) {
	if (fonts.find(fontName) != fonts.end())
		TTF_CloseFont(fonts[fontName]);
	
	fonts.insert({fontName, font});
}
void GUIData::clearFonts() {
	for (auto const& font : fonts) {
		TTF_CloseFont(fonts[font.first]);
	}
	fonts.clear();
}
TTF_Font* GUIData::getFont(std::string fontName) {
	return fonts[fontName];
}


//string handling
void GUIData::setString(std::string key, std::string value) {
	strings[key] = value;
}

std::string GUIData::getString(std::string key) {
	if (strings.find(key) != strings.end())
		return strings[key];
	
	return key;
}

std::string GUIData::convertString(const char* str) {
	std::string toReturn;
	
	if (strings.contains(str)) {
		return getString(str);
	}
	
	StringReader reader(str);
	
	if (elementsMap.contains(str))
		return elementsMap[str]->getReturnValue();
	
	std::string key = reader.advanceTo('.');
	if (reader.hasNext()) {
		if (elementsMap.contains(key)) {
			return elementsMap[key]->getReturnValue();
		}else if (GUI::menus.contains(key)) {
			UIElement* element = &GUI::menus[key];
			std::string readString;
			
			while (reader.hasNext()) {
				readString = reader.advanceTo('.');
				
				if (element->elements.contains(readString))
					element = &(element->elements[readString]);
			}
			
			return element->getReturnValue();
		}else if (stringHandlers.contains(key))
			return stringHandlers[key](reader.advanceTo('\0'));
	}
	reader.setPosition(-1);
	
	char c;
	while (reader.hasNext()) {
		c = reader.next();
		
		if (c == '\\') {
			if (reader.hasNext()) {
				if (reader.next() == '$') {
					std::string recursor;
					std::string text, str;
					
					int scope = 0;
					do  {
						if (reader.current() == '$')
							scope++;
						else {
							scope--;
							if (scope == 0) break;
							reader.previous();
						}
						
						text = reader.advanceTo('\\');
						str = convertString(getString(text)/*, includeNull*/);
						
						if (/*includeNull || */text != str)
							recursor.append(str);//recursive special strings (\$specialString\)
						
					}while (reader.hasNext() && (reader.next() == '$' || scope > 0));
					reader.previous();
					
					//if (includeNull || !text.equals(recursor.toString()))
					toReturn+= getString(recursor);
				}else{
					toReturn+= reader.previous();
				}
			}
		}else{
			toReturn+= c;
		}
	}
	
	return toReturn;
}

void GUIData::addStringHandler(std::string key, std::function<std::string(std::string)> func) {
	stringHandlers.insert({key, func});
}

std::vector<std::string>* GUIData::addList(std::string key, std::vector<std::string> list) {
	if (lists.contains(key)) lists.erase(key);
	
	lists.insert({key, list});
	return &(lists[key]);
}
std::vector<std::string>* GUIData::getList(std::string key) {
	if (!lists.contains(key) && !loadedLists.contains(key))
		loadList(key);
	
	return &(lists[key]);
}