#include "renderer.h"

static SDL_Renderer* sdl_renderer = NULL;

static TTF_Font* currentFont = NULL;
static TTF_Font* defaultFont = NULL;

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
	
	if (!element.isContainer())
		SDL_RenderDrawRect(sdl_renderer, &rect);
	
	//drawing element text
	if (element.containsData("text")) {
		
		//font switching
		if (element.containsData("textFont")) {
			setFont(element.getDataString("textFont"));
			printf("test1: %s\n", element.getDataString("textFont").c_str());
		}else{
			currentFont = defaultFont;
		}
		
		//text centering for buttons
		if (element.isButton()) {
			renderText(element.getDataString("text"),
				 element.position.x, element.position.y,
				 element.position2.x - element.position.x,
				 element.position2.y - element.position.y);
		}else{
			renderText(element.getDataString("text"), element.position.x, element.position.y);
		}
	}
	
	//rendering children (recursive)
	if (!element.elements.empty()) {
		for (auto const& child : element.elements) {
			renderElement(child.second);
		}
	}
}

void Renderer::renderTextRaw(char* text, int x, int y, int centerWidth, int centerHeight) {
	//printf("text: %s\n", text.c_str());
	SDL_Surface* textSurface = TTF_RenderText_Solid(
		currentFont,
		text,
		{0x00, 0xff, 0x00});
	
	if(textSurface == NULL) {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		return;
	}
	
	SDL_Rect drawRect = textSurface->clip_rect;
	drawRect.x+= x;
	drawRect.y+= y;
	
	if (centerWidth)
		drawRect.x+= (centerWidth-drawRect.w)/2;
	if (centerHeight)
		drawRect.y+= (centerHeight-drawRect.h)/2;
	
	SDL_RenderCopy(sdl_renderer,
		SDL_CreateTextureFromSurface(sdl_renderer, textSurface),
		NULL, &drawRect);
}

//high(er) level draw functions
#define finishString() \
			drawStr[drawIndex] = '\0';\
			renderTextRaw(drawStr, x, printY, centerWidth, centerHeight);
			
void Renderer::renderText(std::string text, int x, int y, int centerWidth, int centerHeight) {
	text = GUIData::convertString(text);
	
	char drawStr[text.length()];
	const char* textPtr = text.c_str();
	int drawIndex = 0, printY = y;
	
	for (int i = 0; i < text.length(); i++) {
		if (textPtr[i] == '\n' || (textPtr[i] == '\\' && textPtr[i+1] == 'n')) {
			finishString();
			drawIndex = 0;
			
			int textHeight = 0;
			TTF_SizeText(currentFont, drawStr, NULL, &textHeight);
			printY+= textHeight;
			i++;
			continue;
		}
		
		drawStr[drawIndex++] = textPtr[i];
	}
	finishString();
}
#undef finishString



//setters
//renderer
void Renderer::setRenderer(SDL_Renderer* renderer) {
	if (sdl_renderer != NULL)
		SDL_DestroyRenderer(sdl_renderer);
	
	sdl_renderer = renderer;
}

//fonts
bool Renderer::setFont(std::string fontName) {
	if (fonts.find(fontName) == fonts.end())
		return false;
	
	currentFont = fonts[fontName];
	return true;
}
void Renderer::updateDefaultFont() {
	defaultFont = GUIData::getFont("Default");
	currentFont = defaultFont;
}

//colors
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
	GUIData::clearFonts();
	setRenderer(NULL);
}