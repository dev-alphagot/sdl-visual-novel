#define _CRT_SECURE_NO_WARNINGS

#include <SDL2/SDL.h>
#include <SDL2/SDL_Image.h>
#include <SDL2/SDL_Mixer.h>
#include <SDL2/SDL_TTF.h>
#include <SDL2/SDL2_framerate.h>

#include <stdio.h>
#include <stdbool.h>

#include "window.h"

#include "engine/fupdate.h"
#include "engine/screen.h"

#include "wrapper/text.h"
#include "wrapper/image.h"

static char* imgs[] = {
    "Concerned.png",
    "embarrased.png",
    "mad c.png",
    "nssnss.png",
    "sad.png",
    "smile c.png",
    "upset.png",
    "angry c.png",
    "happy c.png",
    "mad.png",
    "rage.png",
    "shsusus.png",
    "smile.png",
    "worried c.png",
    "concerned 2.png",
    "happy.png",
    "neutral.png",
    "sad c.png",
    "shsusus2.png",
    "surprised.png",
    "worried.png"
};

int _main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (TTF_Init() == -1)
    {
        return 0;
    }

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL Initialization Failed: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0)
    {
        fprintf(stderr, "SDL_CreateWindowAndRenderer Error\n");
        return 1;
    }

    SDL_RenderSetVSync(renderer, 1);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDOPERATION_ADD);

    SDL_SetWindowTitle(window, WINDOW_TITLE);

    SDL_Init(SDL_INIT_AUDIO); 
    
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

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

    screen_init(renderer);

    res = text_add_as(
        u8"키스의 고유 조건은 입술끼리 만나야\n하고 특별한 기술은 필요치 않다.", 
        SPOQAHANSANSNEO, 
        8, 8, 
        0, 0, 0, 255, 
        0.5f, 0.5f, LEFT, TOP
    );
    if (res > 0) {
        fprintf(stderr, text_error_str(res));
        return 1;
    }

    res = image_add(
        "image/heroine/angry c.png",
        WINDOW_WIDTH / 2, WINDOW_HEIGHT,
        1.2f, 1.2f, H_CENTER, BOTTOM
    );
    if (res > 0) {
        fprintf(stderr, image_error_str(res));
        return 1;
    }

    image_add(
        "image/ui/text_bg.png",
        WINDOW_WIDTH / 2, WINDOW_HEIGHT,
        1.0f, 1.0f, H_CENTER, BOTTOM
    ); 
    
    int nameText = -text_add_as(
        u8"히비키",
        GYEONGGIMILLENNIUMBATANGB,
        104, WINDOW_HEIGHT - 104,
        255, 255, 255, 255,
        1.0f, 1.0f, H_CENTER, V_CENTER
    );

    const char* txt = u8"サンタクロースさんが たった一晩でキヴォトスの 子供たちにプレゼントを渡す\nためには… 彼のトナカイは最低 マッハ4以上の速度を出さなきゃ いけないはず。\n키스의 고유 조건은 입술끼리 만나야 하고 특별한 기술은 필요치 않다.";

    int content = -text_add_as(
        u8"",
        COMBINED,
        220, WINDOW_HEIGHT - 180,
        255, 255, 255, 255,
        0.5, 0.5, LEFT, TOP
    );

    Mix_Music* music = Mix_LoadMUS("sound/bgm/rwaltz.ogg");
    if (music == NULL) {
        fprintf(stderr, "Failed to load music file: %s\n", Mix_GetError());
        return 1;
    }

    Mix_PlayMusic(music, 1 << 30);

    // 메시지 루프
    SDL_Event event;

    int quit = 0;
    int iid = 0;
    long long tcnt = 0;

    bool keyDown = false;

    int tid = -text_add_as(
        u8"",
        GYEONGGIMILLENNIUMBATANGB,
        8, 712,
        0, 0, 0, 255,
        0.4f, 0.4f, LEFT, BOTTOM
    );

    char sb[512] = "";
    char s2[128] = "image/heroine/angry c.png";
    char s3[512] = "";

    int offs = -1;
    int delay = 0;

    FPSmanager fpsManager;
    SDL_initFramerate(&fpsManager);
    SDL_setFramerate(&fpsManager, 60);

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;
                else if (event.key.keysym.sym == SDLK_SPACE) {
                    sprintf(s2, "image/heroine/%s", imgs[iid++]);
                    iid = iid % (sizeof(imgs) / sizeof(char*));
                    
                    text_content(3, s2);
                    
                    err_image_t rs = image_content(0, s2);

                    if (rs > 0) fprintf(stderr, "SDL Error: %s (%s)\n", image_error_str(rs), SDL_GetError());
                }
                else if (event.key.keysym.sym == SDLK_RETURN && !keyDown) {
                    keyDown = true;
                    offs = 0;
                    delay = 0;

                    screen_ss();

                    printf("%d\n", fupdate_add(181, screen_tr));
                }
            }
            else if (event.type == SDL_KEYUP) {
                if (event.key.keysym.sym == SDLK_RETURN && keyDown) {
                    keyDown = false;
                }
            }
            else if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        if (offs >= 0 && offs < strlen(txt) && delay-- <= 0) {
            int bt = char_uni_bytes(txt[offs]);
            delay = bt * 2;
            offs += bt;
            memcpy(s3, txt, offs);

            text_content(content, s3);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        sprintf(sb, u8"%lf / %lf s\n(%d / %d samples)", 
            Mix_GetMusicPosition(music), Mix_MusicDuration(music),
            (int)round(Mix_GetMusicPosition(music) * 48000), (int)round(Mix_MusicDuration(music) * 48000)
        );

        text_content(0, sb);

        screen_render();

        image_render();

        text_render();

        fupdate_update();

        SDL_RenderPresent(renderer);

        SDL_framerateDelay(&fpsManager);

        tcnt++;
    }

    IMG_Quit();

    // 종료
    Mix_FreeMusic(music);
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}