#pragma once

#include <SDL_ttf.h>
#include <yaml-cpp/yaml.h>

class GUIData {
	public:
		static int mouseX, mouseY;
		static const int borderSize = 2;
		
	public:
		static void addColor(std::string colorName, SDL_Color color);
		static SDL_Color getColor(std::string colorName);
		
		static void addFont(std::string fontName, TTF_Font* font);
		static void clearFonts();
		static TTF_Font* getFont(std::string fontName);
		
		
		static void setString(std::string key, std::string value);
		static std::string getString(std::string key);
		
		static void addList(std::string key, std::vector<std::string> list);
		static std::vector<std::string>* getList(std::string key);
		
		static std::string convertString(const char* str);
		static std::string convertString(std::string str) {return convertString(str.c_str());}
};