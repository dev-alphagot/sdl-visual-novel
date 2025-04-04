#include "util.h"

#include <stdint.h>
#include <math.h>

int quit = 0;

SDL_Renderer* renderer;

uint8_t vol_bgm = 0;
uint8_t vol_sfx = 0;

void iclamp(int* v, int mi, int mx) {
	if (*v < mi) *v = mi;
	else if (*v > mx) *v = mx;
}

int char_uni_bytes(char ss) {
    uint8_t s = (uint8_t)ss;

    if (s <= 0x7F) return 1; // 0xxx xxxx, 1 byte
    else if (s >= 0xC0 && s <= 0xDF) return 2; // 110x xxxx, 2 bytes
    else if (s >= 0xE0 && s <= 0xEF) return 3; // 1110 xxxx, 3 bytes
    else if (s >= 0xF0 && s <= 0xF7) return 4; // 1111 0xxx, 4 bytes
    else if (s >= 0xF8 && s <= 0xFB) return 5; // 1111 10xx, 5 bytes
    else if (s >= 0xFC && s <= 0xFD) return 6; // 1111 110x, 6 bytes
    else return 0; // invalid UTF-8 char, 1111 1110?
}

void str_trim_lf(int l, char* arr) {
    for (int i = l; i > -1; i--) {
        if (arr[i] == '\n' || arr[i] == '\r') {
            arr[i] = 0;
            // break;
        }
    }
}

float ease_io_expo(float x) {
    const float c1 = 1.70158f;
    const float c2 = c1 * 1.525f;

    return x < 0.5f
        ? (float)(pow(2 * x, 2) * ((c2 + 1) * 2 * x - c2)) / 2
        : (float)(pow(2 * x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) / 2;
}

float ease_io_cubic(float x){
    return x < 0.5f ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}
