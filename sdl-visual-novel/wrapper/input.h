#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#define INPUT_MAX_KEYS 16

void input_keydown(SDL_KeyCode key);

void input_keyup(SDL_KeyCode key);

bool input_is_keydown(SDL_KeyCode key);

void input_update(void);
