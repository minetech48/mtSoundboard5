#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include <thread>
#include <SDL.h>
#include <SDL_image.h> 
#include <fstream>

#include "../math.cpp"
#include "uiParser.cpp"
#include "uiElement.cpp"
#include "../engineSystem.h"
#include "renderer.cpp"

class GUI : public EngineSystem {
	public:
		void initialize();
		void handleEvent(EngineEvent event);
		void update();
};

void loadGUI(std::string filePath);
void setTheme(std::string filePath);
void loadList(std::string listName);
void resetGUI();

UIElement* getHoveredElement(UIElement* element);
bool isInBounds(UIElement element, int x, int y);

void windowLoop();
bool initSDL();
bool loadMedia();
void endSDL();

#endif