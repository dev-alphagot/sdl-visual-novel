// #define _CRT_SECURE_NO_WARNINGS

#include "character.h"

#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "../util.h"

character_t characters[CHAR_CAPACITY];
static bool is_initialized = false;

bool char_is_initialized(void) {
	return is_initialized;
}

void char_init(void) {
	if (is_initialized) return;

	FILE* index_handle = fopen("def/char_index.txt", "rt");
	char* index_buf = malloc(CHAR_INDEX_BUFFER_LENGTH);
	if (!index_buf) return;
	memset(index_buf, 0, CHAR_INDEX_BUFFER_LENGTH);

	fread_s(index_buf, CHAR_INDEX_BUFFER_LENGTH, CHAR_INDEX_BUFFER_LENGTH, 1, index_handle);

	int indices = 1;
	for (int i = 0; i < CHAR_INDEX_BUFFER_LENGTH; i++) {
		if (index_buf[i] == '\n') indices++;
	}

	char f_buf[64] = "";
	char p_buf[64] = "";
	fseek(index_handle, 0, SEEK_SET);
	for (int i = 0; i < indices; i++) {
		if (i >= CHAR_CAPACITY) return;

		char tok[64] = "";
		fgets(tok, 64, index_handle);
		str_trim_lf(64, tok);
		// char* tok = strtok(i == 0 ? index_buf : NULL, "\n");
		memset(f_buf, 0, 64);
		sprintf_s(f_buf, 64, "def/char/%s.txt", tok);

		FILE* hl = fopen(f_buf, "rt");
		int d = 0;

		fgets(p_buf, 64, hl);
		str_trim_lf(64, p_buf);

		d = atoi(p_buf);

		character_t chr = { -1, "", 0 }; // 임시 초기화

		for (int j = 0; j < d; j++) {
			fgets(p_buf, 64, hl);
			str_trim_lf(64, p_buf);

			if (j == 0) {
				chr.id = atoi(p_buf);
			}
			else if (j == 1) {
				strcpy_s(chr.path, 64, p_buf);
			}
			else if (j == 2) {
				chr.font = atoi(p_buf);
				//strcpy(chr.font, p_buf);
			}
			else {
				int emo_index = (j - 3) >> 1;

				if (j % 2 == 1) {
					chr.emotions[emo_index].movement_multiplier = atof(p_buf);
				}
				else {
					strcpy_s(chr.emotions[emo_index].path, 64, p_buf);
				}
			}
		}

		printf("ID %d\tPATH %s\nFONT %d\n", chr.id, chr.path, chr.font);
		for (int a = 0; a < CHAR_EMOTION_MAX_PATH; a++) {
			if (!chr.emotions[a].path) continue;
			if (!strlen(chr.emotions[a].path)) continue;

			printf(" - MMP %f\tPATH %s/%s\n", chr.emotions[a].movement_multiplier, chr.path, chr.emotions[a].path);
		}
		puts("");
		characters[i] = chr;
	}

	is_initialized = true;
}
