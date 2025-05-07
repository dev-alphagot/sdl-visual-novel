// #define _CRT_SECURE_NO_WARNINGS

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <SDL2/SDL_Mixer.h>
#include <SDL2/SDL_TTF.h>
#include <SDL2/SDL2_framerate.h>

#include <stdio.h>
#include <stdbool.h>

#include "window.h"
#include "util.h"

#include "engine/fupdate.h"
#include "engine/screen.h"
#include "engine/texthold.h"
#include "engine/modal.h"

#include "wrapper/text.h"
#include "wrapper/image.h"
#include "wrapper/input.h"

#include "screens/notice.h"
#include "screens/title.h"
#include "screens/ingame.h"
#include "screens/diary.h"
#include "screens/wcoll.h"
#include "screens/settings.h"
#include "screens/ending.h"

static bool fullscreen = false;

int _main(void) {
    SDL_Window* window = NULL;

    if (TTF_Init() == -1)
    {
        return 0;
    }

    if(!st_scale_low_quality) SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Initialization Failed: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI, &window, &renderer) < 0)
    {
        fprintf(stderr, "SDL_CreateWindowAndRenderer Error\n");
        return 1;
    }

    SDL_RenderSetVSync(renderer, 1);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDOPERATION_ADD);

    SDL_SetWindowTitle(window, WINDOW_TITLE);

    SDL_Init(SDL_INIT_AUDIO); 
    
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    SDL_SetWindowIcon(window, IMG_Load("image/icon.png"));

    if (Mix_OpenAudio(48000, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Failed to initialize SDL_mixer: %s\n", Mix_GetError());
        exit(1);
    }

    err_text_t res = text_init(renderer);
    if (res) {
        fprintf(stderr, text_error_str(res));
        return 1;
    }

    res = image_init(renderer);
    if (res) {
        fprintf(stderr, image_error_str(res));
        return 1;
    }

    screens[0] = sc_notice;
    screens[1] = sc_title;
    screens[2] = sc_ingame;
    screens[3] = sc_diary;
    screens[4] = sc_wcoll;
    screens[5] = sc_settings;
    screens[6] = sc_ending;

    sc_ending_renderer = renderer;

    screen_init(renderer, 7);

    th_init();

	modal_init(renderer);

    Mix_VolumeMusic(vol_bgm);

    decide_sfx = Mix_LoadWAV("sound/se/decide.wav");
    arrow_sfx  = Mix_LoadWAV("sound/se/move.wav");

    Mix_VolumeChunk(decide_sfx, vol_sfx);
    Mix_VolumeChunk(arrow_sfx, vol_sfx);

    SDL_Surface* imageSurface;
    if (!(imageSurface = IMG_Load("image/ui/saving.png"))) return;

    if (!(sc_save_marker = SDL_CreateTextureFromSurface(renderer, imageSurface))) return;

    // 메시지 루프
    SDL_Event event;

    FPSmanager fpsManager;
    SDL_initFramerate(&fpsManager);
    SDL_setFramerate(&fpsManager, 60);

    SDL_Texture* target = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        WINDOW_WIDTH, WINDOW_HEIGHT);

    SDL_Texture* blurTex = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        WINDOW_WIDTH, WINDOW_HEIGHT);

    //SDL_SetTextureScaleMode(target, SDL_ScaleModeLinear);
    //SDL_SetTextureScaleMode(blurTex, SDL_ScaleModeLinear);

    struct Offset {
        int x, y;
        int alpha;
    } offsets[] = {
        { 0, 0, 128 },  // 중심
        { -1, 0, 64 }, { 1, 0, 64 }, { 0, -1, 64 }, { 0, 1, 64 }, // 1칸 거리
        { -2, 0, 32 }, { 2, 0, 32 }, { 0, -2, 32 }, { 0, 2, 32 }, // 2칸 거리
        { -1, -1, 48 }, { 1, -1, 48 }, { -1, 1, 48 }, { 1, 1, 48 }, // 대각
        { -2, -2, 16 }, { 2, -2, 16 }, { -2, 2, 16 }, { 2, 2, 16 }, // 먼 대각
    };

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                input_keydown(event.key.keysym.sym);
            }
            else if (event.type == SDL_KEYUP) {
                input_keyup(event.key.keysym.sym);
            }
            else if (event.type == SDL_QUIT) {
                quit = 1;
            }
            else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    screen_focus_lost();
                }
                else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                    screen_focus_restore();
                }
            }
        }

        if (input_is_keydown(SDLK_F11)) {
            SDL_DisplayMode DM;
            SDL_GetCurrentDisplayMode(0, &DM);

            SDL_SetWindowFullscreen(window, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
            SDL_RenderSetScale(
                renderer, 
                fullscreen ? 1.0f : DM.w / (float)WINDOW_WIDTH,
                fullscreen ? 1.0f : DM.h / (float)WINDOW_HEIGHT
            );
            fullscreen = !fullscreen;
        }

        SDL_SetRenderTarget(renderer, target);

        SDL_SetRenderDrawColor(renderer, bg_fill_color.r, bg_fill_color.g, bg_fill_color.b, bg_fill_color.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        screen_render();

        image_render();

        text_render();

        //SDL_SetRenderTarget(renderer, NULL);

        if (modal_is_on) {
            SDL_SetTextureBlendMode(target, SDL_BLENDMODE_BLEND);

            SDL_SetRenderTarget(renderer, blurTex);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            for (int i = 0; i < sizeof(offsets) / sizeof(offsets[0]); i++) {
                SDL_SetTextureAlphaMod(target, offsets[i].alpha);
                SDL_Rect dst = { offsets[i].x, offsets[i].y, WINDOW_WIDTH, WINDOW_HEIGHT };
                SDL_RenderCopy(renderer, target, NULL, &dst);
            }
            SDL_SetRenderTarget(renderer, NULL);
            SDL_RenderCopy(renderer, blurTex, NULL, &((SDL_Rect) { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }));
        }
        else {
            SDL_SetTextureBlendMode(target, SDL_BLENDMODE_NONE);

            SDL_SetRenderTarget(renderer, NULL);

            SDL_Rect dst = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
            SDL_RenderCopy(renderer, target, NULL, &dst);
        }

        modal_render();

        fupdate_update();

        if (input_is_keydown(SDLK_ESCAPE)) quit = 1;

        input_update();

        if (sc_save_marker) {
            SDL_SetTextureAlphaMod(sc_save_marker, sc_save_marker_a);
            SDL_RenderCopy(renderer, sc_save_marker, NULL, &((SDL_Rect) { 1070, 10, 200, 25 }));
        }

        SDL_RenderPresent(renderer);

        SDL_framerateDelay(&fpsManager);
    }

    IMG_Quit();

    // 종료
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}