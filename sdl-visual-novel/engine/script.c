#define _CRT_SECURE_NO_WARNINGS

#include "script.h"

#include <stdio.h>
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

static void sc_bgm_fadein(void) {
	Mix_FreeMusic(ingame_bgm);
    Mix_FadeInMusic(ingame_bgm_w, 1 << 30, 2000);
	ingame_bgm = ingame_bgm_w;
}

int sc_exec(void) {
	while (delay == 0) {
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
            break;
        case CG_HIDE:
            break;
        case TEXT:
            break;
        case SEL_ADD:
            break;
        case SEL_DISP:
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
            break;
        case SEL_LOAD:
            break;
        case CHAR_SHOW:
            break;
        case CHAR_HIDE:
            break;
        case WCOLL_ADD:
            break;
        case EMOTE:
			break;
		case SE_PLAY:
			break;
		}
	}

	return 0;
}
