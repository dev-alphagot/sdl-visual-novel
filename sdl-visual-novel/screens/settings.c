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

#define RADIO_COUNT 2
#define RADIO_MAX_OPTION 2

#define OPTION_COUNT (SLIDER_COUNT + RADIO_COUNT)

#define RYUGU 3250

typedef enum {
	SLIDER, RADIO
} settings_option_type_t;

typedef struct {
	settings_option_type_t type;
	int section;
	int gauge;
	int val_text;
	int r_value;
} settings_option_t;

static settings_option_t sliders[OPTION_COUNT];

static int slider_sel = 0;

static Mix_Music* music;

static char* slider_options[OPTION_COUNT] = {
	u8"배경음악 볼륨",
	u8"효과음 볼륨",
	u8"키 가이드 표시",
	u8"저품질 스케일 사용"
};

static int kg;

static char* radio_val_texts[RADIO_COUNT][RADIO_MAX_OPTION] = {
	{ u8"표시", u8"숨김" },
	{ u8"사용", u8"사용 안 함" },
};

static void sc_settings_highlight(void) {
	printf("SSEL %d\n", slider_sel);

	for (int i = 0; i < OPTION_COUNT; i++) {
		if(sliders[i].gauge > -1) image_alpha(sliders[i].gauge, 128);
		text_color(sliders[i].section, 255, 255, 255, 128);
		text_color(sliders[i].val_text, 255, 255, 255, 128);
	}

	if (sliders[slider_sel].gauge > -1) image_alpha(sliders[slider_sel].gauge, 255);
	text_color(sliders[slider_sel].section, 255, 255, 255, 255);
	text_color(sliders[slider_sel].val_text, 255, 255, 255, 255);

	Mix_PlayChannel(-1, arrow_sfx, 0);
}

static void sc_settings_highlight_silent(void) {
	for (int i = 0; i < OPTION_COUNT; i++) {
		if (sliders[i].gauge > -1) image_alpha(sliders[i].gauge, 128);
		text_color(sliders[i].section, 255, 255, 255, 128);
		text_color(sliders[i].val_text, 255, 255, 255, 128);
	}

	if (sliders[slider_sel].gauge > -1) image_alpha(sliders[slider_sel].gauge, 255);
	text_color(sliders[slider_sel].section, 255, 255, 255, 255);
	text_color(sliders[slider_sel].val_text, 255, 255, 255, 255);
}

static void sc_settings_value_adjust(int facing) {
	static char pbuf[13];

	settings_option_t opt = sliders[slider_sel];

	if (opt.type == SLIDER) {
		int rv = opt.r_value + (facing * SLIDER_ADJ_STEP);
		iclamp(&rv, 0, 100);

		printf("SVAL %d %f\n", rv, (float)rv / 40.0f);

		sliders[slider_sel].r_value = rv;
		image_scale(opt.gauge, (float)rv / 40.0f, 0.25f);

		sprintf(pbuf, "%d%%", rv);
		text_content(opt.val_text, _strdup(pbuf));

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
	else if (opt.type == RADIO) {
		int rv = opt.r_value + facing;
		
		int mx = 0;
		for (int i = 0; i < RADIO_MAX_OPTION; i++) {
			if (radio_val_texts[slider_sel - SLIDER_COUNT][i] == NULL) break;
			mx++;
		}

		rv = (rv + mx) % mx;

		text_content(opt.val_text, radio_val_texts[slider_sel - SLIDER_COUNT][rv]);
		sliders[slider_sel].r_value = rv;

		switch (slider_sel) {
		case 2:
			st_key_guide = !rv;
			text_move(kg, 0, RYUGU * (st_key_guide ? -1 : 1));
			break;
		case 3:
			st_scale_low_quality = !rv;
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, st_scale_low_quality ? "nearest" : "best");
			break;
		}
	}

	Mix_PlayChannel(-1, arrow_sfx, 0);
}

static void sc_settings_initialize(void) {
	printf("%d\n", OPTION_COUNT);

	char pbuf[5] = "";

	image_add(
		"image/bg/title_f.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		0.666667f, 0.666667f, H_CENTER, V_CENTER
	);

	kg = -text_add_as(
		u8"⇅ 키로 선택 ⇄ 키로 조절",
		NANUMGOTHICEXTRABOLD,
		WINDOW_WIDTH / 2 + 24, WINDOW_HEIGHT - 36,
		255, 255, 255, 255,
		0.333333f, 0.333333f, H_CENTER, V_CENTER
	);
	text_move(kg, 0, RYUGU * (st_key_guide ? 0 : 1));

	for (int i = 0; i < SLIDER_COUNT; i++) {
		int r = i == 0 ? (int)(round(vol_bgm / 1.28f)) : (int)(round(vol_sfx / 1.28f));

		sprintf(pbuf, "%d%%", r);

		sliders[i] = (settings_option_t){
			SLIDER,
			-text_add_as(slider_options[i], PRETENDARDJPMEDIUM, WINDOW_WIDTH / 2 - 190, WINDOW_HEIGHT / 2 - 105 + (i * 70), 255, 255, 255, 255, 0.4f, 0.4f, LEFT, BOTTOM),
			-image_add(
				"image/bg/white.png",
				WINDOW_WIDTH / 2 - 190 + 75, WINDOW_HEIGHT / 2 - 105 + 30 + (i * 70),
				0.025f * r, 0.25f, LEFT, BOTTOM
			),
			-text_add_as(_strdup(pbuf), PRETENDARDJPMEDIUM, WINDOW_WIDTH / 2 - 190 + 350, WINDOW_HEIGHT / 2 - 105 + 30 + (i * 70), 255, 255, 255, 255, 0.4f, 0.4f, LEFT, BOTTOM),
			r
		};
	}

	for (int i = SLIDER_COUNT; i < OPTION_COUNT; i++) {
		int r = i == 0 ? (int)(st_key_guide) : (int)(!st_scale_low_quality);

		sliders[i] = (settings_option_t){
			RADIO,
			-text_add_as(slider_options[i], PRETENDARDJPMEDIUM, WINDOW_WIDTH / 2 - 190, WINDOW_HEIGHT / 2 - 105 + (i * 70), 255, 255, 255, 255, 0.4f, 0.4f, LEFT, BOTTOM),
			-1,
			-text_add_as(radio_val_texts[i - SLIDER_COUNT][r], PRETENDARDJPMEDIUM, WINDOW_WIDTH / 2 - 190 + 20, WINDOW_HEIGHT / 2 - 105 + 30 + (i * 70), 255, 255, 255, 255, 0.333333f, 0.333333f, LEFT, BOTTOM),
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
		iclamp(&slider_sel, 0, OPTION_COUNT - 1);
		sc_settings_highlight();
	}
	else if (input_is_keydown(SDLK_DOWN)) {
		slider_sel++;
		iclamp(&slider_sel, 0, OPTION_COUNT - 1);
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
	fwrite(&st_key_guide, 1, 1, settings);
	fwrite(&st_scale_low_quality, 1, 1, settings);

	fclose(settings);

	Mix_FadeOutMusic(2000);

	Mix_HookMusicFinished(sc_settings_music_free);
}

screen_t sc_settings = {
	"settings", sc_settings_initialize, sc_settings_render, sc_settings_dispose
};
