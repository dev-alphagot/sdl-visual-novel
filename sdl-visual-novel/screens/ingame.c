#include "ingame.h"

#include "../util.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../engine/fupdate.h"

#include <SDL2/SDL_Mixer.h>

static int ntId = 0;

static int bg = 0;
static int heroine = 0;

static int speak_bg = 0;
static int speak_name = 0;
static int speak_content = 0;

static int task_ticks = -1;

static int spk_ticks = -1;
static int spk_offset = -1;

char* name = u8"猫塚ヒビキ";
char* text = u8"サンタクロースさんが たった一晩でキヴォトスの 子供たちにプレゼントを渡す\nためには… 彼のトナカイは最低 マッハ4以上の速度を出さなきゃ いけないはず。\n키스의 고유 조건은 입술끼리 만나야 하고 특별한 기술은 필요치 않다.";
static char  spk_buffer[1024] = "";

static Mix_Music* bgm;

static void sc_ingame_angry_task(void) {
	if(task_ticks == 0) image_content(heroine, "image/heroine/angry c.png");

	if (task_ticks == 180) {
		image_content(heroine, "image/heroine/neutral.png");
		task_ticks = -2;
	}

	task_ticks++;
}

static void sc_ingame_initialize(void) {
	bgm = Mix_LoadMUS("sound/bgm/dotabata.ogg");
	if (bgm == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
	}

	bg = -image_add(
		"image/bg/Stairs Day.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		0.666667f, 0.666667f, H_CENTER, H_CENTER
	);

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
		1.0f, 1.0f, H_CENTER, V_CENTER
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
	if (task_ticks < 0 && input_is_keydown(SDLK_a)) {
		task_ticks = 0;
		fupdate_add(181, sc_ingame_angry_task);
	}

	if (spk_ticks < 0 && spk_offset < 0 && input_is_keydown(SDLK_z)) {
		spk_offset = 0;
		spk_ticks = 0;

		name = u8"猫冢 响";
	}

	if (spk_ticks-- == 0 && spk_offset < strlen(text)) {
		int cb = char_uni_bytes(text[spk_offset]);

		spk_offset += cb;
		memcpy(spk_buffer, text, spk_offset);

		text_content(speak_name, name);

		text_content(speak_content, spk_buffer);

		spk_ticks = cb;
	}
}

static void sc_ingame_dispose(void) {
	Mix_FreeMusic(bgm);
}

screen_t sc_ingame = {
	"ingame", sc_ingame_initialize, sc_ingame_render, sc_ingame_dispose
};
