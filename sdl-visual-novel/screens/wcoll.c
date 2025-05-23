// #define _CRT_SECURE_NO_WARNINGS

#include "wcoll.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../engine/script.h"
#include "../engine/texthold.h"

#include "../util.h"

#include <SDL2/SDL_Mixer.h>

SDL_Texture* wcoll_bg_tex = NULL;
static int wcoll_bg_w = 0;
static int wcoll_bg_h = 0;

static int wcoll_top = 0;
static int wcoll_btm = 0;
static int wcoll_segment_txt = 0;

static int wcoll_count = 1;
static const char** wcoll_indices;

static int* wcoll_imgs = NULL;
static int* wcoll_txts = NULL;
static int* wcoll_mnns = NULL;
static int wcoll_ct_seg = 3;

static int wcoll_scroll_pos = 0;

static int wcoll_sel = -1;
static int wcoll_prev_sel = -1;

static bool wcoll_exiting = false;

static Mix_Music* music;

static Mix_Chunk* flip_sfx;

static void sc_wcoll_keys_initialize(void) {
	wcoll_count = 1;

	FILE* indices = fopen("def/wcoll_index.txt", "rt");
	
	while (!feof(indices)) {
		if (fgetc(indices) == '\n') wcoll_count++;
	}

	wcoll_indices = calloc(sizeof(const char*), wcoll_count);
	fseek(indices, 0, SEEK_SET);

	if (!wcoll_indices) return;

	for (int i = 0; i < wcoll_count; i++) {
		char ibuf[128] = "";
		fgets(ibuf, 128, indices);
		str_trim_lf(128, ibuf);
		wcoll_indices[i] = _strdup(ibuf);
	}

	for (int i = 0; i < wcoll_count; i++) {
		printf("wc %s\n", wcoll_indices[i]);
	}
}

// mod: 1 or -1 (-1이면 끌어내림, 1이면 올림)
static void sc_wcoll_scroll(int mod) {
	SDL_Rect rcb = { 0 };
	SDL_Rect rct = { 0 };

	image_get_rect(wcoll_top, &rct);
	image_get_rect(wcoll_btm, &rcb);

	if (rct.y == 0 && mod == 1) return;
	if (rcb.y + 173 <= WINDOW_HEIGHT && mod == -1) return;

	image_move(wcoll_top, 0, 60 * mod);
	image_move(wcoll_btm, 0, 60 * mod);
	text_move(wcoll_segment_txt, 0, 60 * mod);

	for (int i = 0; i < wcoll_ct_seg; i++) {
		image_move(wcoll_imgs[i], 0, 60 * mod);
		text_move(wcoll_txts[i], 0, 60 * mod);
		text_move(wcoll_mnns[i], 0, 60 * mod);
	}

	wcoll_scroll_pos += mod;
}

