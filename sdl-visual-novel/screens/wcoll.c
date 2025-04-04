#define _CRT_SECURE_NO_WARNINGS

#include "wcoll.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../engine/script.h"
#include "../engine/texthold.h"

#include "../util.h"

SDL_Texture* wcoll_bg_tex = NULL;
static int wcoll_bg_w = 0;
static int wcoll_bg_h = 0;

static int wcoll_top = 0;
static int wcoll_btm = 0;
static int wcoll_segment_txt = 0;

static int wcoll_count = 1;
static const char** wcoll_indices;

static int* wcoll_imgs;
static int* wcoll_txts;
static int* wcoll_mnns;
static int wcoll_ct_seg = 3;

static int wcoll_scroll_pos = 0;

static int wcoll_sel = 0;

static void sc_wcoll_keys_initialize(void) {
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
	sprintf(fb, "def/wcoll/%s.bin", wcoll_indices[wcoll_sel]);

	printf("%s\n", fb);
	FILE* wc = fopen(fb, "rb");

	fseek(wc, 0, SEEK_END);
	long wc_size = ftell(wc);
	fseek(wc, 0, SEEK_SET);

	wcoll_ct_seg = wc_size >> 2;

	free(wcoll_imgs);
	free(wcoll_txts);
	free(wcoll_mnns);

	wcoll_imgs = calloc(4, wcoll_ct_seg);
	wcoll_txts = calloc(4, wcoll_ct_seg);
	wcoll_mnns = calloc(4, wcoll_ct_seg);

	if (!wcoll_imgs || !wcoll_txts || !wcoll_mnns) return;

	for (int i = 0; i < wcoll_ct_seg; i++) {
		int id = 0;
		fread(&id, 4, 1, wc);

		if (sc_word_collected[id - 99800000]) {
			text_h_t txt = { -1, "" };
			th_search(id, &txt);

			char* tb = _strdup(txt.value);

			th_search(id + 100000, &txt);
			char* tm = _strdup(txt.value);

			wcoll_imgs[i] = -image_add_tex(wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h, WINDOW_WIDTH / 2, 173 + 60 * (i + 1), 1.0f, 1.0f, H_CENTER, BOTTOM);
			wcoll_txts[i] = -text_add_as(
				tb, COMBINED, 220, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
			);
			wcoll_mnns[i] = -text_add_as(
				tm, COMBINED, 450, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
			);
		}
		else {
			wcoll_imgs[i] = -image_add_tex(wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h, WINDOW_WIDTH / 2, 173 + 60 * (i + 1), 1.0f, 1.0f, H_CENTER, BOTTOM);
			wcoll_txts[i] = -text_add_as(
				u8"???", COMBINED, 220, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
			);
			wcoll_mnns[i] = -text_add_as(
				u8"아직 획득하지 못한 단어입니다.", COMBINED, 450, 195 + 60 * i, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
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

	int wcoll_ltb = -image_add("image/bg/white.png", 0, 0, 12.8f, 7.2f, LEFT, TOP);
	wcoll_top = -image_add("image/ui/wcoll_top.png", WINDOW_WIDTH / 2, 0, 1.0f, 1.0f, H_CENTER, TOP);

	image_alpha(wcoll_ltb, 250);

	wcoll_segment_txt = -text_add_as(
		u8"단어장을 뒤적거리는 학습자를 위한 안내서", GYEONGGIMILLENNIUMBATANGB, 185, 135, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);

	if (!wcoll_imgs || !wcoll_txts || !wcoll_mnns) return;

	for (int i = 0; i < 3; i++) {
		wcoll_imgs[i] = -image_add_tex(wcoll_bg_tex, wcoll_bg_w, wcoll_bg_h, WINDOW_WIDTH / 2, 173 + 60 * (i + 1), 1.0f, 1.0f, H_CENTER, BOTTOM);
	}

	wcoll_txts[0] = -text_add_as(
		u8"단어장의 페이지는 ← → 키를 이용해 넘길 수 있습니다.", COMBINED, 220, 135 + 60, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_txts[1] = -text_add_as(
		u8"↑ ↓ 키를 이용해 위아래로 스크롤할 수 있습니다.", COMBINED, 220, 135 + 60 * 2, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_txts[2] = -text_add_as(
		u8"X 키를 이용해 타이틀 화면으로 돌아갈 수 있습니다.", COMBINED, 220, 135 + 60 * 3, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);

	wcoll_mnns[0] = -text_add_as(
		u8"", COMBINED, 220, 135 + 60, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_mnns[1] = -text_add_as(
		u8"", COMBINED, 220, 135 + 60 * 2, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	wcoll_mnns[2] = -text_add_as(
		u8"", COMBINED, 220, 135 + 60 * 3, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);

	wcoll_btm = -image_add("image/ui/wcoll_bottom.png", WINDOW_WIDTH / 2, 173 + 180, 1.0f, 1.0f, H_CENTER, TOP);
}

static void sc_wcoll_render(void) {
	if (input_is_keydown(SDLK_LEFT)) {
		wcoll_sel--;
		iclamp(&wcoll_sel, 0, wcoll_count - 1);
		sc_wcoll_segment_update();
	}
	else if (input_is_keydown(SDLK_RIGHT)) {
		wcoll_sel++;
		iclamp(&wcoll_sel, 0, wcoll_count - 1);
		sc_wcoll_segment_update();
	}

	if (input_is_keydown(SDLK_UP)) {
		sc_wcoll_scroll(1);
	}
	else if (input_is_keydown(SDLK_DOWN)) {
		sc_wcoll_scroll(-1);
	}

	if (input_is_keydown(SDLK_x)) {
		screen_change("title");
	}
}

static void sc_wcoll_dispose(void) {}

screen_t sc_wcoll = {
	"wcoll", sc_wcoll_initialize, sc_wcoll_render, sc_wcoll_dispose
};
