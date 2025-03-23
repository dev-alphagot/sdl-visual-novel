#include "notice.h"

#include "../wrapper/text.h"

static int vTicks = 0;
static int ntId = 0;

static void sc_notice_initialize(void) {
	ntId = -text_add_as(
		u8"경고\n\n이 게임은 픽션이며, 등장하는 인물이나 단체 등은 아고입학되어 있습니다.", 
		SPOQAHANSANSNEO, 
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 
		255, 255, 255, 0,
		H_CENTER, V_CENTER, 0.5f, 0.5f
	);
}

static void sc_notice_render(void) {
	if (vTicks <= 51) {
		ntId = -text_color(ntId, 255, 255, 255, vTicks * 5);
	}
	else if (vTicks >= 230 && vTicks <= 281) {
		ntId = -text_color(ntId, 255, 255, 255, (281 - vTicks) * 5);
	}
	else if (vTicks > 311) {
		screen_change("title");
	}

	vTicks++;
}

static void sc_notice_dispose(void) {}

screen_t sc_notice = {
	"notice", sc_notice_initialize, sc_notice_render, sc_notice_dispose
};
