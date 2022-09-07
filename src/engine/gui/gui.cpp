#include "gui.h"

const int windowWidth = 1000, windowHeight = 850;

SDL_Window* window = NULL;

SDL_Surface* windowSurface = NULL;
SDL_Surface* testImage = NULL;
SDL_Surface* testImage2 = NULL;

UIElement rootElement;
UIElement* focusedElement;

UIElement* clickedElement;

std::map<std::string, UIElement> menus;

std::vector<std::function<void(SDL_Event)>> GUI::SDLEventHandlers;

//static int mouseX, mouseY;

void GUI::initialize() {
	
	//ititializing variables
	
	initSDL();
	//loadMedia();
	//tempDrawFunction();
	
	rootElement.position2 = {windowWidth, windowHeight};
	rootElement.name = "root";
	
	//std::thread(windowLoop).detach();
	
	loadList("lists/testList");
}

void GUI::update() {
	windowLoop();
	
	//Update the surface
	// SDL_UpdateWindowSurface(window);
	
	SDL_GetMouseState(&GUIData::mouseX, &GUIData::mouseY);
	
	//printf("Mouse position: (%d, %d)\n", mouseX, mouseY);
	
	if (clickedElement == NULL) {
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
	}
	
	Renderer::start();
	
	for (auto const& menu : menus) {
		Renderer::renderElement(menu.second);
	}
	
	Renderer::finish();
}

UIElement* getHoveredElement(UIElement* element) {
	UIElement* toReturn = element;
	if (!isInBounds(*element, GUIData::mouseX, GUIData::mouseY)) return NULL;
	
	
	if (element->isContainer()) {
		for (auto const& child : element->elements) {
			UIElement* hovered = getHoveredElement(&element->elements[child.first]);
			
			if (hovered != NULL) {
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
	YAML::Node ymlRoot = YAML::LoadFile(FileIO::findFile(filePath, ".yml"));
	
	// std::string str = yml["elements"]["HelloButton"]["text"].as<std::string>();
	// printf("%s \n", str.c_str());
	
	UIElement menu = UIParser::parseUIElement(ymlRoot);
	menu.name = FileIO::getFileName(filePath);
	//printf("menuname: %s\n", menu.name.c_str());
	
	UIElement::alignElement(&rootElement, &menu);
	
	menus[menu.name] = menu;
	
	//printf("%s\n", menu.elements["OtherButton"].text.c_str());
}

//parsing theme file
void setTheme(std::string filePath) {
	GUIData::clearFonts();
	Renderer::clearColors();
	
	YAML::Node ymlRoot = YAML::LoadFile(FileIO::findFile(filePath, ".yml"));
	UIParser::loadTheme(ymlRoot);
}

void loadList(std::string filePath) {
	std::ifstream file(FileIO::findFile(filePath, ".txt"));
	std::string listName = FileIO::getFileName(filePath);
	
	printf("Loading list: \"%s\" from \"%s\"\n", 
		listName.c_str(), 
		FileIO::findFile(filePath, ".txt").c_str());
	
	if (file.is_open()) {
		std::string line;
		GUIData::addList(listName, {"test1"});
		
		while (std::getline(file, line)) {
			GUIData::getList(listName)->push_back(line);
		}
		
		
		file.close();
	}
}

void resetGUI() {
	for (auto const& child : menus) {
		loadGUI(child.second.name);
	}
	EngineCore::broadcast("GUISetTheme", "gui/theming/DefaultTheme");
}

//window (input) handling
void windowLoop() {
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			EngineCore::running = false;
			break;
			
		case SDL_MOUSEBUTTONDOWN:
			if (focusedElement == NULL) break;
			focusedElement->click();
			clickedElement = focusedElement;
			break;
			
		case SDL_MOUSEBUTTONUP:
			if (clickedElement == NULL) break;
			clickedElement->unclick();
			clickedElement = NULL;
			break;
		
		case SDL_MOUSEWHEEL:
			if (focusedElement == NULL) break;
			
			focusedElement->scroll+= -event.wheel.y*11;
			
			if (focusedElement->isList() && focusedElement->scroll < 0)
				focusedElement->scroll = 0;
			
			break;
		}
		
		for (auto funct : GUI::SDLEventHandlers) {
			funct(event);
		}
	}
}

void GUI::addSDLEventHandler(std::function<void(SDL_Event)> function) {
	SDLEventHandlers.push_back(function);
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