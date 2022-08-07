#include "renderer.h"

static SDL_Renderer* sdl_renderer = NULL;

void Renderer::renderElement(UIElement element) {
	// printf("Renderer: %s: %d:%d:%d:%d\n",
	// 	element.metadata["name"].as<std::string>().c_str(),
	// 	element.position.x,
	// 	element.position.y,
	// 	element.position2.x,
	// 	element.position2.y
	// );
	
	//rendering element/container
	SDL_Rect rect = {
		element.position.x,
		element.position.y,
		element.position2.x,
		element.position2.y
	};
	
	SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
	
	SDL_RenderDrawRect(sdl_renderer, &rect);
	
	//rendering children (recursive)
	if (!element.elements.empty()) {
		for (auto const& child : element.elements) {
			renderElement(child.second);
		}
	}
}


void Renderer::setRenderer(SDL_Renderer* renderer) {
	if (sdl_renderer != NULL)
		SDL_DestroyRenderer(sdl_renderer);
	
	sdl_renderer = renderer;
}

void Renderer::start() {
	SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x00, 0xff, 0xff);
	SDL_RenderClear(sdl_renderer);
}

void Renderer::finish() {
	SDL_RenderPresent(sdl_renderer);
}