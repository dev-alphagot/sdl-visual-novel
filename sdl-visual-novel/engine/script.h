#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define SC_SEL_STORAGE_SIZE 256

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
	SE_PLAY,
	CG_CONTENT,
	NEXT,
	ADD_IMMEDIATE,
	SUBTRACT_IMMEDIATE,
	ADD_STORAGE,
	SUBTRACT_STORAGE,
	TEXT_SCALE
} opcode_t;

extern bool sc_exec_desire;

extern int reg;
extern int sc_delay;

extern uint8_t sc_sel_storage[];

extern FILE* sc_script;

extern bool* sc_word_collected;

extern time_t sc_save_last;

extern int sc_index_current;
extern char sc_save_version[16];
extern char** sc_script_index_table;
extern int sc_scripts;

extern time_t* sc_script_cleared_date;

bool sc_is_go_to_title(void);

void sc_forcejump(int offset);

int sc_exec(void);

void sc_init(void);

void sc_reset(void);

void sc_save(void);
