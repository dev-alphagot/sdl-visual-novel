#define _CRT_SECURE_NO_WARNINGS

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>
#include <stdio.h>

#include "window.h"
#include "wrapper/text.h"

int _main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (TTF_Init() == -1)
    {
        return 0;
    }

    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Initialization Failed: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0)
    {
        printf("SDL_CreateWindowAndRenderer Error\n");
        return 1;
    }

    SDL_RenderSetVSync(renderer, 1);

    SDL_SetWindowTitle(window, WINDOW_TITLE);

    err_text_t res = text_init(renderer);
    if (res) {
        printf(text_error_str(res));
        return 1;
    }

    res = text_add(u8"키스의 고유 조건은 입술끼리 만나야 하고 특별한 기술은 필요치 않다.", NANUMBARUNPENB, 10, 10, 255, 255, 255, 255);
    if (res > 0) {
        printf(text_error_str(res));
        return 1;
    }

    // 메시지 루프
    SDL_Event event;

    int quit = 0;
    long long tcnt = 0;

    int tid = 1;

    char sb[512] = "";
    memset(sb, 0, 512);

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    quit = 1;
            }
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 16, 32, 48, 255);

        sprintf(sb, u8"%lld 프레임 경과\n 인간이 이곳에 온 것은 수천 년 만이군…. 왠지 이런 대사가 해보고 싶었습니다.", tcnt);

        text_remove(tid);
        tid = text_add(sb, NANUMBARUNGOTHIC, 10, 200, 255, 255, 255, 255);
        if (tid > 0) {
            printf(text_error_str(res));
            return 1;
        }
        else {
            tid = -tid;
        }

        text_render();

        SDL_RenderPresent(renderer);

        tcnt++;
    }

    // 종료
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}