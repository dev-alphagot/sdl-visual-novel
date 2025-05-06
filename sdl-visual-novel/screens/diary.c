#define _CRT_SECURE_NO_WARNINGS

#include "diary.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../engine/script.h"
#include "../engine/texthold.h"

#include "../util.h"

#include <SDL2/SDL_Mixer.h>

static int diary_bg = 0;
static int diary_painting = 0;
static int diary_segment_txt = 0;
static int diary_comment_txt = 0;
static int diary_cleared_txt = 0;
static int diary_wcount_txt = 0;

static int diary_count = 1;
static const char** diary_indices;

static char diary_cmt[1024] = "";

static int diary_wcoll_count = 0;

static int diary_sel = 0;

static bool diary_exiting = false;

static Mix_Music* music;

static Mix_Chunk* flip_sfx;

static void sc_diary_keys_initialize(void) {
	FILE* indices = fopen("def/wcoll_index.txt", "rt");
	
	while (!feof(indices)) {
		if (fgetc(indices) == '\n') diary_count++;
	}

	diary_indices = calloc(sizeof(const char*), diary_count);
	fseek(indices, 0, SEEK_SET);

	if (!diary_indices) return;

	for (int i = 0; i < diary_count; i++) {
		char ibuf[128] = "";
		fgets(ibuf, 128, indices);
		str_trim_lf(128, ibuf);
		diary_indices[i] = _strdup(ibuf);
	}

	for (int i = 0; i < diary_count; i++) {
		printf("wc %s\n", diary_indices[i]);
	}
}

static void sc_diary_lf_preprocess(void) {
	for (int i = 0; i < 1024; i++) {
		if (diary_cmt[i] == '@') diary_cmt[i] = '\n';
	}
}

static void sc_diary_segment_update(void) {
	Mix_PlayChannel(-1, flip_sfx, 0);

	static char fb[80] = "";
	sprintf(fb, "def/wcoll/%s.bin", diary_indices[diary_sel]);

	printf("%s\n", fb);
	FILE* wc = fopen(fb, "rb");

	fseek(wc, 0, SEEK_END);
	long wc_size = ftell(wc);
	fseek(wc, 0, SEEK_SET);

	diary_wcoll_count = wc_size >> 2;

	printf("%d\n", diary_wcoll_count);

	int wcr = 0;

	for (int i = 0; i < sc_scripts; i++) {
		if (!strcmp(diary_indices[diary_sel], sc_script_index_table[i])) {
			wcr = i; break;
		}
	}

	sprintf(fb, "image/diary/%s.png", diary_indices[diary_sel]);

	image_content(diary_painting, fb);

	static int ww = 0;

	for (int i = 0; i < diary_wcoll_count; i++) {
		int id = 0;
		fread(&id, 4, 1, wc);

		if (sc_word_collected[id - 99800000]) ww++;
	}

	sprintf(fb, "단어 %d / %d개 획득", ww, diary_wcoll_count);
	text_content(diary_wcount_txt, fb);

	text_h_t th = { -1, "" };
	th_search(99700000 + wcr, &th);
	text_content(diary_segment_txt, th.value);

	printf("wcr %d\n", wcr);

	//th_display();

	th_search(99600000 + wcr, &th);
	strncpy(diary_cmt, th.value, 1024);
	sc_diary_lf_preprocess();
	text_content(diary_comment_txt, diary_cmt);
}

static void sc_diary_initialize(void) {
	flip_sfx = Mix_LoadWAV("sound/se/flip.wav");
	Mix_VolumeChunk(flip_sfx, vol_sfx);

	bg_fill_color = (SDL_Color) { 250, 250, 250, 255 };

	diary_exiting = false;

	sc_diary_keys_initialize();

	int scc = 7;

	// int diary_ltb = -image_add("image/bg/white.png", 0, 0, 12.8f, 7.2f, LEFT, TOP);
	diary_bg = -image_add("image/ui/diary_bg.png", 0, 0, 1.0f, 1.0f, LEFT, TOP);
	diary_painting = -image_add("image/diary/0305.png", 190, 135, 1.0f, 1.0f, LEFT, TOP);

	image_alpha(diary_painting, 250);
	image_color(diary_painting, 250, 250, 250);

	// image_alpha(diary_ltb, 250);

	diary_wcoll_count = scc;

	int kg = -text_add_as(
		u8"⇄\n키\n로\n\n넘\n기\n기",
		SPOQAHANSANSBOLD,
		34, 8,
		20, 20, 20, 255,
		0.333333f, 0.333333f, LEFT, TOP
	);
	int kg_bg = -image_add("image/ui/key_guide_bg.png", 20, 0, 1.0f, 1.0f, LEFT, TOP);
	if (!st_key_guide) {
		text_content(kg, "");
		image_alpha(kg_bg, 0);
	}

	diary_segment_txt = -text_add_as(
		u8"3월 25일", JANGMICHE, 975, 48, 0, 0, 0, 255, 0.5f, 0.5f, LEFT, TOP
	);
	diary_comment_txt = -text_add_as(
		u8"아직까지도 청년들이 이 소설에 공감한다는 사실이\n괴로워. 요즘의 청년들이 이런 이야기에\n공감하지 못하고 그냥 옛날이야기라고\n생각하길 바랐어.\n\n사람들은 아버지를 난장이라고 불렀다.\n兄さんは 理想主義者だよ。", 
		JANGMICHE, 675, 136, 0, 0, 0, 255, 0.6125f, 0.6125f, LEFT, TOP
	);
	diary_cleared_txt = -text_add_as(
		u8"1989년 6월 4일 클리어",
		JANGMICHE, 675, 543, 0, 0, 0, 255, 0.6125f, 0.6125f, LEFT, TOP
	);
	diary_wcount_txt = -text_add_as(
		u8"단어 3 / 25개 획득",
		JANGMICHE, 675, 578, 0, 0, 0, 255, 0.6125f, 0.6125f, LEFT, TOP
	);

	music = Mix_LoadMUS("sound/bgm/cosmos.ogg");
	if (music == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
		return 1;
	}

	Mix_FadeInMusic(music, 1 << 30, 5000);
}

static void sc_diary_render(void) {
	static int prev_sel = 0;

	if (input_is_keydown(SDLK_LEFT) && !diary_exiting) {
		diary_sel--;
		iclamp(&diary_sel, 0, diary_count - 1);

		if (diary_sel != prev_sel) {
			prev_sel = diary_sel;
			sc_diary_segment_update();
		}
	}
	else if (input_is_keydown(SDLK_RIGHT) && !diary_exiting) {
		diary_sel++;
		iclamp(&diary_sel, 0, diary_count - 1);

		if (diary_sel != prev_sel) {
			prev_sel = diary_sel;
			sc_diary_segment_update();
		}
	}

	if (input_is_keydown(SDLK_x)) {
		diary_exiting = true;
		screen_change("title");
	}
}

static void sc_diary_music_free(void) {
	if (music) Mix_FreeMusic(music);
	Mix_HookMusicFinished(NULL);
}

static void sc_diary_dispose(void) {
	Mix_FreeChunk(flip_sfx);

	for (int i = 0; i < diary_count; i++) {
		free(diary_indices[i]);
	}
	free(diary_indices);

	Mix_FadeOutMusic(2000);

	Mix_HookMusicFinished(sc_diary_music_free);
}

screen_t sc_diary = {
	"diary", sc_diary_initialize, sc_diary_render, sc_diary_dispose
};
