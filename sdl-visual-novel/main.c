#define _CRT_SECURE_NO_WARNINGS

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

#include "wrapper/text.h"
#include "wrapper/image.h"
#include "wrapper/input.h"

#include "screens/notice.h"
#include "screens/title.h"
#include "screens/ingame.h"
#include "screens/fliptest.h"
#include "screens/wcoll.h"
#include "screens/settings.h"

int _main(void) {
    SDL_Window* window = NULL;

    if (TTF_Init() == -1)
    {
        return 0;
    }

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

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

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
    screens[3] = sc_fliptest;
    screens[4] = sc_wcoll;
    screens[5] = sc_settings;

    screen_init(renderer, 6);

    th_init();

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
        }

        SDL_SetRenderDrawColor(renderer, bg_fill_color.r, bg_fill_color.g, bg_fill_color.b, bg_fill_color.a);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        screen_render();

        image_render();

        text_render();

        fupdate_update();

        if (input_is_keydown(SDLK_ESCAPE)) quit = 1;

        input_update();

        if (sc_save_marker) {
            SDL_SetTextureAlphaMod(sc_save_marker, sc_save_marker_a);
            SDL_RenderCopy(renderer, sc_save_marker, NULL, &((SDL_Rect) { 1070, 10, 200, 50 }));
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