#define _CRT_SECURE_NO_WARNINGS

#include "script.h"

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL_Mixer.h>

#include "character.h"
#include "texthold.h"

#include "../screens/ingame.h"

static FILE* script;

static character_t chr;

static int reg   = 0;
static int delay = 0;

static char fnb[64] = "";

static Mix_Music* ingame_bgm_w;

uint8_t sc_sel_storage[128] = { 0 };

bool* sc_word_collected;

static void sc_bgm_fadein(void) {
	Mix_FreeMusic(ingame_bgm);
    Mix_FadeInMusic(ingame_bgm_w, 1 << 30, 2000);
	ingame_bgm = ingame_bgm_w;
}

int sc_exec(void) {
	do {
		opcode_t opc;

		if (feof(script)) return -1;
		fread(&opc, 1, 1, script);

		switch (opc) {

        case CHAR_SET:
            short cid = 0;
            fread(&cid, 2, 1, script);

            chr = characters[cid];

            break;
        case BG_CROSSFADE:
            short cid = 0;
            fread(&cid, 2, 1, script);

			bg_cf_index = cid;

            break;
        case BGM_PLAY:
            short cid = 0;
            fread(&cid, 2, 1, script);

			sprintf(fnb, "sound/bgm/%d.ogg", cid);
			ingame_bgm_w = Mix_LoadMUS(fnb);

			Mix_FadeOutMusic(2000);
			Mix_HookMusicFinished(sc_bgm_fadein);

            break;
        case CG_SHOW:
            short cid = 0;
            fread(&cid, 2, 1, script);

            sc_ingame_cg_show(cid);
            delay = 60;

            break;
        case CG_HIDE:
            sc_ingame_cg_hide();
            delay = 60;

            break;
        case TEXT:
            text_h_t th = { -1, "" };

            th_search(chr.id * 100000, &th);

            strcpy_s(ingame_name, 128, th.value);

            short cid = 0;
            fread(&cid, 2, 1, script);

            th_search(chr.id * 100000 + cid, &th);

            strcpy_s(ingame_text, 2048, th.value);

            sc_ingame_text();
            break;
        case SEL_ADD:
            uint8_t cix = 0;
            short cid = 0;
            int ciw = 0;
            text_h_t th = { -1, "" };

            fread(&cix, 1, 1, script);
            fread(&cid, 2, 1, script);
            fread(&ciw, 4, 1, script);

            cix--;

            th_search(chr.id * 100000 + ciw, &th);

            if (!(cix > 0 && cix < 10)) break;

            ingame_sel_text[cix] = th.value;

            break;
        case SEL_DISP:
            ingame_sel_last = -1;
            ingame_sel_disp = true;
            break;
        case JUMP:
            short cid = 0;
            fread(&cid, 2, 1, script);

			fseek(script, cid, SEEK_SET);

            break;
        case COMPARE_JUMP:
            uint8_t ccd = 0;
            short cid = 0;
            fread(&ccd, 1, 1, script);
            fread(&cid, 2, 1, script);

			if (reg == ccd) fseek(script, cid, SEEK_SET);

            break;
        case WAIT:
            short cid = 0;
            fread(&cid, 2, 1, script);

			delay = cid;
            break;
        case SEL_SAVE:
            short cid = 0;
            fread(&cid, 2, 1, script);

            sc_sel_storage[cid] = reg;

            break;
        case SEL_LOAD:
            short cid = 0;
            fread(&cid, 2, 1, script);

            reg = sc_sel_storage[cid];

            break;
        case CHAR_SHOW:
            break;
        case CHAR_HIDE:
            break;
        case WCOLL_ADD:
            break;
        case EMOTE:
            uint8_t cid = 0;
            fread(&cid, 1, 1, script);
            sc_ingame_emote(cid);
			break;
		case SE_PLAY:
            short cid = 0;
            fread(&cid, 2, 1, script);

            sprintf(fnb, "sound/se/%d.wav", cid);
            Mix_Chunk* r = Mix_LoadWAV(fnb);

            Mix_PlayChannel(-1, r, 0);

			break;
		}
    } while (delay == 0);

	return 0;
}
