#if _DEBUG
#define _CRT_SECURE_NO_WARNINGS
#endif

#define SDL_MAIN_HANDLED

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

//#pragma comment(linker, "/subsystem:\"windows\" \ /entry:\"mainCRTStartup\"")

#include "util.h"

#include <stdio.h>

#include <Windows.h>

int _main(void);

int main(void) {
#if NDEBUG
    HWND hWndConsole = GetConsoleWindow();
    ShowWindow(hWndConsole, SW_HIDE);
#else
    HWND hWndConsole = GetConsoleWindow();
    ShowWindow(hWndConsole, SW_SHOW);
    //freopen("log.txt", "w", stdout);
    //freopen("log.txt", "w", stderr);
#endif

    FILE* settings = fopen("settings.bin", "rb");

    if (settings) {
        fread(&vol_bgm, 1, 1, settings);
        fread(&vol_sfx, 1, 1, settings);
        fread(&st_key_guide, 1, 1, settings);
        fread(&st_scale_low_quality, 1, 1, settings);

        fclose(settings);
    }
    else {
        vol_bgm = 127;
        vol_sfx = 127;
        st_key_guide = true;
        st_scale_low_quality = false;

        fopen_s(&settings, "settings.bin", "wb");

        if (!settings) goto A;

        fwrite(&vol_bgm, 1, 1, settings);
        fwrite(&vol_sfx, 1, 1, settings);
        fwrite(&st_key_guide, 1, 1, settings);
        fwrite(&st_scale_low_quality, 1, 1, settings);

        fclose(settings);
    }

A:

    return _main();
}