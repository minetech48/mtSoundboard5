#include "gui.h"

static int windowWidth, windowHeight;

SDL_Window* window = NULL;

SDL_Surface* windowSurface = NULL;
SDL_Surface* testImage = NULL;
SDL_Surface* testImage2 = NULL;

UIElement rootElement;

void GUI::initialize() {
	printf("--System GUI Initializing\n");
	
	//ititializing variables
	windowWidth = 800;
	windowHeight = 800;
	
	initSDL();
	//loadMedia();
	//tempDrawFunction();
	
	rootElement.position2 = {windowWidth, windowHeight};
	
	std::thread(windowLoop).detach();
	
	printf("--System GUI Initialized.\n");
}

void GUI::update() {
	
	//Update the surface
	// SDL_UpdateWindowSurface(window);
}

void GUI::handleEvent(EngineEvent event) {
	switch (hash(event.event)) {
		case hash("GUIShow"):
			loadGUI(event.arg1);
			break;
		case (hash("GUISetTheme")):
			setTheme(event.arg1);
			break;
		
		case hash("Shutdown"):
			endSDL();
			break;
	}
}

//events
//parsing YAML menu file
void loadGUI(std::string filePath) {
	YAML::Node ymlRoot = YAML::LoadFile("resources/" + filePath);
	
	// std::string str = yml["elements"]["HelloButton"]["text"].as<std::string>();
	// printf("%s \n", str.c_str());
	
	UIElement menu = UIParser::parseUIElement(ymlRoot);
	menu.name = filePath.substr(filePath.find_last_of('/')+1, filePath.find_last_of(".")-4);
	//printf("menuname: %s\n", menu.name.c_str());
	
	UIElement::alignElement(&rootElement, &menu);
	
	Renderer::start();
	Renderer::renderElement(menu);
	Renderer::finish();
	
	//printf("%s\n", menu.elements["OtherButton"].text.c_str());
}

//parsing theme file
void setTheme(std::string filePath) {
	GUIData::clearFonts();
	Renderer::clearColors();
	
	YAML::Node ymlRoot = YAML::LoadFile("resources/" + filePath);
	UIParser::loadTheme(ymlRoot);
}


//window (input) handling
void windowLoop() {
	SDL_Event event;
	while (EngineCore::running) {
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				EngineCore::running = false;
				break;
			}
		}
	}
}

void tempDrawFunction() {
	//SDL_BlitSurface(testImage, NULL, windowSurface, NULL);
	SDL_Rect stretchRect;
	stretchRect.x = 0;
	stretchRect.y = 0;
	stretchRect.w = windowWidth/2;
	stretchRect.h = windowHeight;
	SDL_BlitScaled(testImage, NULL, windowSurface, &stretchRect);

	stretchRect.x = windowWidth / 2;
	SDL_BlitScaled(testImage2, NULL, windowSurface, &stretchRect);
}

bool initSDL() {
	//Initializing SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	if(TTF_Init() == -1) {
		printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
		return false;
	}

	//Initializing SDL IMG
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	//Creating window
	window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	//windowSurface = SDL_GetWindowSurface(window);
	
    SDL_Renderer* gRenderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
	if (gRenderer == NULL) {
		printf("Failed to create renderer: %s\n", SDL_GetError());
		return false;
	}
	Renderer::setRenderer(gRenderer);
	
	//temp
	// TTF_Font* font = TTF_OpenFont("resources/FreeMono.ttf", 22);
	// if (font == NULL) {
	// 	printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
	// 	return false;
	// }
	// printf("Font loading success!\n");
	
	// Renderer::addFont("primary", font);

	return true;
}

bool loadMedia() {
	testImage = SDL_LoadBMP("resources/Hello World.bmp");
	if (testImage == NULL) {
		printf("Unable to load image! SDL Error: %s\n", SDL_GetError());
		return false;
	}
		
	testImage2 = IMG_Load("resources/Hello World.png");
	if (testImage2 == NULL) {
		printf("Unable to load image! SDL Error: %s\n", IMG_GetError());
		return false;
	}
	


	//converting/optimizing image
	SDL_Surface* tempImg = testImage;
	testImage = SDL_ConvertSurface(tempImg, windowSurface->format, 0);
	SDL_FreeSurface(tempImg);
	
	tempImg = testImage2;
	testImage2 = SDL_ConvertSurface(tempImg, windowSurface->format, 0);
	SDL_FreeSurface(tempImg);

	return true;
}

void endSDL() {
	//Deallocate surface
	SDL_FreeSurface(testImage);
	testImage = NULL;

	//Destroy window
	SDL_DestroyWindow(window);
	window = NULL;
	
	Renderer::close();
	
	//Quit SDL subsystems
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}