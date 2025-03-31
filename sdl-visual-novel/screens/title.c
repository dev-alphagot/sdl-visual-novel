#include "title.h"

#include "../wrapper/text.h"
#include "../wrapper/input.h"
#include "../wrapper/image.h"

#include "../engine/screen.h"
#include "../engine/character.h"
#include "../engine/texthold.h"

#include "../misc.h"
#include "../util.h"

#include <SDL2/SDL_Mixer.h>

#define OP_COUNT 5

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
	u8"환경설정",
	u8"종료"
};

static Mix_Music* titlemusic;

static void sc_title_op_highlight(void) {
	for (int i = 0; i < 5; i++) {
		image_alpha(op_img[i], 128);
		text_color(op_txt[i], 0, 0, 0, 128);
	}

	image_alpha(op_img[op_sel], 255);
	text_color(op_txt[op_sel], 0, 0, 0, 255);
}


static void sc_title_initialize(void) {
	bg_id = -image_add(
		"image/bg/title.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.666667f, 0.666667f, H_CENTER, V_CENTER
	);
	note_id = -image_add(
		"image/bg/note.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 1.0f, 1.0f, H_CENTER, V_CENTER
	);

	text_add_as(
		u8"タイトル名を\nここで入力", 
		COMBINED, 
		WINDOW_WIDTH / 2 - 12, WINDOW_HEIGHT / 2 - 128, 
		255, 255, 255, 255,
		0.8f, 0.8f, H_CENTER, V_CENTER
	);
	text_add_as(
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
	);
	ntId = -text_add_as(
		"",
		SPOQAHANSANSNEO,
		WINDOW_WIDTH / 2 - 20, WINDOW_HEIGHT / 1.3f,
		255, 255, 255, 0,
		0.5f, 0.5f, H_CENTER, V_CENTER
	); 

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

	// Mix_PlayMusic(titlemusic, 1 << 30);

	char_init();

	th_load();

#if VERBOSE
	th_display();
#endif

	Mix_FadeInMusic(titlemusic, 1 << 30, 5000);

	sc_title_op_highlight();
}

static void sc_title_render(void) {
	if (vTicks >= 60 && vTicks <= 111) {
		int a = (vTicks - 60) * 5;
		if (vTicks == 61) text_content(ntId, u8"Enter 키를 눌러 시작");
		text_color(ntId, 255, 255, 255, a);
	}

	if (input_is_keydown(SDLK_RETURN)) {
		screen_change("ingame");
	}

	if (input_is_keydown(SDLK_RSHIFT)) {
		screen_change("fliptest");
	}

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
			screen_change("ingame");
			break;
		case 1:
			screen_change("ingame");
			break;
		case 2:
			screen_change("wcoll");
			break;
		case 3:
			screen_change("config");
			break;
		case 4:
			quit = 1;
			break;
		}
	}

	vTicks++;
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
