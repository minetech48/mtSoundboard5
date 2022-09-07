#include <unordered_map>

#include "guiData.h"
#include "../util/stringReader.h"

std::unordered_map<std::string, SDL_Color> GUIData::colors;
std::unordered_map<std::string, TTF_Font*> GUIData::fonts;
std::unordered_map<std::string, std::vector<std::string>> GUIData::lists;

std::unordered_map<std::string, std::string> GUIData::strings;


int GUIData::mouseX = 0, GUIData::mouseY = 0;
int GUIData::borderSize = 2;

//getter functions

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
	
	if (strings.find(str) != strings.end()) {
		return getString(str);
	}
	
	StringReader reader(str);
	
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

void GUIData::addList(std::string key, std::vector<std::string> list) {
	if (lists.contains(key)) lists.erase(key);
	
	lists.insert({key, list});
}
std::vector<std::string>* GUIData::getList(std::string key) {
	return &lists[key];
}