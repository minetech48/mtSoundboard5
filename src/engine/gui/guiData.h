#pragma once

#include <SDL_ttf.h>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>

struct UIElement;

class GUIData {
	public:
		static int mouseX, mouseY;
		static int borderSize;
		
		static std::unordered_map<std::string, UIElement*> elementsMap;
		
		static std::unordered_map<std::string, SDL_Color> colors;
		static std::unordered_map<std::string, TTF_Font*> fonts;
		static std::unordered_map<std::string, std::vector<std::string>> lists;
		static std::unordered_set<std::string> loadedLists;
		
		static std::unordered_map<std::string, std::string> strings;
		
		static std::unordered_map<std::string, std::function<std::string(std::string)>> stringHandlers;
		
	public:
		static UIElement* getElement(std::string elementName);
		
		static void addColor(std::string colorName, SDL_Color color);
		static SDL_Color getColor(std::string colorName);
		
		static void addFont(std::string fontName, TTF_Font* font);
		static void clearFonts();
		static TTF_Font* getFont(std::string fontName);
		
		
		static void setString(std::string key, std::string value);
		static std::string getString(std::string key);
		
		static std::vector<std::string>* addList(std::string key, std::vector<std::string> list);
		static std::vector<std::string>* getList(std::string key);
		
		static std::string convertString(const char* str);
		static std::string convertString(std::string str) {return convertString(str.c_str());}
		
		static void addStringHandler(std::string, std::function<std::string(std::string)>);
};