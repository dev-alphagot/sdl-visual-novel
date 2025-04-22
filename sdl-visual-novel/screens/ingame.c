#define _CRT_SECURE_NO_WARNINGS

#include "ingame.h"

#include "../util.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../engine/texthold.h"
#include "../engine/fupdate.h"
#include "../engine/character.h"
#include "../engine/script.h"
#include "../engine/screen.h"

#include <SDL2/SDL_Mixer.h>

#include <stdbool.h>

static int ntId = 0;

static int bg = 0;
static int bg_cf = 0;
       int ingame_char = 0;

static int speak_bg = 0;
static int speak_name = 0;
       int speak_content = 0;

static int cg_id = 0;
static int cg_show_ticks = -1;

static int char_show_ticks = -1;

static int emo_ticks = -1;
static int emo_index = 0;

static int bg_cf_ticks = -1;
       int bg_cf_index = 0;

static int spk_ticks = -1;
static int spk_offset = -1;

       int ingame_tid = 100001;

	   char ingame_name[ 128] = u8" ";
	   char ingame_text[2048] = u8" ";
static char spk_buffer [2048] = "";

       Mix_Music* ingame_bgm;

character_t chr;

static text_h_t th = { -1, "" };

static char emo_buf[CHAR_BASE_MAX_PATH + CHAR_EMOTION_MAX_PATH] = "";

static char cg_buf[64];

       char* ingame_sel_text[10];
	   int   ingame_sel_offs[10];
bool ingame_sel_disp = false;
int ingame_sel_last = 0; // 0..9

static int ingame_sel_img_ids[10];
static int ingame_sel_txt_ids[10];

static int imx = 10;

static int spk_bg_hide_tick = 0;

void sc_ingame_text(void) {
	if (!(spk_ticks < 0 && spk_offset < 0)) return;

	memset(spk_buffer, 0, 2048);
	spk_offset = 0;
	spk_ticks = 0;
}

static void sc_ingame_emotion(void) {
	emotion_t emo = chr.emotions[emo_index];

	if (emo_ticks == 0) {
		sprintf(emo_buf, "%s/%s.png", chr.path, emo.path);
		printf("%s\n", emo.path);
		image_content(ingame_char, emo_buf);
	}

	// printf("%f\n", task_ticks > 15 ? ease_io_expo(1.0f - ((task_ticks - 15) / 15.0f)) : ease_io_expo(task_ticks / 15.0f));

	if (emo_ticks >= 1 && emo_ticks <= 22) {
		image_move(ingame_char, 0, (int)ceil(-4 * emo.movement_multiplier * ease_io_expo(emo_ticks / 22.0f)));
	}
	else if (emo_ticks >= 23 && emo_ticks <= 44) {
		image_move(ingame_char, 0, (int)floor(4 * emo.movement_multiplier * ease_io_expo(1.0f - ((emo_ticks - 23) / 22.0f))));
	}

	if (emo_ticks == 44) {
		// image_content(heroine, "image/heroine/neutral.png");
		emo_ticks = -2;
	}

	emo_ticks++;
}

