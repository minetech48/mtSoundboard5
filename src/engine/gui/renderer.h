#include "uiElement.h"
#include <SDL2/SDL.h>

class Renderer {
	public:
		static void setRenderer(SDL_Renderer* renderer);
		
		static void renderElement(UIElement element);
		
		static void start();
		static void finish();
};