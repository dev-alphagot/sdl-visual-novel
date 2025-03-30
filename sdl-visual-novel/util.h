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

extern int quit;

int char_uni_bytes(char ss);

void str_trim_lf(int l, char* arr);

float ease_io_expo(float x);

float ease_io_cubic(float x);
