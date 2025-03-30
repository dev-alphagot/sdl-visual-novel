#define _CRT_SECURE_NO_WARNINGS

#include "script.h"

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL_Mixer.h>

#include "character.h"
#include "texthold.h"

#include "../wrapper/image.h"

#include "../screens/ingame.h"

static FILE* script;

       int reg   = 0;
       int sc_delay = 65;

static char fnb[80] = "";

static Mix_Music* ingame_bgm_w;

uint8_t sc_sel_storage[128] = { 0 };

bool* sc_word_collected = NULL;

static void sc_bgm_fadein(void) {
	Mix_FreeMusic(ingame_bgm);
    Mix_FadeInMusic(ingame_bgm_w, 1 << 30, 2000);
	ingame_bgm = ingame_bgm_w;
}

void sc_forcejump(int offset) {
	fseek(script, offset, SEEK_SET);
}

int sc_exec(void) {
    if (!sc_word_collected) {
        sc_word_collected = malloc(sizeof(bool) * 128);
    }

    if (!script) {
        script = fopen("def/scripts/prologue.bin", "rb");
    }

	do {
        if (sc_delay > 0) {
            if(sc_delay < 65534) sc_delay--;
            return 1;
        }

		opcode_t opc = 0;

		if (feof(script)) return -1;
		fread(&opc, 1, 1, script);

		printf("Script Offset %d OpCode: %d\n", ftell(script), opc);

		switch (opc) {

        case CHAR_SET:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

			for (int i = 0; i < CHAR_CAPACITY; i++) {
				if (characters[i].id == cid) {
					chr = characters[i];
					break;
				}
			}

            if (cid != 990) {
                sprintf(fnb, "%s/%s.png", chr.path, chr.emotions[0].path); 
                image_content(ingame_char, fnb);
            }

            break;
        }
        case BG_CROSSFADE:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            bg_cf_index = cid;
            sc_ingame_bg_cf_start();
            sc_delay = 60;

            break;
        }
        case BGM_PLAY:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            sprintf(fnb, "sound/bgm/%d.ogg", cid);
            ingame_bgm_w = Mix_LoadMUS(fnb);

            if (ingame_bgm) {
                Mix_FadeOutMusic(2000);
                Mix_HookMusicFinished(sc_bgm_fadein);
            }
            else {
                ingame_bgm = ingame_bgm_w;
				Mix_FadeInMusic(ingame_bgm, 1 << 30, 2000);
            }

            break;
        }
        case CG_SHOW:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            sc_ingame_cg_show(cid);
            sc_delay = 60;

            break;
        }
        case CG_HIDE:
        {
            sc_ingame_cg_hide();
            sc_delay = 60;

            break;
        }
        case TEXT:
        {
            sc_delay = 65536;

            text_h_t th = { -1, "" };

            th_search(chr.id * 100000, &th);

            strcpy_s(ingame_name, 128, th.value);

            int cid = 0;
            fread(&cid, 4, 1, script);

            th_search(cid, &th);

            strcpy_s(ingame_text, 2048, th.value);

            sc_ingame_text();
            break;
        }
        case SEL_ADD:
        {
            uint8_t cix = 0;
            short cid = 0;
            int ciw = 0;
            text_h_t th = { -1, "" };

            fread(&cix, 1, 1, script);
            fread(&cid, 2, 1, script);
            fread(&ciw, 4, 1, script);

			printf("SEL_ADD %d %d %d\n", cix, cid, ciw);

            th_search(ciw, &th);

            if (!(cix > 0 && cix < 10)) break;

            ingame_sel_text[cix - 1] = th.value;
			ingame_sel_offs[cix - 1] = cid;
            printf("%p %p\n", th.value, ingame_sel_text[cix - 1]);

            break;
        }
        case SEL_DISP:
        {
            sc_delay = 65535;
            sc_ingame_sel_disp();
            break;
        }
        case JUMP:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            fseek(script, cid, SEEK_SET);

            break;
        }
        case COMPARE_JUMP:
        {
            uint8_t ccd = 0;
            short cid = 0;
            fread(&ccd, 1, 1, script);
            fread(&cid, 2, 1, script);

            printf("%d %d %d %d\n", reg, ccd, reg == ccd, cid);
            if (reg == ccd) fseek(script, cid, SEEK_SET);

            break;
        }
        case WAIT:
        {
            unsigned short cid = 0;
            fread(&cid, 2, 1, script);

            sc_delay = cid;
            break;
        }
        case SEL_SAVE:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            sc_sel_storage[cid] = reg;

            break;
        }
        case SEL_LOAD:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            reg = sc_sel_storage[cid];

            break;
        }
        case CHAR_SHOW:
        {
            sc_ingame_char_show();
            sc_delay = 60;
            break;
        }
        case CHAR_HIDE:
        {
            sc_ingame_char_hide();
            sc_delay = 60;
            break;
        }
        case WCOLL_ADD:
        {
            int cid = 0;
            fread(&cid, 4, 1, script);

            sc_word_collected[cid - 99800000] = true;
            break;
        }
        case EMOTE:
        {
            uint8_t cid = 0;
            fread(&cid, 1, 1, script);
            sc_ingame_emote(cid);
            break;
        }
		case SE_PLAY:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            sprintf(fnb, "sound/se/%d.wav", cid);
            Mix_Chunk* r = Mix_LoadWAV(fnb);

            Mix_PlayChannel(-1, r, 0);

            break;
        }
        case CG_CONTENT:
        {
            short cid = 0;
            fread(&cid, 2, 1, script);

            sc_ingame_cg_content(cid);

            break;
        }
		}
    } while (sc_delay == 0);

	return 0;
}
