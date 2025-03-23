#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>

// #define SCREEN_TRANSITION_DEBUG

typedef struct {
	const char* id;

	void (*initialize)(void);
	void (*render)(void);
	void (*dispose)(void);
} screen_t;

extern screen_t screens[6];

void screen_init(SDL_Renderer* _renderer, int scc);

void screen_change(const char* id);

void screen_render(void);

#ifdef SCREEN_TRANSITION_DEBUG
void screen_tr(void);

void screen_ss(void);
#endif
