#define _CRT_SECURE_NO_WARNINGS

#include "wcoll.h"

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../util.h"

static int wcoll_bg = 0;
static int wcoll_segment_txt = 0;

static int wcoll_indices_c = 1;
static const char** wcoll_indices;

static int wcoll_sel = 0;

static void sc_wcoll_keys_initialize(void) {
	FILE* indices = fopen("def/wcoll_index.txt", "rt");
	
	while (!feof(indices)) {
		if (fgetc(indices) == '\n') wcoll_indices_c++;
	}

	wcoll_indices = calloc(sizeof(const char*), wcoll_indices_c);
	fseek(indices, 0, SEEK_SET);

	if (!wcoll_indices) return;

	for (int i = 0; i < wcoll_indices_c; i++) {
		char ibuf[128] = "";
		fgets(ibuf, 128, indices);
		str_trim_lf(128, ibuf);
		wcoll_indices[i] = _strdup(ibuf);
	}

	for (int i = 0; i < wcoll_indices_c; i++) {
		printf("wc %s\n", wcoll_indices[i]);
	}
}

static void sc_wcoll_segment_update(void) {
	text_content(wcoll_segment_txt, wcoll_indices[wcoll_sel]);
}

static void sc_wcoll_initialize(void) {
	sc_wcoll_keys_initialize();

	int wcoll_ltb = -image_add("image/bg/white.png", 0, 0, 12.8f, 7.2f, LEFT, TOP);
	wcoll_bg = -image_add("image/bg/wcoll.png", WINDOW_WIDTH / 2, 0, 1.0f, 1.0f, H_CENTER, TOP);

	image_alpha(wcoll_ltb, 250);

	wcoll_segment_txt = -text_add_as(
		u8"단어장을 뒤적거리는 학습자를 위한 안내서", GYEONGGIMILLENNIUMBATANGB, 185, 135, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);

	text_add_as(
		u8"      단어장의 페이지는 ←→ 키를 이용해 넘길 수 있습니다.", COMBINED, 185, 135 + 60, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
	text_add_as(
		u8"      ↑↓ 키를 이용해 위아래로 스크롤할 수 있습니다.", COMBINED, 185, 135 + 60 * 2, 0, 0, 0, 255, 0.6f, 0.6f, LEFT, TOP
	);
}

static void sc_wcoll_render(void) {
	if (input_is_keydown(SDLK_LEFT)) {
		wcoll_sel--;
		iclamp(&wcoll_sel, 0, wcoll_indices_c - 1);
		sc_wcoll_segment_update();
	}
	else if (input_is_keydown(SDLK_RIGHT)) {
		wcoll_sel++;
		iclamp(&wcoll_sel, 0, wcoll_indices_c - 1);
		sc_wcoll_segment_update();
	}
}

static void sc_wcoll_dispose(void) {}

screen_t sc_wcoll = {
	"wcoll", sc_wcoll_initialize, sc_wcoll_render, sc_wcoll_dispose
};
