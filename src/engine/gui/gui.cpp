#include "gui.h"

static int windowWidth, windowHeight;

SDL_Window* window = NULL;

SDL_Surface* windowSurface = NULL;
SDL_Surface* testImage = NULL;
SDL_Surface* testImage2 = NULL;

UIElement rootElement;
UIElement* focusedElement;

UIElement* clickedElement;

std::map<std::string, UIElement> menus;
	
static int mouseX, mouseY;

void GUI::initialize() {
	printf("--System GUI Initializing\n");
	
	//ititializing variables
	windowWidth = 800;
	windowHeight = 800;
	
	initSDL();
	//loadMedia();
	//tempDrawFunction();
	
	rootElement.position2 = {windowWidth, windowHeight};
	rootElement.name = "root";
	
	std::thread(windowLoop).detach();
	
	printf("--System GUI Initialized.\n");
}

void GUI::update() {
	
	//Update the surface
	// SDL_UpdateWindowSurface(window);
	
	SDL_GetMouseState(&mouseX, &mouseY);
	
	//printf("Mouse position: (%d, %d)\n", mouseX, mouseY);
	
	for (auto const& menu : menus) {
		UIElement* hovered = getHoveredElement(&menus[menu.first]);
		if (hovered != focusedElement) {
			if (focusedElement != NULL)
				focusedElement->focused = false;
			
			focusedElement = hovered;
			
			if (focusedElement != NULL)
				focusedElement->focused = true;
			break;
		}
	}
	
	Renderer::start();
	
	for (auto const& menu : menus) {
		Renderer::renderElement(menu.second);
	}
	
	Renderer::finish();
}

UIElement* getHoveredElement(UIElement* element) {
	UIElement* toReturn = element;
	if (!isInBounds(*element, mouseX, mouseY)) return NULL;
	
	
	if (element->isContainer()) {
		for (auto const& child : element->elements) {
			UIElement* hovered = getHoveredElement(&element->elements[child.first]);
			
			if (hovered != NULL ) {
				toReturn = hovered;
				break;
			}
		}
	}
	
	if (toReturn->isFocusable()) {
		return toReturn;
	}
	return NULL;
}
bool isInBounds(UIElement element, int x, int y) {
	return	x > element.position.x && x < element.position2.x &&
			y > element.position.y && y < element.position2.y;
}

void GUI::handleEvent(EngineEvent event) {
	switch (hash(event.event)) {
		case hash("GUIShow"):
			loadGUI(event.arg1);
			break;
		case (hash("GUISetTheme")):
			setTheme(event.arg1);
			break;
		case (hash("GUIReset")):
			resetGUI();
			break;
		
		case hash("Shutdown"):
			endSDL();
			break;
		
	}
}

//events
//parsing YAML menu file
void loadGUI(std::string filePath) {
	YAML::Node ymlRoot = YAML::LoadFile(UIParser::findFile(filePath, ".yml"));
	
	// std::string str = yml["elements"]["HelloButton"]["text"].as<std::string>();
	// printf("%s \n", str.c_str());
	
	UIElement menu = UIParser::parseUIElement(ymlRoot);
	menu.name = filePath.substr(filePath.find_last_of('/')+1, filePath.find_last_of(".")-4);
	//printf("menuname: %s\n", menu.name.c_str());
	
	UIElement::alignElement(&rootElement, &menu);
	
	menus[menu.name] = menu;
	
	//printf("%s\n", menu.elements["OtherButton"].text.c_str());
}

//parsing theme file
void setTheme(std::string filePath) {
	GUIData::clearFonts();
	Renderer::clearColors();
	
	YAML::Node ymlRoot = YAML::LoadFile(UIParser::findFile(filePath, ".yml"));
	UIParser::loadTheme(ymlRoot);
}

void resetGUI() {
	for (auto const& child : menus) {
		loadGUI(child.second.name);
	}
}


void clickElement(UIElement* element) {
	if (element == NULL || !element->isButton()) return;
	
	if (element->isSwitch() && element->active) {
		EngineCore::broadcast(element->getDataString("onClick")+"R");
		element->active = false;
	}else{
		EngineCore::broadcast(element->getDataString("onClick"));
		element->active = true;
	}
	
	clickedElement = element;
}
void unclickElement() {
	if (clickedElement == NULL || !clickedElement->isButton()) return;
	
	if (!clickedElement->isSwitch())
		clickedElement->active = false;
	
	clickedElement = NULL;
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
				
			case SDL_MOUSEBUTTONDOWN:
				clickElement(focusedElement);
				break;
				
			case SDL_MOUSEBUTTONUP:
				unclickElement();
				break;
			}
		}
	}
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