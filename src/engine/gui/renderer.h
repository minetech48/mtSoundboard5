#ifndef RENDERER_H
#define RENDERER_H

#include "uiElement.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include "guiData.h"

class Renderer {
	public:
		static void renderElement(UIElement element);
		static void renderMenu(UIElement menu);
		static void renderTextRaw(char* text, int x, int y, int centerWidth, int centerHeight);
		
		static void renderText(std::string text, int x, int y, int centerWidth, int centerHeight, bool right);
		static inline void renderText(std::string text, int x, int y, int centerWidth, int centerHeight) {renderText(text, x, y, centerWidth, centerHeight, false);}
		static inline void renderText(std::string text, int x, int y) {renderText(text, x, y, 0, 0);}
		static inline void renderTextRight(std::string text, int x, int y) {renderText(text, x, y, 0, 0, true);}
		
		static void drawElementRect(UIElement element);
		
		static void drawList(UIElement element);
		
		
		static void setRenderer(SDL_Renderer* renderer);
		
		static bool setFont(std::string fontName);
		static void updateDefaultFont();
		
		static bool setColor(std::string colorName);
		static void clearColors();
		
		static void applyBorder(SDL_Rect* rect);
		static void applyBorder(vec2i* start, vec2i* end);
		
		
		static void start();
		static void finish();
		
		static void close();
};

#endif