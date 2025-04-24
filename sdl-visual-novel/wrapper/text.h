#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>

#include "../window.h"
#include "../util.h"

#define TEXT_CAPACITY 128

typedef enum {
	TEXT_INVALID_FONT = 1,
	TEXT_STRING_NULL,
	TEXT_FONT_TEXTURE_NULL,
	TEXT_CAPACITY_EXCEEDED,
	TEXT_INVALID_INDEX,
	TEXT_FONT_LOAD_FAILED,
	TEXT_SURFACE_NULL
} err_text_t;

typedef enum {
	NANUMBARUNGOTHIC,
	NANUMBARUNPENB,
	NANUMGOTHIC,
	PRETENDARDJPLIGHT,
	GYEONGGIMILLENNIUMBATANGB,
	HUIJI,
	COMBINED,
	JANGMICHE,
	CAFE24SSUKSSUK,
	PRETENDARDJPMEDIUM,
	NANUMGOTHICBOLD,
	NANUMGOTHICEXTRABOLD,
	SPOQAHANSANSBOLD,
	FONT_COUNT
} font_t;

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

err_text_t text_add_o(const char* text, SDL_Color color, font_t font, int x, int y, int w, float sx, float sy, halign_t ha, valign_t va);

err_text_t text_add(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
err_text_t text_add_a(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, halign_t ha, valign_t va);
err_text_t text_add_s(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float sx, float sy);
err_text_t text_add_as(const char* text, font_t font, int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, float sx, float sy, halign_t ha, valign_t va);
err_text_t text_add_r(const char* text, font_t font, int x, int y, int w, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

err_text_t text_remove(int index);

err_text_t text_content(int id, const char* content);

err_text_t text_color(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

err_text_t text_move(int id, int x, int y);

err_text_t text_pos(int id, int x, int y);

err_text_t text_font(int id, font_t font);

err_text_t text_get_rect(int id, SDL_Rect* const rc);

void text_render(void);

void text_clear(void);