static void sc_ingame_bg_cf(void) { // 배경 크로스페이드
	char bgf[64] = "";

	sprintf(bgf, "image/bg/%d.png", bg_cf_index);

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

static void sc_ingame_cg_show_task(void) { // 배경 크로스페이드
	if (cg_show_ticks == 0) {
		image_remove(cg_id);
		cg_id = -image_add(
			cg_buf, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 1.0f, 1.0f, H_CENTER, V_CENTER
		);
		image_alpha(cg_id, 0);
	}

	// printf("%f\n", task_ticks > 15 ? ease_io_expo(1.0f - ((task_ticks - 15) / 15.0f)) : ease_io_expo(task_ticks / 15.0f));

	if (cg_show_ticks >= 1 && cg_show_ticks <= 60) {
		float va = ease_io_cubic(cg_show_ticks / 60.0f);

		image_alpha(cg_id, (uint8_t)ceil(va * 255));
	}

	if (cg_show_ticks == 60) {
		cg_show_ticks = -2;
	}

	cg_show_ticks++;
}

static void sc_ingame_cg_hide_task(void) {
	if (cg_show_ticks >= 1 && cg_show_ticks <= 60) {
		float va = 1.0f - ease_io_cubic(cg_show_ticks / 60.0f);

		image_alpha(cg_id, (uint8_t)floor(va * 255));
	}

	if (cg_show_ticks == 60) {
		cg_show_ticks = -2;
	}

	cg_show_ticks++;
}

static void sc_ingame_char_show_task(void) { // 배경 크로스페이드
	if (char_show_ticks >= 1 && char_show_ticks <= 60) {
		float va = ease_io_cubic(char_show_ticks / 60.0f);

		image_alpha(ingame_char, (uint8_t)ceil(va * 255));
	}

	if (char_show_ticks == 60) {
		char_show_ticks = -2;
	}

	char_show_ticks++;
}

static void sc_ingame_char_hide_task(void) { // 배경 크로스페이드
	if (char_show_ticks >= 1 && char_show_ticks <= 60) {
		float va = 1.0f - ease_io_cubic(char_show_ticks / 60.0f);

		image_alpha(ingame_char, (uint8_t)floor(va * 255));
	}

	if (char_show_ticks == 60) {
		char_show_ticks = -2;
	}

	char_show_ticks++;
}

void sc_ingame_cg_show(short id) {
	sprintf(cg_buf, "image/cg/%d.png", id);
	cg_show_ticks = 0;
	fupdate_add(61, sc_ingame_cg_show_task);
}

void sc_ingame_cg_hide(void) {
	cg_show_ticks = 0;
	fupdate_add(61, sc_ingame_cg_hide_task);
}

void sc_ingame_cg_content(short id) {
	sprintf(cg_buf, "image/cg/%d.png", id);
	image_content(cg_id, cg_buf);
}

void sc_ingame_char_show(void) {
	char_show_ticks = 0;
	fupdate_add(61, sc_ingame_char_show_task);
}

void sc_ingame_char_hide(void) {
	char_show_ticks = 0;
	fupdate_add(61, sc_ingame_char_hide_task);
}

void sc_ingame_emote(int ei) {
	emo_ticks = 0;
	emo_index = ei;
	fupdate_add(45, sc_ingame_emotion);
}

void sc_ingame_bg_cf_start(void){
	bg_cf_ticks = 0;
	fupdate_add(61, sc_ingame_bg_cf);
}

static void sc_ingame_sel_highlight(void) {
	for (int i = 0; i < 10; i++) {
		image_alpha(ingame_sel_img_ids[i], i >= (5 - (imx / 2)) && i < (5 - (imx / 2)) + imx ? 128 : 0);
		text_color(ingame_sel_txt_ids[i], 255, 255, 255, i >= (5 - (imx / 2)) && i < (5 - (imx / 2)) + imx ? 128 : 0);
	}
	image_alpha(ingame_sel_img_ids[ingame_sel_last + (5 - (imx / 2))], 255);
	text_color(ingame_sel_txt_ids[ingame_sel_last + (5 - (imx / 2))], 255, 255, 255, 255);
}

static void sc_ingame_sel_clear(void) {
	for (int i = 0; i < 10; i++) {
		image_alpha(ingame_sel_img_ids[i], 0);
		text_content(ingame_sel_txt_ids[i], "");
	}
}

void sc_ingame_sel_disp(void) {
	ingame_sel_disp = true;

	for (int i = 0; i < 10; i++) {
		printf("%s\n", ingame_sel_text[i]);
		if (!ingame_sel_text[i]) {
			imx = i;
			break;
		}
	}

	/*for (int imax = 1; imax < 11; imax++) {
		printf("%2d: %d <= x < %d\n", imax, (5 - (imax / 2)), (5 - (imax / 2)) + imax);
	}*/

	for (int i = (5 - (imx / 2)); i < (5 - (imx / 2)) + imx; i++) {
		image_alpha(ingame_sel_img_ids[i], 255);
		text_content(ingame_sel_txt_ids[i], ingame_sel_text[i - (5 - (imx / 2))]);
	}

	ingame_sel_last = 0;
	sc_ingame_sel_highlight();
}

static void sc_ingame_initialize(void) {
	//printf("%s %s\n", name, text);
	//printf("START\n");

	bg_fill_color = (SDL_Color){ 0, 0, 0, 255 };

	sc_script = NULL;

	bg = -image_add(
		"image/bg/0.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		0.666667f, 0.666667f, H_CENTER, H_CENTER
	);
	bg_cf = -image_add(
		"image/bg/0.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		0.666667f, 0.666667f, H_CENTER, H_CENTER
	);

	image_alpha(bg_cf, 0);

	ingame_char = -image_add(
		"image/char/hibiki/neutral.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT,
		1.2f, 1.2f, H_CENTER, BOTTOM
	);
	image_alpha(ingame_char, 0);
	
	speak_bg = -image_add(
		"image/ui/text_bg.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT,
		1.0f, 1.0f, H_CENTER, BOTTOM
	);

	cg_id = -image_add(
		"image/cg/1.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		1.0f, 1.0f, H_CENTER, V_CENTER
	);
	image_alpha(cg_id, 0);

	speak_name = -text_add_as(
		"",
		GYEONGGIMILLENNIUMBATANGB,
		104, WINDOW_HEIGHT - 104,
		255, 255, 255, 255,
		0.8f, 0.8f, H_CENTER, V_CENTER
	);

	speak_content = -text_add_as(
		"",
		CAFE24SSUKSSUK,
		220, WINDOW_HEIGHT - 180,
		255, 255, 255, 255,
		0.5, 0.5, LEFT, TOP
	);

	for (int i = 0; i < 10; i++) {
		ingame_sel_img_ids[i] = -image_add(
			"image/ui/choice_bg.png",
			WINDOW_WIDTH / 2, 56 * i + 64,
			1.0f, 1.0f, H_CENTER, V_CENTER
		);
		ingame_sel_txt_ids[i] = -text_add_as(
			u8"",
			JANGMICHE,
			WINDOW_WIDTH / 2, 56 * i + 64 + 2,
			255, 255, 255, 255,
			0.5f, 0.5f, H_CENTER, V_CENTER
		);

		image_alpha(ingame_sel_img_ids[i], 0);
	}

	// Mix_PlayMusic(titlemusic, 1 << 30);

	sc_delay = 62;
	sc_exec_desire = true;
}

static void sc_ingame_render(void) {
	int res = 0;
	
	do {
		res = sc_exec();
	} while (res == 0);

	if (res == -1) {
		sc_save();
		// printf("TITLE DESIRE %d\n", sc_is_go_to_title());
		screen_change(sc_is_go_to_title() ? "title" : "ingame");
	}

	if (!(!ingame_name || !spk_buffer)) {
		if (strlen(ingame_name) == 0 && strlen(spk_buffer) == 0) {
			spk_bg_hide_tick++;

			if (spk_bg_hide_tick > 5) {
				image_alpha(speak_bg, 0);
			}
		}
		else {
			image_alpha(speak_bg, 255);
			spk_bg_hide_tick = 0;
		}
	}

	if (emo_ticks < 0 && input_is_keydown(SDLK_a)) {
		emo_ticks = 0;
		fupdate_add(45, sc_ingame_emotion);
	}

	if (bg_cf_ticks < 0 && input_is_keydown(SDLK_s)) {
		bg_cf_ticks = 0;
		fupdate_add(61, sc_ingame_bg_cf);
	}

	if (sc_delay == 65534 && input_is_keydown(SDLK_z)) {
		memset(ingame_name, 0, 128);
		memset(spk_buffer, 0, 2048);
		text_content(speak_name, "");
		text_content(speak_content, "");
		sc_delay = 0;
	}

	/// region SEL
	if (sc_delay == 65535) {
		if (input_is_keydown(SDLK_UP)) {
			puts("NEVER GONNA GIVE YOU UP");
			ingame_sel_last--;
			iclamp(&ingame_sel_last, 0, imx - 1);
			sc_ingame_sel_highlight();
		}
		else if (input_is_keydown(SDLK_DOWN)) {
			puts("GET DOWN");
			ingame_sel_last++;
			iclamp(&ingame_sel_last, 0, imx - 1);
			sc_ingame_sel_highlight();
		}
		else if (input_is_keydown(SDLK_z)) {
			puts("Z");

			reg = ingame_sel_last + 1;
			ingame_sel_disp = false;
			sc_delay = 0;

			sc_forcejump(ingame_sel_offs[ingame_sel_last]);

			memset(ingame_sel_text, 0, sizeof(char*) * 10);
			memset(ingame_sel_offs, 0, sizeof(int) * 10);
			
			sc_ingame_sel_clear();
		}
	}
	/// endregion

	if (spk_ticks-- == 0 && spk_offset < strlen(ingame_text)) {
		int cb = char_uni_bytes(ingame_text[spk_offset]);

		spk_offset += cb;
		if (ingame_text[spk_offset - 1] == '@') ingame_text[spk_offset - 1] = '\n';
		memcpy(spk_buffer, ingame_text, spk_offset);
		//printf("%d %c %s\n", spk_offset - 1, spk_buffer[spk_offset - 1], spk_buffer);

		text_content(speak_name, ingame_name);

		text_content(speak_content, spk_buffer);

		spk_ticks = cb;
	}
	else if (spk_ticks == 0 && spk_offset >= strlen(ingame_text)) {
		spk_offset = -1;
		spk_ticks = 0;

		sc_delay = 65534;
	}
}

static void sc_ingame_music_free(void) {
	Mix_FreeMusic(ingame_bgm);
	Mix_HookMusicFinished(NULL); // 개같이 버그 해결
	ingame_bgm = NULL;
}

static void sc_ingame_dispose(void) {
	Mix_FadeOutMusic(2000);

	Mix_HookMusicFinished(sc_ingame_music_free);
}

screen_t sc_ingame = {
	"ingame", sc_ingame_initialize, sc_ingame_render, sc_ingame_dispose
};
