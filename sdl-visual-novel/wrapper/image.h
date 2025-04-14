#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include "../util.h"

#define IMAGE_CAPACITY 128

#define IMAGE_RECT_REUSE -3250000

typedef enum {
	IMAGE_LOAD_FAILED = 1,
	IMAGE_INVALID_RENDERER,
	IMAGE_INVALID_PATH,
	IMAGE_CAPACITY_EXCEEDED,
	IMAGE_INVALID_INDEX,
	IMAGE_SURFACE_NULL
} err_image_t;

typedef struct {
	SDL_Texture* tex;
	SDL_Rect rect;
	float scale_x;
	float scale_y;
	halign_t halign;
	valign_t valign;
	SDL_Color color;
} image_t;

err_image_t image_init(SDL_Renderer* _renderer);

const char* image_error_str(err_image_t err);

err_image_t image_add(
	const char* path,
	int x,
	int y,
	float scale_x,
	float scale_y,
	halign_t halign,
	valign_t valign
);

err_image_t image_add_tex(
	SDL_Texture* tex, int width, int height,
	int x,
	int y,
	float scale_x,
	float scale_y,
	halign_t halign,
	valign_t valign
);

err_image_t image_remove(int index);

err_image_t image_remove_nofree(int index);

err_image_t image_content(int id, const char* new_path);

err_image_t image_move(int id, int x, int y);

err_image_t image_pos(int id, int x, int y);

err_image_t image_scale(int id, float sx, float sy);

err_image_t image_alpha(int id, uint8_t a);

err_image_t image_color(int id, uint8_t r, uint8_t g, uint8_t b);

err_image_t image_get_rect(int id, SDL_Rect* const rc);

void image_render(void);

void image_end(void);

void image_clear(void);
