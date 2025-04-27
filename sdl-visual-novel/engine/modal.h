#pragma once

#include <SDL2/SDL.h>

#include <stdbool.h>

typedef void (*modal_behavior_t)(void);

typedef enum {
	ACTIONS_OK,
	ACTIONS_OK_CANCEL
} modal_actions_t;

extern bool modal_is_on;

void modal_init(SDL_Renderer* renderer);

bool modal_on(modal_actions_t ac, const char* text, modal_behavior_t ok, modal_behavior_t cc);
void modal_off(void);

void modal_render(void);
