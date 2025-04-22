#define _CRT_SECURE_NO_WARNINGS

#include "settings.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <SDL2/SDL_Mixer.h>
#include <stdbool.h>

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../window.h"

#define SLIDER_COUNT 2
#define SLIDER_ADJ_STEP 5

typedef struct {
	int section;
	int gauge;
	int value;
	int r_value;
} settings_slider_t;

static settings_slider_t sliders[SLIDER_COUNT];

static int slider_sel = 0;

static Mix_Music* music;

static char* slider_options[SLIDER_COUNT] = {
	u8"배경음악 볼륨",
	u8"효과음 볼륨"
};

static void sc_settings_highlight(void) {
	printf("SSEL %d\n", slider_sel);

	for (int i = 0; i < SLIDER_COUNT; i++) {
		image_alpha(sliders[i].gauge, 128);
		text_color(sliders[i].section, 255, 255, 255, 128);
		text_color(sliders[i].value, 255, 255, 255, 128);
	}

	image_alpha(sliders[slider_sel].gauge, 255);
	text_color(sliders[slider_sel].section, 255, 255, 255, 255);
	text_color(sliders[slider_sel].value, 255, 255, 255, 255);

	Mix_PlayChannel(-1, arrow_sfx, 0);
}

static void sc_settings_highlight_silent(void) {
	for (int i = 0; i < SLIDER_COUNT; i++) {
		image_alpha(sliders[i].gauge, 128);
		text_color(sliders[i].section, 255, 255, 255, 128);
		text_color(sliders[i].value, 255, 255, 255, 128);
	}

	image_alpha(sliders[slider_sel].gauge, 255);
	text_color(sliders[slider_sel].section, 255, 255, 255, 255);
	text_color(sliders[slider_sel].value, 255, 255, 255, 255);
}

static void sc_settings_value_adjust(int facing) {
	static char pbuf[5];

	int rv = sliders[slider_sel].r_value + (facing * SLIDER_ADJ_STEP);
	iclamp(&rv, 0, 100);

	printf("SVAL %d %f\n", rv, (float)rv / 40.0f);

	sliders[slider_sel].r_value = rv;
	image_scale(sliders[slider_sel].gauge, (float)rv / 40.0f, 0.25f);

	sprintf(pbuf, "%d%%", rv);
	text_content(sliders[slider_sel].value, _strdup(pbuf));

	Mix_PlayChannel(-1, arrow_sfx, 0);

	switch (slider_sel) {
	case 0:
		vol_bgm = (uint8_t)round(rv * 1.28f);
		Mix_VolumeMusic(vol_bgm);
		break;
	case 1:
		Mix_VolumeChunk(decide_sfx, vol_sfx);
		Mix_VolumeChunk(arrow_sfx, vol_sfx);
		vol_sfx = (uint8_t)round(rv * 1.28f);
		break;
	}
}

static void sc_settings_initialize(void) {
	char pbuf[5] = "";

	image_add(
		"image/bg/title_f.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		0.666667f, 0.666667f, H_CENTER, V_CENTER
	);
	
	for (int i = 0; i < SLIDER_COUNT; i++) {
		int r = i == 0 ? (int)(round(vol_bgm / 1.28f)) : (int)(round(vol_sfx / 1.28f));

		sprintf(pbuf, "%d%%", r);

		sliders[i] = (settings_slider_t){
			-text_add_as(slider_options[i], PRETENDARDJPMEDIUM, WINDOW_WIDTH / 2 - 190, WINDOW_HEIGHT / 2 - 35 + (i * 70), 255, 255, 255, 255, 0.4f, 0.4f, LEFT, BOTTOM),
			-image_add(
				"image/bg/white.png",
				WINDOW_WIDTH / 2 - 190 + 75, WINDOW_HEIGHT / 2 - 35 + 30 + (i * 70),
				0.025f * r, 0.25f, LEFT, BOTTOM
			),
			-text_add_as(_strdup(pbuf), PRETENDARDJPMEDIUM, WINDOW_WIDTH / 2 - 190 + 350, WINDOW_HEIGHT / 2 - 35 + 30 + (i * 70), 255, 255, 255, 255, 0.4f, 0.4f, LEFT, BOTTOM),
			r
		};
	}

	sc_settings_highlight_silent();

	music = Mix_LoadMUS("sound/bgm/cosmos.ogg");
	if (music == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
		return 1;
	}

	Mix_FadeInMusic(music, 1 << 30, 5000);
}

static void sc_settings_render(void) {
	if (input_is_keydown(SDLK_UP)) {
		slider_sel--;
		iclamp(&slider_sel, 0, SLIDER_COUNT - 1);
		sc_settings_highlight();
	}
	else if (input_is_keydown(SDLK_DOWN)) {
		slider_sel++;
		iclamp(&slider_sel, 0, SLIDER_COUNT - 1);
		sc_settings_highlight();
	}

	if (input_is_keydown(SDLK_LEFT)) {
		sc_settings_value_adjust(-1);
	}
	else if (input_is_keydown(SDLK_RIGHT)){
		sc_settings_value_adjust(1);
	}

	if (input_is_keydown(SDLK_x)) {
		screen_change("title");
	}
}

static void sc_settings_music_free(void) {
	if (music) Mix_FreeMusic(music);
	Mix_HookMusicFinished(NULL);
}

static void sc_settings_dispose(void) {
	FILE* settings = fopen("settings.bin", "wb");

	fwrite(&vol_bgm, 1, 1, settings);
	fwrite(&vol_sfx, 1, 1, settings);

	fclose(settings);

	Mix_FadeOutMusic(2000);

	Mix_HookMusicFinished(sc_settings_music_free);
}

screen_t sc_settings = {
	"settings", sc_settings_initialize, sc_settings_render, sc_settings_dispose
};
