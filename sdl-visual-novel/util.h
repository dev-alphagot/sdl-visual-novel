#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_Mixer.h>

#include <stdint.h>
#include <stdbool.h>

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
extern bool st_key_guide;

extern SDL_Texture* sc_save_marker;
extern uint8_t sc_save_marker_a;

extern SDL_Color bg_fill_color;

extern char VERSION[16];

extern Mix_Chunk* arrow_sfx;
extern Mix_Chunk* decide_sfx;

int char_uni_bytes(char ss);

void str_trim_lf(int l, char* arr);

float ease_io_expo(float x);

float ease_io_cubic(float x);
