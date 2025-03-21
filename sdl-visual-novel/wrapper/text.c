#include "text.h"
#include "../window.h"

static SDL_Texture* font_texture;

void text_init(SDL_Renderer* renderer) {
	font_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void text_render(const char* text, SDL_Color color, font_t font, )
