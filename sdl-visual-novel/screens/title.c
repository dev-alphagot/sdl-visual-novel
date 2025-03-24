#include "title.h"

#include "../wrapper/text.h"
#include "../wrapper/input.h"

#include "../engine/screen.h"

#include <SDL2/SDL_Mixer.h>

static int vTicks = 0;
static int ntId = 0;

static Mix_Music* titlemusic;

static void sc_title_initialize(void) {
	ntId = -text_add_as(
		u8"タイトル名をここで入力", 
		COMBINED, 
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 
		0, 0, 0, 254,
		1.0f, 1.0f, H_CENTER, V_CENTER
	);
	text_add_as(
		u8"♪ 77o44birthdayzero @ mamomo",
		SPOQAHANSANSNEO,
		8, WINDOW_HEIGHT - 8,
		254, 254, 254, 254,
		0.333333f, 0.333333f, LEFT, BOTTOM
	);
	text_add_as(
		u8"responsive/milestone",
		SPOQAHANSANSNEO,
		WINDOW_WIDTH - 8, WINDOW_HEIGHT - 8,
		254, 254, 254, 254,
		0.333333f, 0.333333f, RIGHT, BOTTOM
	);
	text_add_as(
		u8"Enter 키를 눌러 시작",
		SPOQAHANSANSNEO,
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 1.5f,
		254, 254, 254, 254,
		0.5f, 0.5f, H_CENTER, V_CENTER
	);

	titlemusic = Mix_LoadMUS("sound/bgm/77o44birthdayzero.ogg");
	if (titlemusic == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
		return 1;
	}

	// Mix_PlayMusic(titlemusic, 1 << 30);

	Mix_FadeInMusic(titlemusic, 1 << 30, 5000);
}

static void sc_title_render(void) {
	if (vTicks >= 60 && vTicks <= 111) {
		int a = (vTicks - 60) * 5;
		ntId = -text_color(ntId, a, a, a, 254);
	}

	if (input_is_keydown(SDLK_RETURN)) {
		puts("fc\n");
		screen_change("ingame");
	}

	vTicks++;
}

static void sc_title_music_free(void) {
	Mix_FreeMusic(titlemusic);
}

static void sc_title_dispose(void) {
	Mix_FadeOutMusic(2000);
	
	Mix_HookMusicFinished(sc_title_music_free);
}

screen_t sc_title = {
	"title", sc_title_initialize, sc_title_render, sc_title_dispose
};
