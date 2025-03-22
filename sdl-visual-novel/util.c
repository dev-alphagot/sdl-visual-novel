#include "util.h"

#include <stdint.h>

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
