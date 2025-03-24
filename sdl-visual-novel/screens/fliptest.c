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

static int img_l;
static int img_r;
static int img_rb;

static char bf[16] = "";

static void sc_fliptest_initialize(void) {
    txt_lt = -text_add_as(
        u8"-1",
        SPOQAHANSANSNEO,
        8, 8,
        255, 255, 255, 255,
        0.25f, 0.25f, LEFT, TOP
    );

	img_l  = -image_add("image/ui/page_image_b.png", WINDOW_WIDTH / 2 - 4, WINDOW_HEIGHT / 2, 1.125f, 1.5f, RIGHT, V_CENTER);
    img_rb = -image_add("image/ui/page_image_b.png", WINDOW_WIDTH / 2 + 4, WINDOW_HEIGHT / 2, 1.125f, 1.5f, LEFT, V_CENTER);
	img_r  = -image_add("image/ui/page_image.png", WINDOW_WIDTH / 2 + 4, WINDOW_HEIGHT / 2, 1.125f, 1.5f,  LEFT, V_CENTER);
}

static void sc_fliptest_render(void) {
    if (input_is_keydown(SDLK_p) && anmTick == -1) {
        anmTick = 0;
        image_clear();

        img_l = -image_add("image/ui/page_image_b.png", WINDOW_WIDTH / 2 - 4, WINDOW_HEIGHT / 2, 1.125f, 1.5f, RIGHT, V_CENTER);
        img_rb = -image_add("image/ui/page_image_b.png", WINDOW_WIDTH / 2 + 4, WINDOW_HEIGHT / 2, 1.125f, 1.5f, LEFT, V_CENTER);
        img_r = -image_add("image/ui/page_image.png", WINDOW_WIDTH / 2 + 4, WINDOW_HEIGHT / 2, 1.125f, 1.5f, LEFT, V_CENTER);

        Mix_PlayChannel(-1, Mix_LoadWAV("sound/se/page.wav"), 0);
    }

    if (anmTick >= 0) {
        if (anmTick % 3 == 0) {
            switch (anmTick / 3) {
                case 0:
                    img_r = -image_content(img_r, "image/ui/page_image_h.png");
                    break;
                case 1:
                    img_r = -image_content(img_r, "image/ui/page_image_3.png");
                    break;
                case 2:
                    image_remove(img_r); 
                    img_r = -image_add("image/ui/page_image_h.png", WINDOW_WIDTH / 2 - 4, WINDOW_HEIGHT / 2, 1.125f, 1.5f, RIGHT, V_CENTER);
                    break;
                case 3:
                    img_r = -image_content(img_r, "image/ui/page_image.png");

                    int i = img_l;
                    img_l = img_r;

                    image_remove(i);
                    break;
                default:
                    anmTick = -2;
                    break;
            }
        }
        anmTick++;
    }

	sprintf(bf, "%d", anmTick);
	text_content(txt_lt, bf);
}

static void sc_fliptest_dispose(void) {}

screen_t sc_fliptest = {
	"fliptest", sc_fliptest_initialize, sc_fliptest_render, sc_fliptest_dispose
};
