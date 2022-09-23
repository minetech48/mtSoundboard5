#include "renderer.h"
#include "engine/logger.h"

#include <regex>

static SDL_Renderer* sdl_renderer = NULL;

static TTF_Font* currentFont = NULL;
static TTF_Font* defaultFont = NULL;

static SDL_Color currentColor;


void Renderer::renderMenu(UIElement menu) {
	
	if (menu.containsData("drawBackground")) {
		int oldBorder = GUIData::borderSize;
		GUIData::borderSize*= -3;
		
		SDL_Rect rect = {
			menu.position.x,
			menu.position.y,
			menu.position2.x - menu.position.x,
			menu.position2.y - menu.position.y
		};
		applyBorder(&rect);
		
		setColor("Border");
		SDL_RenderFillRect(sdl_renderer, &rect);
		
		GUIData::borderSize*= -1;
		applyBorder(&rect);
		GUIData::borderSize = oldBorder;
		
		setColor("Background");
		SDL_RenderFillRect(sdl_renderer, &rect);
	}
	
	renderElement(menu);
}
void Renderer::renderElement(UIElement element) {
	// logf("Renderer: %s: %d:%d:%d:%d\n",
	// 	element.metadata["name"].as<std::string>().c_str(),
	// 	element.position.x,
	// 	element.position.y,
	// 	element.size.x,
	// 	element.size.y
	// );
	
	//rendering element/container
	if (element.isList()) {
		drawList(element);
		return;
	}
	if (!element.isContainer())
		drawElementRect(element);
	
	//drawing element text
	if (element.containsData("text") || element.containsData("text2")) {
		setColor("TextDefault");
		
		//font switching
		if (element.containsData("textFont")) {
			setFont(element.getDataString("textFont"));
		}else{
			currentFont = defaultFont;
		}
		
		//text centering for buttons
		if (element.isButton()) {
			if (element.containsData("text"))
				renderText(element.getDataString("text"),
					element.position.x, element.position.y,
					element.position2.x - element.position.x,
					element.position2.y - element.position.y);
		}else{
			if (element.getDataString("text") != "")
				renderText(element.getDataString("text"), element.position.x, element.position.y);
			if (element.getDataString("text2") != "")
				renderTextRight(element.getDataString("text2"), element.position2.x, element.position.y);
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
	
	if (element.active && !element.isList()) {
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
	
	applyBorder(&rect);
	
	setColor("Primary");
	setColor(backgroundColor);
	
	SDL_RenderFillRect(sdl_renderer, &rect);
}

void Renderer::drawList(UIElement element) {
	if (!element.isList()) return;
	
	drawElementRect(element);
	
	SDL_Rect* clipRect = new SDL_Rect{
		element.position.x,
		element.position.y + GUIData::borderSize,
		element.position2.x - element.position.x,
		element.position2.y - element.position.y - GUIData::borderSize*2
	};
	SDL_RenderSetClipRect(sdl_renderer, clipRect);
	delete clipRect;
	
	
	UIElement listElement;// = element.elements["ListElement"];
	int listWidth = element.getDataInteger("listWidth");
	int elementHeight = element.getListElementHeight();
	int elementWidth = element.getListElementWidth() - GUIData::borderSize*2/listWidth;
	
	//listElement.position = element.position;
	int startX = element.position.x + GUIData::borderSize;
	int startY = element.position.y + GUIData::borderSize - element.scroll;
		
	// listElement.position2.x = listElement.position.x + elementWidth;
	// listElement.position2.y = listElement.position.y + elementHeight;
	// 	//listElement.position2.x-= GUIData::borderSize;
	// 	listElement.position2.y-= GUIData::borderSize;
	
	
	//renderElement(listElement);
	std::vector<std::string>* list = GUIData::getList(element.getDataString("listName"));
	if (list == NULL) return;
	
	int startCount = std::max(element.scroll/(elementHeight/listWidth)-listWidth, 0);
	int endCount = std::min(
		(element.position2.y - element.position.y) / (elementHeight/listWidth)
		 + startCount + elementWidth*2,
		(int) list->size()
	);
	
	int selectedElement = element.getListSelected();
	
	int oldBorder = GUIData::borderSize;
	GUIData::borderSize/= 2;//adjusting border for elements
	
	std::string text = element.getDataString("text");
	std::string text2 = element.getDataString("text2");
	
	//drawing elements with special positioning system
	int posI = -1 + startCount % listWidth;
	for (int i = startCount; i < endCount; i++) {
		posI++;
		if (text != "")
			listElement.metadata["text"] = std::regex_replace(text, std::regex("\\|ELEMENT\\|"),
					(*list)[i]);
		else
			listElement.metadata["text"] = (*list)[i];
		if (text2 != "")
			listElement.metadata["text2"] = std::regex_replace(text2, std::regex("\\|ELEMENT\\|"),
					(*list)[i]);
		
		listElement.position.x = startX + elementWidth * (i % listWidth);
		
		if (i % listWidth == listWidth-1)//rounding to list's edge
			listElement.position2.x = element.position2.x-GUIData::borderSize*2;
		else
			listElement.position2.x = listElement.position.x + elementWidth;
		
		listElement.position.y = startY + (elementHeight * (i / listWidth));
		listElement.position2.y = listElement.position.y + elementHeight;
		
		//applyBorder(&listElement.position, &listElement.position2);
		
		if (selectedElement == i)
			listElement.focused = true;
			
		if (element.listActive == i)
			listElement.active = true;
		
		renderElement(listElement);
		
		listElement.focused = false;
		listElement.active = false;
		
	}
	
	
	SDL_RenderSetClipRect(sdl_renderer, NULL);
	GUIData::borderSize = oldBorder;//resetting borders
}


void Renderer::renderTextRaw(char* text, int x, int y, int centerWidth, int centerHeight) {
	//logf("text: %s\n", text.c_str());
	if (text[0] == 0) return;
	
	SDL_Surface* textSurface = TTF_RenderText_Solid(
		currentFont,
		text,
		currentColor);
	
	if(textSurface == NULL) {
		logf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
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
		printX,\
		printY,\
		centerWidth	- (centerWidth ? (GUIData::borderSize+1)*2 : 0),\
		centerHeight- (centerHeight? (GUIData::borderSize-1)*2 : 0));
			
void Renderer::renderText(std::string text, int x, int y, int centerWidth, int centerHeight, bool right) {
	text = GUIData::convertString(text);
	
	char* drawStr = new char[text.length()+2];
	const char* textPtr = text.c_str();
	int drawIndex = 0,
		printY = y +GUIData::borderSize-1,
		printX = x +GUIData::borderSize+1;
	
	for (int i = 0; i < text.length(); i++) {
		if (textPtr[i] == '\n' || (textPtr[i] == '\\' && textPtr[i+1] == 'n')) {
			int textHeight = 0;
			int textWidth = 0;
			TTF_SizeText(currentFont, drawStr, &textWidth, &textHeight);
			if (right)
				printX = x-textWidth -GUIData::borderSize-1;
				
			finishString();
			
			drawIndex = 0;
			printY+= textHeight;
			
			i++;
			continue;
		}
		
		drawStr[drawIndex++] = textPtr[i];
	}
	drawStr[drawIndex] = '\0';
	
	if (right) {
		int textWidth = 0;
		TTF_SizeText(currentFont, drawStr, &textWidth, NULL);
		printX = x-textWidth -GUIData::borderSize-1;
	}
	finishString();
	
	delete[] drawStr;
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
	if (GUIData::fonts.find(fontName) == GUIData::fonts.end())
		return false;
	
	currentFont = GUIData::fonts[fontName];
	return true;
}
void Renderer::updateDefaultFont() {
	defaultFont = GUIData::getFont("Default");
	currentFont = defaultFont;
}

//colors
bool Renderer::setColor(std::string colorName) {
	if (GUIData::colors.find(colorName) == GUIData::colors.end())
		return false;
	
	currentColor = GUIData::colors[colorName];
	
	if (sdl_renderer != NULL)
		SDL_SetRenderDrawColor(sdl_renderer,
			currentColor.r,
			currentColor.g,
			currentColor.b,
			currentColor.a);
	
	return true;
}
void Renderer::clearColors() {
	GUIData::colors.clear();
}

void Renderer::applyBorder(SDL_Rect* rect) {
	rect->x+= GUIData::borderSize;
	rect->y+= GUIData::borderSize;
	rect->w-= GUIData::borderSize*2;
	rect->h-= GUIData::borderSize*2;
}
void Renderer::applyBorder(vec2i* start, vec2i* end) {
	start->x+= GUIData::borderSize;
	start->y+= GUIData::borderSize;
	end->x-= GUIData::borderSize*2;
	end->y-= GUIData::borderSize*2;
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