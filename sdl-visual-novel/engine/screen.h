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
	void (*focus_lost)(void);
	void (*focus_restore)(void);
} screen_t;

extern screen_t screens[];

void screen_init(SDL_Renderer* _renderer, int scc);

void screen_change(const char* id);

void screen_render(void);

void screen_focus_lost(void);

void screen_focus_restore(void);

#ifdef SCREEN_TRANSITION_DEBUG
void screen_tr(void);

void screen_ss(void);
#endif
