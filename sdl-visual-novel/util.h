#pragma once

void iclamp(int* v, int mi, int mx);

typedef enum {
	LEFT,
	H_CENTER,
	RIGHT
} halign_t;

typedef enum {
	TOP,
	V_CENTER,
	BOTTOM
} valign_t;

int char_uni_bytes(char ss);

void str_trim_lf(int l, char* arr);
