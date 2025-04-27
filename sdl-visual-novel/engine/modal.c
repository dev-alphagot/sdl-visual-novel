#include "modal.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../window.h"
#include "../wrapper/text.h"
#include "../wrapper/input.h"

#include <stdbool.h>

#define MODAL_TEXT_SIZE 0.5f
#define MODAL_KEY_GUIDE_SIZE 0.333333f

static SDL_Renderer* renderer;
static SDL_Texture* modal_bg;

static int modal_bg_w = 0;
static int modal_bg_h = 0;

static SDL_Texture* modal_text_tex;
static int modal_text_w = 0;
static int modal_text_h = 0;

static SDL_Texture* modal_key_guide;
static int modal_kg_w = 0;
static int modal_kg_h = 0;

bool modal_is_on = false;
bool modal_first_frame_skip = false;

static modal_behavior_t md_ok;
static modal_behavior_t md_cancel;

static const char* modal_key_guide_text(modal_actions_t t) {
	switch (t) {
	case ACTIONS_OK: return u8"Z | 확인";
	case ACTIONS_OK_CANCEL: return u8"Z | 확인\nX | 취소";
	}
}

void modal_init(SDL_Renderer* r) {	// Initialize the modal system with the given renderer
	renderer = r;
	modal_bg = SDL_CreateTextureFromSurface(renderer, IMG_Load("image/ui/modal_bg.png"));

	SDL_SetTextureBlendMode(modal_bg, SDL_BLENDMODE_BLEND);

	SDL_QueryTexture(modal_bg, NULL, NULL, &modal_bg_w, &modal_bg_h);
}

bool modal_on(modal_actions_t ac, const char* text, modal_behavior_t ok, modal_behavior_t cc) {
	modal_text_tex = text_create_texture(text, (SDL_Color){ 0, 0, 0, 255 }, PRETENDARDJPLIGHT, (modal_bg_w - 32) / MODAL_TEXT_SIZE, 1.0f, 1.0f);
	modal_key_guide = text_create_texture(modal_key_guide_text(ac), (SDL_Color) { 0, 0, 0, 255 }, SPOQAHANSANSBOLD, (modal_bg_w - 32) / MODAL_KEY_GUIDE_SIZE, 1.0f, 1.0f);

	SDL_QueryTexture(modal_text_tex, NULL, NULL, &modal_text_w, &modal_text_h);
	SDL_QueryTexture(modal_key_guide, NULL, NULL, &modal_kg_w, &modal_kg_h);

	md_ok = ok;
	md_cancel = cc;

	modal_is_on = true;
	modal_first_frame_skip = true;
}

void modal_off(void) {
	if (!modal_is_on) return;

	SDL_DestroyTexture(modal_text_tex);
	SDL_DestroyTexture(modal_key_guide);

	modal_is_on = false;
}

void modal_render(void) {
	if (!modal_is_on) return;

	SDL_RenderCopy(renderer, modal_bg, NULL, &(
		(SDL_Rect) { (WINDOW_WIDTH - modal_bg_w) / 2, (WINDOW_HEIGHT - modal_bg_h) / 2, modal_bg_w, modal_bg_h }
		)
	);
	SDL_RenderCopy(renderer, modal_text_tex, NULL, &(
		(SDL_Rect) { (WINDOW_WIDTH - modal_text_w * MODAL_TEXT_SIZE) / 2, (WINDOW_HEIGHT - modal_text_h * MODAL_TEXT_SIZE) / 2, modal_text_w * MODAL_TEXT_SIZE, modal_text_h * MODAL_TEXT_SIZE }
		)
	);
	SDL_RenderCopy(renderer, modal_key_guide, NULL, &(
		(SDL_Rect) { (WINDOW_WIDTH - modal_bg_w) / 2 + 12, (WINDOW_HEIGHT / 2) + (modal_bg_h / 2) - 12 - modal_kg_h * MODAL_KEY_GUIDE_SIZE, modal_kg_w * MODAL_KEY_GUIDE_SIZE, modal_kg_h * MODAL_KEY_GUIDE_SIZE }
		)
	);

	if (modal_first_frame_skip) {
		modal_first_frame_skip = false;
		return;
	}

	if (input_is_keydown(SDLK_z) && md_ok) {
		modal_off();
		md_ok();
	}
	else if (input_is_keydown(SDLK_x) && md_cancel) {
		modal_off();
		md_cancel();
	}
}
