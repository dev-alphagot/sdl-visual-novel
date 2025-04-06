#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>

void iclamp(int* v, int mi, int mx);

typedef enum {
	LEFT,
	H_CENTER,
	RIGHT
} halign_t;

typedef enum {
	TOP,
	V_CENTER,
	BOTTOM
} valign_t;

extern int quit;

extern SDL_Renderer* renderer;

extern uint8_t vol_bgm;
extern uint8_t vol_sfx;

extern SDL_Texture* sc_save_marker;
extern uint8_t sc_save_marker_a;

int char_uni_bytes(char ss);

void str_trim_lf(int l, char* arr);

float ease_io_expo(float x);

float ease_io_cubic(float x);
