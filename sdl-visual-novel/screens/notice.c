#include "notice.h"

#include "../wrapper/text.h"

static int vTicks = 0;
static int ntId = 0;
static int ntI2 = 0;
static int ntI3 = 0;

static void sc_notice_initialize(void) {
	ntId = -text_add_as(
		u8"안내",
		PRETENDARDJPMEDIUM,
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 25,
		0, 0, 0, 0,
		0.75f, 0.75f, H_CENTER, V_CENTER
	);
	ntI2 = -text_add_as(
		u8"이 게임은 픽션이며, 등장하는 인물이나 단체 등은 아고입학되어 있습니다.", 
		PRETENDARDJPLIGHT, 
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 25, 
		0, 0, 0, 0,
		0.5f, 0.5f, H_CENTER, TOP
	);
	ntI3 = -text_add_as(
		u8"2025학년도 3학년 1학기 자몽주스 제출용",
		GYEONGGIMILLENNIUMBATANGB,
		8, WINDOW_HEIGHT - 8,
		0, 0, 0, 0,
		0.333333f, 0.333333f, LEFT, BOTTOM
	);
}

static void sc_notice_render(void) {
	if (vTicks <= 51) {
		int a = vTicks * 5;

		text_color(ntId, a, a, a, 254);
		text_color(ntI2, a, a, a, 254);
		text_color(ntI3, a, a, a, 254);
	}
	else if (vTicks >= 230 && vTicks <= 280) {
		int a = (281 - vTicks) * 5;

		text_color(ntId, a, a, a, 254);
		text_color(ntI2, a, a, a, 254);
		text_color(ntI3, a, a, a, 254);
	}
	else if (vTicks == 311) {
		screen_change("title");
	}

	vTicks++;
}

static void sc_notice_dispose(void) {}

screen_t sc_notice = {
	"notice", sc_notice_initialize, sc_notice_render, sc_notice_dispose
};
