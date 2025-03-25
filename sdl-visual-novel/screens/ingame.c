#define _CRT_SECURE_NO_WARNINGS

#include "ingame.h"

#include "../util.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../engine/texthold.h"
#include "../engine/fupdate.h"
#include "../engine/character.h"

#include <SDL2/SDL_Mixer.h>

static int ntId = 0;

static int bg = 0;
static int bg_cf = 0;
static int heroine = 0;

static int speak_bg = 0;
static int speak_name = 0;
static int speak_content = 0;

static int emo_ticks = -1;
static int emo_index = 0;

static int bg_cf_ticks = -1;
static int bg_cf_index = 0;

static int spk_ticks = -1;
static int spk_offset = -1;

static int tid = 100001;

char name[ 128] = u8" ";
char text[2048] = u8" ";
static char  spk_buffer[2048] = "";

static Mix_Music* bgm;

static character_t chr;

static text_h_t th = { -1, "" };

static char emo_buf[CHAR_BASE_MAX_PATH + CHAR_EMOTION_MAX_PATH] = "";

static void sc_ingame_emotion(void) {
	emotion_t emo = chr.emotions[emo_index + 1];

	if (emo_ticks == 0) {
		sprintf(emo_buf, "%s/%s.png", chr.path, emo.path);
		printf("%s\n", emo.path);
		image_content(heroine, emo_buf);
	}

	// printf("%f\n", task_ticks > 15 ? ease_io_expo(1.0f - ((task_ticks - 15) / 15.0f)) : ease_io_expo(task_ticks / 15.0f));

	if (emo_ticks >= 1 && emo_ticks <= 22) {
		image_move(heroine, 0, (int)ceil(-4 * emo.movement_multiplier * ease_io_expo(emo_ticks / 22.0f)));
	}
	else if (emo_ticks >= 23 && emo_ticks <= 44) {
		image_move(heroine, 0, (int)floor(4 * emo.movement_multiplier * ease_io_expo(1.0f - ((emo_ticks - 23) / 22.0f))));
	}

	if (emo_ticks == 44) {
		image_content(heroine, "image/heroine/neutral.png");
		emo_ticks = -2;

		emo_index++;
		emo_index %= 7;
	}

	emo_ticks++;
}

static void sc_ingame_bg_cf(void) { // 배경 크로스페이드
	const char* bgf = bg_cf_index ? "image/bg/Stairs Day.png" : "image/bg/Classroom Day.png";

	if (bg_cf_ticks == 0) {
		image_content(bg_cf, bgf);
	}

	// printf("%f\n", task_ticks > 15 ? ease_io_expo(1.0f - ((task_ticks - 15) / 15.0f)) : ease_io_expo(task_ticks / 15.0f));

	if (bg_cf_ticks >= 1 && bg_cf_ticks <= 60) {
		float va = ease_io_cubic(bg_cf_ticks / 60.0f);

		image_alpha(bg_cf, (uint8_t)ceil(va * 255));
		image_alpha(bg, (uint8_t)floor((1.0f - va) * 255));
	}

	if (bg_cf_ticks == 60) {
		image_content(bg, bgf);
		image_alpha(bg, 255);
		image_alpha(bg_cf, 0);
		bg_cf_ticks = -2;
		bg_cf_index = ++bg_cf_index % 2;
	}

	bg_cf_ticks++;
}

static void sc_ingame_initialize(void) {
	chr = characters[0];

	th_search(100000, &th);
	memcpy(name, th.value, 128);

	//printf("%s %s\n", name, text);
	//printf("START\n");

	bgm = Mix_LoadMUS("sound/bgm/dotabata.ogg");
	if (bgm == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
	}

	bg = -image_add(
		"image/bg/Stairs Day.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		0.666667f, 0.666667f, H_CENTER, H_CENTER
	);
	bg_cf = -image_add(
		"image/bg/Stairs Afternoon.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		0.666667f, 0.666667f, H_CENTER, H_CENTER
	);

	image_alpha(bg_cf, 0);

	heroine = -image_add(
		"image/heroine/neutral.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT,
		1.2f, 1.2f, H_CENTER, BOTTOM
	);
	
	speak_bg = -image_add(
		"image/ui/text_bg.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT,
		1.0f, 1.0f, H_CENTER, BOTTOM
	);

	speak_name = -text_add_as(
		name,
		GYEONGGIMILLENNIUMBATANGB,
		104, WINDOW_HEIGHT - 104,
		255, 255, 255, 255,
		0.8f, 0.8f, H_CENTER, V_CENTER
	);

	speak_content = -text_add_as(
		spk_buffer,
		COMBINED,
		220, WINDOW_HEIGHT - 180,
		255, 255, 255, 255,
		0.5, 0.5, LEFT, TOP
	);

	// Mix_PlayMusic(titlemusic, 1 << 30);

	Mix_FadeInMusic(bgm, 1 << 30, 5000);
}

static void sc_ingame_render(void) {
	if (emo_ticks < 0 && input_is_keydown(SDLK_a)) {
		emo_ticks = 0;
		fupdate_add(45, sc_ingame_emotion);
	}

	if (bg_cf_ticks < 0 && input_is_keydown(SDLK_s)) {
		bg_cf_ticks = 0;
		fupdate_add(61, sc_ingame_bg_cf);
	}

	if (spk_ticks < 0 && spk_offset < 0 && input_is_keydown(SDLK_z)) {
		memset(spk_buffer, 0, 2048);
		th_search(tid++, &th);
		memcpy(text, th.value, 2048);
		spk_offset = 0;
		spk_ticks = 0;
	}

	if (spk_ticks-- == 0 && spk_offset < strlen(text)) {
		int cb = char_uni_bytes(text[spk_offset]);

		spk_offset += cb;
		if (text[spk_offset - 1] == '@') text[spk_offset - 1] = '\n';
		memcpy(spk_buffer, text, spk_offset);
		//printf("%d %c %s\n", spk_offset - 1, spk_buffer[spk_offset - 1], spk_buffer);

		text_content(speak_name, name);

		text_content(speak_content, spk_buffer);

		spk_ticks = cb;
	}
	else if (spk_ticks == 0 && spk_offset >= strlen(text)) {
		spk_offset = -1;
		spk_ticks = 0;
	}
}

static void sc_ingame_music_free(void) {
	Mix_FreeMusic(bgm);
	Mix_HookMusicFinished(NULL); // 개같이 버그 해결
}

static void sc_ingame_dispose(void) {
	Mix_FadeOutMusic(2000);

	Mix_HookMusicFinished(sc_ingame_music_free);
}

screen_t sc_ingame = {
	"ingame", sc_ingame_initialize, sc_ingame_render, sc_ingame_dispose
};
