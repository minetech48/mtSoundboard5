class GUIData {
	public:
		static void addColor(std::string colorName, SDL_Color color);
		static SDL_Color getColor(std::string colorName);
		
		static void addFont(std::string fontName, TTF_Font* font);
		static void clearFonts();
		static TTF_Font* getFont(std::string fontName);
		
		
		static void setStrings(YAML::Node stringsList);
		static void addString(std::string key, std::string value);
		static std::string getString(std::string key);
		
		static std::string convertString(const char* str);
		static std::string convertString(std::string str) {return convertString(str.c_str());}
};