static void sc_wcoll_segment_update(void) {
	Mix_PlayChannel(-1, flip_sfx, 0);

	printf("%p %d %d\n", wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h);

	if (!wcoll_imgs || !wcoll_txts || !wcoll_mnns) return;

	for (int i = 0; i < wcoll_ct_seg; i++) {
		image_remove_nofree(wcoll_imgs[i]);
		text_remove(wcoll_txts[i]);
		text_remove(wcoll_mnns[i]);
	}

	wcoll_scroll_pos = 0;

	image_pos(wcoll_top, WINDOW_WIDTH / 2, 0);
	text_pos(wcoll_segment_txt, 185, 135);

	static char fb[80] = "";
	sprintf_s(fb, 80, "def/wcoll/%s.bin", wcoll_indices[wcoll_sel]);

	printf("%s\n", fb);
	FILE* wc = fopen(fb, "rb");

	fseek(wc, 0, SEEK_END);
	long wc_size = ftell(wc);
	fseek(wc, 0, SEEK_SET);

	wcoll_ct_seg = wc_size >> 2;

	printf("%d\n", wcoll_ct_seg);

	free(wcoll_imgs);
	free(wcoll_txts);
	free(wcoll_mnns);

	wcoll_imgs = calloc(4, wcoll_ct_seg);
	wcoll_txts = calloc(4, wcoll_ct_seg);
	wcoll_mnns = calloc(4, wcoll_ct_seg);

	if (!wcoll_imgs || !wcoll_txts || !wcoll_mnns) return;

	for (int i = 0; i < wcoll_ct_seg; i++) {
		int id = 0;
		fread_s(&id, 4, 4, 1, wc);

		if (sc_word_collected[id - 99800000]) {
			text_h_t txt = { -1, "" };
			th_search(id, &txt);

			char* tb = _strdup(txt.value);

			th_search(id + 100000, &txt);
			char* tm = _strdup(txt.value);

			wcoll_imgs[i] = -image_add_tex(wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h, WINDOW_WIDTH / 2, 173 + 60 * (i + 1), 1.0f, 1.0f, H_CENTER, BOTTOM);
			wcoll_txts[i] = -text_add_as(
				tb, CAFE24SSUKSSUK, 220, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
			);
			wcoll_mnns[i] = -text_add_as(
				tm, CAFE24SSUKSSUK, 600, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
			);
		}
		else {
			wcoll_imgs[i] = -image_add_tex(wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h, WINDOW_WIDTH / 2, 173 + 60 * (i + 1), 1.0f, 1.0f, H_CENTER, BOTTOM);
			wcoll_txts[i] = -text_add_as(
				u8"???", CAFE24SSUKSSUK, 220, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
			);
			wcoll_mnns[i] = -text_add_as(
				u8"아직 획득하지 못한 단어입니다.", CAFE24SSUKSSUK, 600, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
			);
		}
	}

	image_pos(wcoll_btm, WINDOW_WIDTH / 2, 173 + wcoll_ct_seg * 60);

	int wcr = 0;

	for (int i = 0; i < sc_scripts; i++) {
		if (!strcmp(wcoll_indices[wcoll_sel], sc_script_index_table[i])) {
			wcr = i; break;
		}
	}

	text_h_t th = { -1, "" };
	th_search(99700000 + wcr, &th);

	text_content(wcoll_segment_txt, th.value);
}

