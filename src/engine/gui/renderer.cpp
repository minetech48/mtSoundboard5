#include "renderer.h"

static SDL_Renderer* sdl_renderer = NULL;

static TTF_Font* currentFont = NULL;
static TTF_Font* defaultFont = NULL;

static SDL_Color currentColor;

const int borderSize = 2;

void Renderer::renderElement(UIElement element) {
	// printf("Renderer: %s: %d:%d:%d:%d\n",
	// 	element.metadata["name"].as<std::string>().c_str(),
	// 	element.position.x,
	// 	element.position.y,
	// 	element.size.x,
	// 	element.size.y
	// );
	
	//rendering element/container
	if (!element.isContainer())
		drawElementRect(element);
	else if (element.isList())
		drawList(element);
	
	//drawing element text
	if (element.containsData("text")) {
		setColor("TextDefault");
		
		//font switching
		if (element.containsData("textFont")) {
			setFont(element.getDataString("textFont"));
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
	if (element.isContainer() && !element.isList()) {
		for (auto const& child : element.elements) {
			renderElement(child.second);
		}
	}
}
void Renderer::drawElementRect(UIElement element) {
	SDL_Rect rect = {
		element.position.x,
		element.position.y,
		element.position2.x - element.position.x,
		element.position2.y - element.position.y
	};
	
	//SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
	std::string borderColor;
	std::string backgroundColor;
	
	if (element.active) {
		borderColor = "ActiveBorder";
		backgroundColor = "Active";
	}
	else if (element.focused) {
		borderColor = "FocusedBorder";
		backgroundColor = "Focused";
	}
	
	setColor("Border");
	setColor(borderColor);
	
	SDL_RenderFillRect(sdl_renderer, &rect);
	
	rect.x+= borderSize;
	rect.y+= borderSize;
	rect.w-= borderSize*2;
	rect.h-= borderSize*2;
	
	setColor("Primary");
	setColor(backgroundColor);
	
	SDL_RenderFillRect(sdl_renderer, &rect);
}

void Renderer::drawList(UIElement element) {
	if (!element.isList()) return;
	
	drawElementRect(element);
	
	SDL_Rect* clipRect = new SDL_Rect{
		element.position.x,
		element.position.y + borderSize,
		element.position2.x - element.position.x,
		element.position2.y - element.position.y - borderSize*2
	};
	SDL_RenderSetClipRect(sdl_renderer, clipRect);
	delete clipRect;
	
	
	UIElement listElement = element.elements["ListElement"];
	int elementHeight = element.getDataInteger("elementHeight");
	
	listElement.position = element.position;
		listElement.position.x+= borderSize;
		listElement.position.y+= borderSize - (element.scroll % elementHeight);
		
	listElement.position2.x = element.position2.x;
	listElement.position2.y = listElement.position.y + elementHeight;
		listElement.position2.x-= borderSize;
		listElement.position2.y-= borderSize;
	
	
	//renderElement(listElement);
	std::vector<std::string>* list = GUIData::getList(element.getDataString("listName"));
	if (list == NULL) return;
	
	int startCount = std::max(element.scroll/elementHeight-1, 0);
	int endCount = std::min(
		(element.position2.y - element.position.y) / elementHeight + startCount + 2,
		(int) list->size()
	);
	
	for (int i = startCount; i < endCount; i++) {
		//listElement.metadata["text"] = (*list)[i];
		
		renderElement(listElement);
		
		listElement.position.y+= elementHeight;
		listElement.position2.y+= elementHeight;
	}
	
	
	SDL_RenderSetClipRect(sdl_renderer, NULL);
}


void Renderer::renderTextRaw(char* text, int x, int y, int centerWidth, int centerHeight) {
	//printf("text: %s\n", text.c_str());
	SDL_Surface* textSurface = TTF_RenderText_Solid(
		currentFont,
		text,
		currentColor);
	
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
	
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdl_renderer, textSurface);
	SDL_RenderCopy(sdl_renderer,
		textTexture,
		NULL, &drawRect);
	
	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);
}

//high(er) level draw functions
//draw string macro, adjusted for border
#define finishString() \
			drawStr[drawIndex] = '\0';\
			renderTextRaw(drawStr,\
			x			+borderSize+1,\
			printY		+borderSize-1,\
			centerWidth	- (centerWidth ? (borderSize+1)*2 : 0),\
			centerHeight- (centerHeight? (borderSize-1)*2 : 0));
			
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
	setColor("Background");//background color
	SDL_RenderClear(sdl_renderer);
}

void Renderer::finish() {
	SDL_RenderPresent(sdl_renderer);
}

void Renderer::close() {
	GUIData::clearFonts();
	setRenderer(NULL);
}