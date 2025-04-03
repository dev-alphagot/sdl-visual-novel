#define _CRT_SECURE_NO_WARNINGS

#include "title.h"

#include "../wrapper/text.h"
#include "../wrapper/input.h"
#include "../wrapper/image.h"

#include "../engine/screen.h"
#include "../engine/character.h"
#include "../engine/texthold.h"
#include "../engine/script.h"

#include "../misc.h"
#include "../util.h"

#include <SDL2/SDL_Mixer.h>
#include <time.h>

#define OP_COUNT 4
#define RYUGU 1770 // 용궁으로 보내는 위치 상수

static int vTicks = 0;
static int ntId = 0;

static int bg_id = 0;
static int note_id = 0;

static int op_img[OP_COUNT];
static int op_txt[OP_COUNT];
static int op_sel = 0;
static const char* op_ctt[OP_COUNT] = {
	u8"처음부터",
	u8"이어서",
	u8"단어장",
	u8"종료"
};

static int modal_bg = 0;
static int modal_text = 0;
static bool modal = false;

static Mix_Music* titlemusic;

static void sc_title_op_highlight(void) {
	for (int i = 0; i < OP_COUNT; i++) {
		image_alpha(op_img[i], 128);
		text_color(op_txt[i], 0, 0, 0, 128);
	}

	image_alpha(op_img[op_sel], 255);
	text_color(op_txt[op_sel], 0, 0, 0, 255);
}

static void sc_title_modal_on(void) {
	static char sss[1536] = "";

	struct tm* tw;
	tw = localtime(&sc_save_last);

	text_h_t th = { -1, "" };
	th_search(sc_index_current + 99700000, &th);

	sprintf(sss, 
		u8"이전에 플레이하던 데이터가 있습니다.\n%d년 %d월 %d일 %d시 %d분에 마지막으로 플레이하였으며,\n%s까지 진행했습니다.\n\n플레이 데이터를 초기화하고 계속하려면 Z 키를,\n돌아가려면 X 키를 눌러주세요."
		, tw->tm_year + 1900, tw->tm_mon + 1, tw->tm_mday, tw->tm_hour, tw->tm_min, th.value);

	modal = true;
	image_alpha(modal_bg, 240);
	text_content(modal_text, sss);
	text_move(ntId, 0, RYUGU);
	text_move(modal_text, 0, -RYUGU);
}

static void sc_title_modal_off(void) {
	modal = false;
	image_alpha(modal_bg, 0);
	text_move(modal_text, 0, RYUGU);
	text_move(ntId, 0, -RYUGU);
}

static void sc_title_initialize(void) {
	bg_id = -image_add(
		"image/bg/title.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.666667f, 0.666667f, H_CENTER, V_CENTER
	);
	note_id = -image_add(
		"image/bg/note.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 1.0f, 1.0f, H_CENTER, V_CENTER
	);

	ntId = -text_add_as(
		u8"タイトル名を\nここで入力", 
		COMBINED, 
		WINDOW_WIDTH / 2 - 12, WINDOW_HEIGHT / 2 + 12, 
		0, 0, 0, 255,
		0.8f, 0.8f, H_CENTER, V_CENTER
	);
	/*text_add_as(
		u8"♪ 77o44birthdayzero @ mamomo",
		SPOQAHANSANSNEO,
		8, WINDOW_HEIGHT - 8,
		254, 254, 254, 254,
		0.35f, 0.35f, LEFT, BOTTOM
	);
	text_add_as(
		u8"responsive/milestone",
		SPOQAHANSANSNEO,
		WINDOW_WIDTH - 8, WINDOW_HEIGHT - 8,
		254, 254, 254, 254,
		0.35f, 0.35f, RIGHT, BOTTOM
	);*/

	SDL_Rect rc = { 0 };
	text_get_rect(ntId, &rc);

	image_add("image/bg/white.png", WINDOW_WIDTH / 2 - 12, WINDOW_HEIGHT / 2 + 12, rc.w / 100.0f + 0.16f, rc.h / 100.0f + 0.16f, H_CENTER, V_CENTER);

	for (int i = 0; i < OP_COUNT; i++) {
		op_img[i] = -image_add(
			"image/ui/selection_bg.png", 847, 64 + (i * 40), 0.666667f, 0.666667f, LEFT, V_CENTER
		);
		op_txt[i] = -text_add_as(
			op_ctt[i],
			COMBINED,
			855, 66 + (i * 40),
			0, 0, 0, 255,
			0.35f, 0.35f, LEFT, V_CENTER
		);
	}

	titlemusic = Mix_LoadMUS("sound/bgm/77o44birthdayzero.ogg");
	if (titlemusic == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
		return 1;
	}

	modal_bg = -image_add(
		"image/bg/white.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 6.4f, 3.0f, H_CENTER, V_CENTER
	);
	modal_text = -text_add_as(
		u8"다람쥐 헌 쳇바퀴에 타고파.",
		NANUMBARUNGOTHIC,
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + RYUGU,
		0, 0, 0, 255,
		0.5f, 0.5f, H_CENTER, V_CENTER
	);

	image_alpha(modal_bg, 0);

	// Mix_PlayMusic(titlemusic, 1 << 30);

	char_init();

	th_load();

	sc_init();

#if VERBOSE
	th_display();
#endif

	Mix_FadeInMusic(titlemusic, 1 << 30, 5000);

	sc_title_op_highlight();
}

static void sc_title_render(void) {
	if (modal) {
		if (input_is_keydown(SDLK_z)) {
			sc_reset();
			screen_change("ingame");
		}
		else if (input_is_keydown(SDLK_x)) {
			sc_title_modal_off();
		}
	}
	else {
		if (input_is_keydown(SDLK_UP)) {
			op_sel--;
			iclamp(&op_sel, 0, OP_COUNT - 1);
			sc_title_op_highlight();
		}
		else if (input_is_keydown(SDLK_DOWN)) {
			op_sel++;
			iclamp(&op_sel, 0, OP_COUNT - 1);
			sc_title_op_highlight();
		}
		else if (input_is_keydown(SDLK_z)) {
			printf("op_sel %d\n", op_sel);

			switch (op_sel) {
			case 0:
				if (sc_save_last) {
					sc_title_modal_on();
				}
				else {
					screen_change("ingame");
				}
				break;
			case 1:
				screen_change("ingame");
				break;
			case 2:
				screen_change("wcoll");
				break;
			case 3:
				quit = 1;
				break;
			}
		}
	}
}

static void sc_title_music_free(void) {
	Mix_FreeMusic(titlemusic);
	Mix_HookMusicFinished(NULL);
}

static void sc_title_dispose(void) {
	Mix_FadeOutMusic(2000);
	
	Mix_HookMusicFinished(sc_title_music_free);
}

screen_t sc_title = {
	"title", sc_title_initialize, sc_title_render, sc_title_dispose
};
