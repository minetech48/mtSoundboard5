#include "guiData.h"
#include "../util/stringReader.h"

static std::unordered_map<std::string, SDL_Color> colors;
static std::unordered_map<std::string, TTF_Font*> fonts;
static std::unordered_map<std::string, std::vector<std::string>> lists;

static YAML::Node strings;

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
void GUIData::setStrings(YAML::Node stringsList) {
	strings = stringsList;
}

std::string GUIData::getString(std::string key) {
	if (strings[key].IsDefined())
		return strings[key].as<std::string>();
	
	return key;
}

std::string GUIData::convertString(const char* str) {
	std::string toReturn;
	
	if (strings[str].IsDefined()) {
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