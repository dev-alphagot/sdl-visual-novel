#pragma once

#define _CRT_SECURE_NO_WARNINGS

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

typedef enum {
	LEFT,
	CENTER,
	RIGHT
} halign_t;

typedef enum {
	TOP,
	CENTER,
	BOTTOM
} valign_t;

typedef struct {
	font_t font;
	SDL_Color color;
	const char* text;
	int x;
	int y;
	int w;
	float scale_x;
	float scale_y;
	halign_t halign;
	valign_t valign;
} text_t;

const char* text_error_str(err_text_t err);

err_text_t text_init(SDL_Renderer* renderer);
void text_end(void);

err_text_t text_add_o(const char* text, SDL_Color color, font_t font, int x, int y, int w);

err_text_t text_add(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
err_text_t text_add_r(const char* text, font_t font, int x, int y, int w, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

err_text_t text_remove(int index);

void text_render(void);