static void sc_wcoll_initialize(void) {
	flip_sfx = Mix_LoadWAV("sound/se/flip.wav");
	Mix_VolumeChunk(flip_sfx, vol_sfx);

	bg_fill_color = (SDL_Color) { 250, 250, 250, 255 };

	wcoll_exiting = false;

	sc_wcoll_keys_initialize();

	SDL_Surface* imageSurface;
	if (!(imageSurface = IMG_Load("image/ui/wcoll_w_bg.png"))) return;

	if (!(wcoll_bg_tex = SDL_CreateTextureFromSurface(renderer, imageSurface))) return;

	wcoll_bg_w = imageSurface->w;
	wcoll_bg_h = imageSurface->h;

	printf("%p %d %d\n", wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h);

	SDL_FreeSurface(imageSurface);

	wcoll_imgs = calloc(4, 3);
	wcoll_txts = calloc(4, 3);
	wcoll_mnns = calloc(4, 3);

	// int wcoll_ltb = -image_add("image/bg/white.png", 0, 0, 12.8f, 7.2f, LEFT, TOP);
	wcoll_top = -image_add("image/ui/wcoll_top.png", WINDOW_WIDTH / 2, 0, 1.0f, 1.0f, H_CENTER, TOP);

	// image_alpha(wcoll_ltb, 250);

	wcoll_ct_seg = 3;

	int kg = -text_add_as(
		u8"⇄ ⇅\n키 키\n로 로\n\n넘 스\n기 크\n기 롤",
		SPOQAHANSANSBOLD,
		25, 8,
		20, 20, 20, 255,
		0.333333f, 0.333333f, LEFT, TOP
	);
	int kg_bg = -image_add("image/ui/key_guide_bg.png", 20, 0, 1.0f, 1.0f, LEFT, TOP);
	if (!st_key_guide) {
		text_content(kg, "");
		image_alpha(kg_bg, 0);
	}

	wcoll_segment_txt = -text_add_as(
		u8"단어장을 뒤적거리는 학습자를 위한 안내서", PRETENDARDJPMEDIUM, 185, 135, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);

	if (!wcoll_imgs || !wcoll_txts || !wcoll_mnns) return;

	for (int i = 0; i < 3; i++) {
		wcoll_imgs[i] = -image_add_tex(wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h, WINDOW_WIDTH / 2, 173 + 60 * (i + 1), 1.0f, 1.0f, H_CENTER, BOTTOM);
	}

	wcoll_txts[0] = -text_add_as(
		u8"단어장의 페이지는 ← → 키를 이용해 넘길 수 있습니다.", CAFE24SSUKSSUK, 220, 135 + 60, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_txts[1] = -text_add_as(
		u8"↑ ↓ 키를 이용해 위아래로 스크롤할 수 있습니다.", CAFE24SSUKSSUK, 220, 135 + 60 * 2, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_txts[2] = -text_add_as(
		u8"X 키를 이용해 타이틀 화면으로 돌아갈 수 있습니다.", CAFE24SSUKSSUK, 220, 135 + 60 * 3, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);

	wcoll_mnns[0] = -text_add_as(
		u8"", CAFE24SSUKSSUK, 220, 135 + 60, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_mnns[1] = -text_add_as(
		u8"", CAFE24SSUKSSUK, 220, 135 + 60 * 2, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_mnns[2] = -text_add_as(
		u8"", CAFE24SSUKSSUK, 220, 135 + 60 * 3, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);

	wcoll_btm = -image_add("image/ui/wcoll_bottom.png", WINDOW_WIDTH / 2, 173 + 180, 1.0f, 1.0f, H_CENTER, TOP);

	music = Mix_LoadMUS("sound/bgm/cosmos.ogg");
	if (music == NULL) {
		fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
		return 1;
	}

	wcoll_sel = -1;
	wcoll_prev_sel = -1;

	Mix_FadeInMusic(music, 1 << 30, 5000);
}

static void sc_wcoll_render(void) {
	if (input_is_keydown(SDLK_LEFT) && !wcoll_exiting) {
		wcoll_sel--;
		iclamp(&wcoll_sel, 0, wcoll_count - 1);

		if (wcoll_sel != wcoll_prev_sel) {
			wcoll_prev_sel = wcoll_sel;
			sc_wcoll_segment_update();
		}
	}
	else if (input_is_keydown(SDLK_RIGHT) && !wcoll_exiting) {
		wcoll_sel++;
		iclamp(&wcoll_sel, 0, wcoll_count - 1);

		if (wcoll_sel != wcoll_prev_sel) {
			wcoll_prev_sel = wcoll_sel;
			sc_wcoll_segment_update();
		}
	}

	if (input_is_keydown(SDLK_UP) && !wcoll_exiting) {
		sc_wcoll_scroll(1);
	}
	else if (input_is_keydown(SDLK_DOWN) && !wcoll_exiting) {
		sc_wcoll_scroll(-1);
	}

	if (input_is_keydown(SDLK_x)) {
		wcoll_exiting = true;
		screen_change("title");
	}
}

static void sc_wcoll_music_free(void) {
	if (music) Mix_FreeMusic(music);
	Mix_HookMusicFinished(NULL);
}

static void sc_wcoll_dispose(void) {
	Mix_FreeChunk(flip_sfx);

	SDL_DestroyTexture(wcoll_bg_tex);

	for (int i = 0; i < wcoll_count; i++) {
		free(wcoll_indices[i]);
	}
	free(wcoll_indices);

	free(wcoll_imgs);
	free(wcoll_txts);
	free(wcoll_mnns);

	wcoll_imgs = NULL;
	wcoll_txts = NULL;
	wcoll_mnns = NULL;

	Mix_FadeOutMusic(2000);

	Mix_HookMusicFinished(sc_wcoll_music_free);
}

screen_t sc_wcoll = {
	"wcoll", sc_wcoll_initialize, sc_wcoll_render, sc_wcoll_dispose
};
