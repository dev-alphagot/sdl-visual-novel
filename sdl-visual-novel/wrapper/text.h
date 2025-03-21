#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>

#include "../window.h"

#define TEXT_CAPACITY 128

typedef enum {
	INVALID_FONT = 1,
	STRING_NULL,
	FONT_TEXTURE_NULL,
	CAPACITY_EXCEEDED,
	INVALID_INDEX,
	FONT_LOAD_FAILED,
	TEXT_SURFACE_NULL
} err_text_t;

typedef enum {
	NANUMBARUNGOTHIC,
	NANUMBARUNPENB,
	NANUMGOTHIC,
	FONT_COUNT
} font_t;

typedef struct {
	font_t font;
	SDL_Color color;
	const char* text;
	int x;
	int y;
	int w;
	int h;
} text_t;

err_text_t text_init(SDL_Renderer* renderer);
void text_end(void);

err_text_t text_add(const char* text, SDL_Color color, font_t font, int x, int y, int w, int h);

err_text_t text_add(const char* text, int r, int g, int b, font_t font, int x, int y);
err_text_t text_add(const char* text, int r, int g, int b, int a, font_t font, int x, int y);
err_text_t text_add(const char* text, int r, int g, int b, font_t font, int x, int y, int w, int h);
err_text_t text_add(const char* text, int r, int g, int b, int a, font_t font, int x, int y, int w, int h);

err_text_t text_remove(int index);

void text_render(void);
