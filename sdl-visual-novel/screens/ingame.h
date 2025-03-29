#pragma once

#include "../engine/screen.h"
#include "../engine/character.h"

#include <SDL2/SDL_Mixer.h>

#include <stdbool.h>

extern screen_t sc_ingame;

extern character_t chr;

extern int ingame_char;

extern int bg_cf_index;
extern int ingame_tid;

extern char ingame_name[];
extern char ingame_text[];

extern Mix_Music* ingame_bgm;

extern char* ingame_sel_text[];
extern int   ingame_sel_offs[];
extern bool ingame_sel_disp;
extern int ingame_sel_last; // 0..9

void sc_ingame_text(void);

void sc_ingame_cg_show(short id);
void sc_ingame_cg_hide(void);

void sc_ingame_char_show(void);
void sc_ingame_char_hide(void);

void sc_ingame_sel_disp(void);

void sc_ingame_emote(int ei);

void sc_ingame_bg_cf_start(void);
