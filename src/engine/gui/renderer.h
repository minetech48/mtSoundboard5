#include "uiElement.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

class Renderer {
	public:
		static void renderElement(UIElement element);
		static void renderText(std::string text, int x, int y);
		
		
		static void setRenderer(SDL_Renderer* renderer);
		
		static void addFont(std::string fontName, TTF_Font* font);
		static bool setFont(std::string fontName);
		static void clearFonts();
		
		
		static void start();
		static void finish();
		
		static void close();
};