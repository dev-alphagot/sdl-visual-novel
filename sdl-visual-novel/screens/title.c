#include "title.h"

#include "../wrapper/text.h"

static int vTicks = 0;
static int ntId = 0;

static void sc_title_initialize(void) {
	ntId = -text_add_as(
		u8"타이틀 화면 만들기 귀찮은데", 
		GYEONGGIMILLENNIUMBATANGB, 
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 
		255, 255, 255, 0, 
		H_CENTER, V_CENTER, 0.5f, 0.5f
	);
}

static void sc_title_render(void) {
	if (vTicks <= 51) {
		ntId = -text_color(ntId, 255, 255, 255, vTicks * 5);
	}

	vTicks++;
}

static void sc_title_dispose(void) {}

screen_t sc_title = {
	"notice", sc_title_initialize, sc_title_render, sc_title_dispose
};
