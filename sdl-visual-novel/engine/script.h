#pragma once

#include <stdbool.h>

typedef enum {
	CHAR_SET = 1,
	BG_CROSSFADE,
	BGM_PLAY,
	CG_SHOW,
	CG_HIDE,
	TEXT,
	SEL_ADD,
	SEL_DISP,
	JUMP,
	COMPARE_JUMP,
	WAIT,
	SEL_SAVE,
	SEL_LOAD,
	CHAR_SHOW,
	CHAR_HIDE,
	WCOLL_ADD, // 단어장
	EMOTE,
	SE_PLAY
} opcode_t;

extern uint8_t sc_sel_storage[];

extern bool* sc_word_collected;

int sc_exec(void);
