#include "renderer.h"

static SDL_Renderer* sdl_renderer = NULL;

static std::unordered_map<std::string, TTF_Font*> fonts;
static TTF_Font* currentFont = NULL;

static std::unordered_map<std::string, SDL_Color> colors;
static SDL_Color currentColor;

void Renderer::renderElement(UIElement element) {
	// printf("Renderer: %s: %d:%d:%d:%d\n",
	// 	element.metadata["name"].as<std::string>().c_str(),
	// 	element.position.x,
	// 	element.position.y,
	// 	element.size.x,
	// 	element.size.y
	// );
	
	//rendering element/container
	SDL_Rect rect = {
		element.position.x,
		element.position.y,
		element.position2.x - element.position.x,
		element.position2.y - element.position.y
	};
	
	SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
	
	SDL_RenderDrawRect(sdl_renderer, &rect);
	
	if (element.containsData("text"))
		renderText(element.getDataString("text"), element.position.x, element.position.y);
	//renderText(element.name, element.position.x, element.position.y);
	
	//rendering children (recursive)
	if (!element.elements.empty()) {
		for (auto const& child : element.elements) {
			renderElement(child.second);
		}
	}
}

void Renderer::renderText(std::string text, int x, int y) {
	//printf("text: %s\n", text.c_str());
	SDL_Surface* textSurface = TTF_RenderText_Solid(
		currentFont,
		text.c_str(),
		{0x00, 0xff, 0x00});
	
	if(textSurface == NULL) {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return;
	}
	
	SDL_Rect drawRect = textSurface->clip_rect;
	drawRect.x+= x;
	drawRect.y+= y;
	
	SDL_RenderCopy(sdl_renderer,
		SDL_CreateTextureFromSurface(sdl_renderer, textSurface),
		NULL, &drawRect);
	
	
}


//renderer
void Renderer::setRenderer(SDL_Renderer* renderer) {
	if (sdl_renderer != NULL)
		SDL_DestroyRenderer(sdl_renderer);
	
	sdl_renderer = renderer;
}

//fonts
void Renderer::addFont(std::string fontName, TTF_Font* font) {
	if (fonts.find(fontName) != fonts.end())
		TTF_CloseFont(fonts[fontName]);
	
	fonts.insert({fontName, font});
}
bool Renderer::setFont(std::string fontName) {
	if (fonts.find(fontName) == fonts.end())
		return false;
	
	currentFont = fonts[fontName];
	return true;
}
void Renderer::clearFonts() {
	for (auto const& font : fonts) {
		TTF_CloseFont(fonts[font.first]);
	}
	fonts.clear();
}

//colors
void Renderer::addColor(std::string colorName, SDL_Color color) {
	colors.insert({colorName, color});
}
bool Renderer::setColor(std::string colorName) {
	if (colors.find(colorName) == colors.end())
		return false;
	
	currentColor = colors[colorName];
	
	if (sdl_renderer != NULL)
		SDL_SetRenderDrawColor(sdl_renderer,
			currentColor.r,
			currentColor.g,
			currentColor.b,
			currentColor.a);
	
	return true;
}
void Renderer::clearColors() {
	colors.clear();
}

//rendering actions
void Renderer::start() {
	SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x00, 0xff, 0xff);//background color
	SDL_RenderClear(sdl_renderer);
}

void Renderer::finish() {
	SDL_RenderPresent(sdl_renderer);
}

void Renderer::close() {
	clearFonts();
	setRenderer(NULL);
}