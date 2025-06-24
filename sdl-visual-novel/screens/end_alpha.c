#define _CRT_SECURE_NO_WARNINGS

#include "end_alpha.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../engine/script.h"
#include "../engine/texthold.h"

#include "../util.h"

#include <SDL2/SDL_Mixer.h>

#include <time.h>

static int end_alpha_bg = 0;

static Mix_Music* music;

static void sc_end_alpha_music_free(void) {
	if (music) Mix_FreeMusic(music);
	Mix_HookMusicFinished(NULL);
	screen_change("title"); // 음악이 끝나면 타이틀로 되돌아가기
}

static void sc_end_alpha_initialize(void) {
	bg_fill_color = (SDL_Color) { 250, 250, 250, 255 };

	// int end_alpha_ltb = -image_add("image/bg/white.png", 0, 0, 12.8f, 7.2f, LEFT, TOP);
	end_alpha_bg = -image_add("image/ui/diary_bg.png", 0, 0, 1.0f, 1.0f, LEFT, TOP);

	music = Mix_LoadMUS("sound/bgm/amor.ogg");
	if (music == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
		return 1;
	}

	image_add("image/ui/end_alpha_pic.png", 180, 135, 475.0f / 1024, 475.0f / 1024, LEFT, TOP);
	
	text_add_as(
		u8"Fin", JANGMICHE, 975, 48, 0, 0, 0, 255, 0.5f, 0.5f, LEFT, TOP // ♪ 桐生 - Amor
	);
	text_add_as(
		u8"사용한 그림, 음악 등\n"
		u8" 그림\n"
		u8"  - ChatGPT (일기장, 엔딩)\n"
		u8"  - Unity Asset Store\n"
		u8" 글꼴\n"
		u8"  - 네이버 나눔글꼴, 클로바 손글씨\n"
		u8"  - Pretendard JP\n"
		u8"  - Spoqa Han Sans\n"
		u8"  - 카페24 쑥쑥\n"
		u8" 음악\n"
		u8"  ♪ 桐生 - Amor (엔딩 화면)\n"
		u8"  ♪ a_hisa - Cosmos (일기장, 단어장, 설정 화면)\n"
		u8"  ♪ a_hisa - Nite Nite (타이틀)\n"
		u8"  ♪ 이외 a_hisa 앨범 내 수록곡 다수\n"
		,
		JANGMICHE, 665, 136, 0, 0, 0, 255, 0.6125f, 0.6125f, LEFT, TOP
	);

	Mix_FadeInMusic(music, 0, 5000);
	Mix_HookMusicFinished(sc_end_alpha_music_free);
}

static void sc_end_alpha_render(void) {
	if (input_is_keydown(SDLK_x)) {
		screen_change("title");
	}
}

static void sc_end_alpha_dispose(void) {
	//Mix_FadeOutMusic(2000);

	//Mix_HookMusicFinished(sc_end_alpha_music_free);
}

screen_t sc_end_alpha = {
	"end_alpha", sc_end_alpha_initialize, sc_end_alpha_render, sc_end_alpha_dispose
};
