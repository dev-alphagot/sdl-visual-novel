#define _CRT_SECURE_NO_WARNINGS

#include "fliptest.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <SDL2/SDL_Mixer.h>
#include <stdbool.h>

#include "../wrapper/text.h"
#include "../wrapper/image.h"
#include "../wrapper/input.h"

#include "../window.h"

static int txt_lt;

static int anmTick = -1;

static int wImgIds[10];
static int wTxtIds[10];

static void sc_fliptest_initialize(void) {
	image_add(
		"image/ui/page_image.png",
		WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
		3.2f, 3.2f, H_CENTER, V_CENTER
	);

    txt_lt = -text_add_as(
        u8"숫자 키로 선택",
        GOORMSANS,
        8, 8,
        255, 255, 255, 255,
        0.25f, 0.25f, LEFT, TOP
    );

	int imx = 10;

    for (int i = 0; i < imx; i++) {
		wImgIds[i] = -image_add(
			"image/ui/choice_bg.png",
			WINDOW_WIDTH / 2, 56 * i + 64,
			1.0f, 1.0f, H_CENTER, V_CENTER
		);
		wTxtIds[i] = -text_add_as(
			"선택지",
			COMBINED,
			WINDOW_WIDTH / 2, 56 * i + 64 + 2,
			255, 255, 255, 255,
			0.5f, 0.5f, H_CENTER, V_CENTER
		);
    }
}

static void sc_fliptest_render(void) {
}

static void sc_fliptest_dispose(void) {}

screen_t sc_fliptest = {
	"fliptest", sc_fliptest_initialize, sc_fliptest_render, sc_fliptest_dispose
};
