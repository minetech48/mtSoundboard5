#include <stdio.h>
#include <thread>
#include <SDL.h>
#include <SDL_image.h> 

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

void windowLoop();
void tempDrawFunction();
bool initSDL();
bool loadMedia();
void endSDL();
