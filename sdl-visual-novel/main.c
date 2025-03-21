#define _CRT_SECURE_NO_WARNINGS

#include <SDL2/SDL.h>
#include <SDL2/SDL_TTF.h>
#include <stdio.h>

#include "window.h"

int _main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* pFont;

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


    SDL_SetWindowTitle(window, WINDOW_TITLE);

    SDL_Texture* screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    pFont = TTF_OpenFont("font/nbpb.ttf", 30);

    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(pFont, "키스의 고유 조건은 입술끼리\n만나야 하고 특별한 기술은\n필요치 않다.", textColor);

    if (!textSurface) {
        printf("SDL Initialization Failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Texture* mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    int mWidth = textSurface->w;
    int mHeight = textSurface->h;

    // render text
    SDL_Rect renderQuad = { 10, 10, mWidth, mHeight };

    // 메시지 루프
    SDL_Event event;
    int quit = 0;
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
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderCopy(renderer, mTexture, NULL, &renderQuad);
        SDL_RenderPresent(renderer);
    }

    // 종료
    TTF_CloseFont(pFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}