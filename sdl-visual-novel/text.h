#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>

typedef enum {
	INVALID_FONT,
	STRING_NULL,
	FONT_TEXTURE_NULL
} err_text_t;

typedef enum {
	NANUMBARUNGOTHIC,
	NANUMBARUNPENB,
	NANUMGOTHIC,
	FONT_COUNT
} font_t;

void text_init(SDL_Renderer* renderer);
