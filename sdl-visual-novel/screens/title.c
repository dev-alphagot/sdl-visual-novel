#include "title.h"

#include "../wrapper/text.h"

static int vTicks = 0;
static int ntId = 0;

static void sc_title_initialize(void) {
	ntId = -text_add_as(
		u8"タイトル名をここで入力", 
		COMBINED, 
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 
		0, 0, 0, 255,
		1.0f, 1.0f, H_CENTER, V_CENTER
	);
}

static void sc_title_render(void) {
	if (vTicks >= 60 && vTicks <= 111) {
		int a = (vTicks - 60) * 5;
		ntId = -text_color(ntId, a, a, a, 255);
	}

	vTicks++;
}

static void sc_title_dispose(void) {}

screen_t sc_title = {
	"title", sc_title_initialize, sc_title_render, sc_title_dispose
};
