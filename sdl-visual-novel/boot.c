#define SDL_MAIN_HANDLED

#include "util.h"

#include <stdio.h>

int _main(void);

int main(void) {
    FILE* settings = fopen("settings.bin", "rb");

    fread(&vol_bgm, 1, 1, settings);
    fread(&vol_sfx, 1, 1, settings);
    fread(&st_key_guide, 1, 1, settings);
    fread(&st_scale_low_quality, 1, 1, settings);

    fclose(settings);

    return _main();
}