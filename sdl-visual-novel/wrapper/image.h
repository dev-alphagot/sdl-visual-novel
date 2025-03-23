#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include "../util.h"

#define IMAGE_CAPACITY 16

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

err_image_t image_remove(int index);

err_image_t image_content(int id, const char* new_path);

void image_render(void);

void image_end(void);

void image_clear(void);
