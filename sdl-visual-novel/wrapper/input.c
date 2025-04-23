#include "input.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

bool input_available = true;

static SDL_KeyCode keys[INPUT_MAX_KEYS];
static SDL_KeyCode keys_prev[INPUT_MAX_KEYS];

void input_keydown(SDL_KeyCode key) {
	for (int i = 0; i < INPUT_MAX_KEYS; i++) {
		if (!keys[i]) continue;

		if (key == keys[i]) return;
	}

	for (int i = 0; i < INPUT_MAX_KEYS; i++) {
		if (!keys[i]) {
			keys[i] = key;
			return;
		}
	}
}

void input_keyup(SDL_KeyCode key) {
	for (int i = 0; i < INPUT_MAX_KEYS; i++) {
		if (!keys[i]) continue;

		if (key == keys[i]) {
			keys[i] = 0;
			return;
		}
	}
}

bool input_is_keydown(SDL_KeyCode key) {
	for (int i = 0; i < INPUT_MAX_KEYS; i++) {
		if (!keys[i]) continue;

		if (key == keys[i]){
			for (int i = 0; i < INPUT_MAX_KEYS; i++) {
				if (!keys_prev[i]) continue;

				if (key == keys_prev[i]) {
					return false;
				}
			}

			return true;
		}
	}

	return false;
}

bool input_is_keypressed(SDL_KeyCode key) {
	for (int i = 0; i < INPUT_MAX_KEYS; i++) {
		if (!keys[i]) continue;

		if (key == keys[i]) {
			return true;
		}
	}

	return false;
}

void input_update(void) {
	memcpy(keys_prev, keys, sizeof(SDL_KeyCode) * INPUT_MAX_KEYS);
}
