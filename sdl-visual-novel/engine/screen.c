#include <SDL2/SDL.h>

#include "screen.h"
#include "fupdate.h"

#include "../window.h"
#include "../util.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

screen_t screens[6];
static int screen_count = -1;

static SDL_Renderer* renderer;

static screen_t screen_current;
static screen_t screen_desired;

static int stf = -1;

static void screen_transition(void) {
	if (stf < 120) {
		for (int i = 0; i < 4; i++) {
			int a = ((stf + 1) - (i * 15)) * 15;
			SDL_Rect rc = { 0, (WINDOW_HEIGHT / 4) * i, WINDOW_WIDTH, WINDOW_HEIGHT / 4};

			SDL_Color orc = { 0, 0, 0, 0 };

			iclamp(&a, 0, 255);

			SDL_GetRenderDrawColor(renderer, &orc.r, &orc.g, &orc.b, &orc.a);

			SDL_SetRenderDrawColor(renderer, 13, 0, 33, a);
			SDL_RenderFillRect(renderer, &rc);

			SDL_SetRenderDrawColor(renderer, orc.r, orc.g, orc.b, orc.a);
		}

		if (stf == 62) {
			screen_current.dispose();

			text_clear();
			image_clear();
		}
		else if (stf == 119) {
			screen_desired.initialize();
			screen_current = screen_desired;
		}
	}
	else if (stf < 180) {
		for (int i = 0; i < 4; i++) {
			int a = (15 - (((stf - 120) + 1) - (i * 15))) * 15;
			SDL_Rect rc = { 0, (WINDOW_HEIGHT / 4) * i, WINDOW_WIDTH, WINDOW_HEIGHT / 4};

			SDL_Color orc = { 0, 0, 0, 0 };

			iclamp(&a, 0, 255);

			SDL_GetRenderDrawColor(renderer, &orc.r, &orc.g, &orc.b, &orc.a);

			SDL_SetRenderDrawColor(renderer, 13, 0, 33, a);
			SDL_RenderFillRect(renderer, &rc);

			SDL_SetRenderDrawColor(renderer, orc.r, orc.g, orc.b, orc.a);
		}
	}
	else {
		input_available = true;
		stf = -2;
	}

	stf++;
}

#ifdef SCREEN_TRANSITION_DEBUG
void screen_tr(void) {
	screen_transition();
}

void screen_ss(void) {
	stf = 0;
}
#endif

void screen_init(SDL_Renderer* _renderer, int scc) {
	renderer = _renderer;
	screen_count = scc;

	screen_current = screens[0];

	screen_current.initialize();
}

static void screen_change_o(screen_t screen) {
	// if (!strcmp(screen_current.id, screen.id)) return;
	if (stf >= 0) return;

	screen_desired = screen;
	stf = 0;

	input_available = false;

	fupdate_add(181, screen_transition);
}

void screen_change(const char* id) {
	for (int i = 0; i < screen_count; i++) {
		if (!strcmp(id, screens[i].id)) {
			screen_change_o(screens[i]);
			break;
		}
	}
}

void screen_render(void) {
	screen_current.render();
}
