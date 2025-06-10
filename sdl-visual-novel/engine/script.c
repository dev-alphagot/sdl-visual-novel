// #define _CRT_SECURE_NO_WARNINGS

#include "script.h"

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <SDL2/SDL_Mixer.h>

#include "character.h"
#include "texthold.h"

#include "../wrapper/image.h"

#include "../screens/ingame.h"

#include "fupdate.h"

#include "../misc.h"

FILE* sc_script = NULL;

bool sc_exec_desire = true;

       int reg   = 0;
       int sc_delay = 65;

static char fnb[80] = "";

static int ingame_bgm_c = 0;
static Mix_Music* ingame_bgm_w;

uint8_t sc_sel_storage[SC_SEL_STORAGE_SIZE] = { 0 };

char** sc_script_index_table = NULL;
int sc_scripts = 0;
time_t* sc_script_cleared_date = NULL;

static int sc_words = 0;
bool* sc_word_collected = NULL;

time_t sc_save_last = 0;

int sc_index_current = 4;
static int sc_index_next = 4;
char sc_save_version[16] = "";
char sc_cd_version[16] = ""; // 클리어 일자 기록 파일 버전

static int sc_saving_mk_tick = -1;

bool sc_is_go_to_title(void) {
#if VERBOSE
    printf("SIGTT %d %d %d\n", sc_index_current, sc_index_next, sc_exec_desire);
#endif
    return (sc_index_current == sc_index_next) && !sc_exec_desire;
}

static void sc_bgm_fadein(void) {
	Mix_FreeMusic(ingame_bgm);
    Mix_FadeInMusic(ingame_bgm_w, 1 << 30, 2000);
	ingame_bgm = ingame_bgm_w;
}

static void sc_save_marker_anim(void) {
    if (sc_saving_mk_tick >= 180) {
        sc_saving_mk_tick = -2;
        sc_save_marker_a = 0;
        return;
    }

    sc_save_marker_a = (uint8_t)ceil(fabs(sinf(++sc_saving_mk_tick / 120.0f * M_PI * 2)) * 255);
}

void sc_forcejump(int offset) {
	fseek(sc_script, offset, SEEK_SET);
}

int sc_exec(void) {
    if (!sc_word_collected) {
        sc_word_collected = malloc(sizeof(bool) * 256);
    }

    if (!sc_script && sc_exec_desire) {
        static char fff[80] = "";
		sprintf_s(fff, 80, "def/scripts/%s.bin", sc_script_index_table[sc_index_next]);
        fopen_s(&sc_script, fff, "rb");
		sc_index_current = sc_index_next;
    }

	do {
        if (sc_delay > 0) {
            if(sc_delay < 65534) sc_delay--;
            return 1;
        }

		opcode_t opc = 0;

        if (feof(sc_script)) {
			sc_exec_desire = false;
            if(sc_saving_mk_tick == -1) fupdate_add(182, sc_save_marker_anim);
            return -1;
        }
        else {
            if (sc_saving_mk_tick == -2) sc_saving_mk_tick = -1;
        }

		fread_s(&opc, 1, 1, 1, sc_script);

		printf("Script Offset %d OpCode: %d\n", ftell(sc_script) - 1, opc);

		switch (opc) {

        case CHAR_SET:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

			for (int i = 0; i < CHAR_CAPACITY; i++) {
				if (characters[i].id == cid) {
					chr = characters[i];
					break;
				}
			}

			text_font(speak_content, chr.font);

            if (cid != 990) {
                sprintf_s(fnb, 80, "%s/%s.png", chr.path, chr.emotions[0].path); 
                image_color(ingame_char, 255, 255, 255);

                uint8_t ctmp = 0;
                fread_s(&ctmp, 1, 1, 1, sc_script);

                fseek(sc_script, -1, SEEK_CUR);

                if(ctmp != EMOTE) image_content(ingame_char, fnb);
            }
            else {
                image_color(ingame_char, 216, 216, 216);
            }

            break;
        }
        case BG_CROSSFADE:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            bg_cf_index = cid;
            sc_ingame_bg_cf_start();
            sc_delay = 60;

            break;
        }
        case BGM_PLAY:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            sprintf_s(fnb, 80, "sound/bgm/%d.ogg", cid);
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
            fread_s(&cid, 2, 2, 1, sc_script);

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
            fread_s(&cid, 4, 4, 1, sc_script);

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

            fread_s(&cix, 1, 1, 1, sc_script);
            fread_s(&cid, 2, 2, 1, sc_script);
            fread_s(&ciw, 4, 4, 1, sc_script);

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
            fread_s(&cid, 2, 2, 1, sc_script);

            fseek(sc_script, cid, SEEK_SET);

            break;
        }
        case COMPARE_JUMP:
        {
            uint8_t ccd = 0;
            short cid = 0;
            fread_s(&ccd, 1, 1, 1, sc_script);
            fread_s(&cid, 2, 2, 1, sc_script);

            printf("%d %d %d %d\n", reg, ccd, reg == ccd, cid);
            if (reg == ccd) fseek(sc_script, cid, SEEK_SET);

            break;
        }
        case WAIT:
        {
            unsigned short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            sc_delay = cid;
            break;
        }
        case SEL_SAVE:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            sc_sel_storage[cid] = reg;

            break;
        }
        case SEL_LOAD:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

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
            fread_s(&cid, 4, 4, 1, sc_script);

            sc_word_collected[cid - 99800000] = true;
            break;
        }
        case EMOTE:
        {
            uint8_t cid = 0;
            fread_s(&cid, 1, 1, 1, sc_script);
            sc_ingame_emote(cid);
            break;
        }
		case SE_PLAY:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            sprintf_s(fnb, 80, "sound/se/%d.wav", cid);
            Mix_Chunk* r = Mix_LoadWAV(fnb);

            Mix_VolumeChunk(r, vol_sfx);

            Mix_PlayChannel(-1, r, 0);

            break;
        }
        case CG_CONTENT:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            sc_ingame_cg_content(cid);

            break;
        }
		case NEXT:
		{
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            sc_index_next = cid;
			break;
		}
        case ADD_IMMEDIATE:
        {
            uint8_t cid = 0;
            fread_s(&cid, 1, 1, 1, sc_script);

            reg += cid;
            break;
        }
        case SUBTRACT_IMMEDIATE:
        {
            uint8_t cid = 0;
            fread_s(&cid, 1, 1, 1, sc_script);

            reg -= cid;
            break;
        }
        case ADD_STORAGE:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            reg += sc_sel_storage[cid];
            break;
        }
        case SUBTRACT_STORAGE:
        {
            short cid = 0;
            fread_s(&cid, 2, 2, 1, sc_script);

            reg -= sc_sel_storage[cid];
            break;
        }
        case TEXT_SCALE:
        {
            uint8_t cid = 0;
            fread_s(&cid, 1, 1, 1, sc_script);

			text_scale(speak_content, TEXT_CONTENT_SCALE * cid, TEXT_CONTENT_SCALE* cid);
            break;
        }
		}
    } while (sc_delay == 0);

	return 0;
}

void sc_init(void) {
    if (sc_word_collected) return;

    memcpy(sc_save_version, VERSION, 16);

    FILE* ff = fopen("def/text/998.csv", "rt");

    int wcnt = 1;

    while (!feof(ff)) {
        if (fgetc(ff) == '\n') wcnt++;
    }

    fclose(ff);

    sc_words = wcnt;

    sc_word_collected = calloc(sizeof(bool), wcnt);
    if (!sc_word_collected) return;

    fopen_s(&ff, "def/script_index.txt", "rt");

    wcnt = 1;

    while (!feof(ff)) {
        if (fgetc(ff) == '\n') wcnt++;
    }

    sc_scripts = wcnt;

	sc_script_index_table = calloc(sizeof(char*), wcnt);

    if (!sc_script_index_table) return;

    fseek(ff, 0, SEEK_SET);

    for (int i = 0; i < wcnt; i++) {
		sc_script_index_table[i] = calloc(sizeof(char), 64);
        if (!sc_script_index_table[i]) continue;
        fgets(sc_script_index_table[i], 64, ff);
        printf("%d %s\n", i, sc_script_index_table[i]);
		str_trim_lf(64, sc_script_index_table[i]);
    }

	fclose(ff);

    sc_script_cleared_date = calloc(sc_scripts, sizeof(time_t));

    FILE* cdf = fopen("cleared_date.bin", "rb");
    if (!cdf || !sc_script_cleared_date) goto SAVE;
    fread_s(sc_cd_version, 16, 16, 1, cdf);
    fread_s(sc_script_cleared_date, sizeof(time_t) * sc_scripts, sizeof(time_t), sc_scripts, cdf);
    fclose(cdf);

SAVE:

    FILE* save = fopen("save.bin", "rb");
    if (!save) return;
    fread_s(&sc_save_last, sizeof(time_t), sizeof(time_t), 1, save);
    fread_s(&sc_index_current, sizeof(int), sizeof(int), 1, save);
    fread_s(&sc_index_next, sizeof(int), sizeof(int), 1, save);
    fread_s(sc_save_version, 16, 16, 1, save);
    fread_s(sc_sel_storage, SC_SEL_STORAGE_SIZE, 1, SC_SEL_STORAGE_SIZE, save);
    
    int swd = 0;
    fread_s(&swd, sizeof(int), sizeof(int), 1, save);
	printf("WCOLL %d %d\n", swd, sc_words);
    fread_s(sc_word_collected, sc_words > swd ? swd : sc_words, sc_words > swd ? swd : sc_words, 1, save);
    fclose(save);

	printf("Save %lld %d %d\n", sc_save_last, sc_index_current, sc_index_next);
}

void sc_reset(void) {
    sc_save_last = 0;

    sc_index_current = sc_scripts - 2; // for fallback
    for (int i = 0; i < sc_scripts; i++) {
        if (!strcmp(sc_script_index_table[i], "prologue")) {
            sc_index_current = i;
        }
    }

	sc_index_next = sc_index_current;
	memset(sc_sel_storage, 0, SC_SEL_STORAGE_SIZE);
}

void sc_save(void) {
    time_t tm = time(NULL);
    FILE* save = fopen("save.bin", "wb");
    fwrite(&tm, sizeof(time_t), 1, save);
    fwrite(&sc_index_current, sizeof(int), 1, save);
    fwrite(&sc_index_next, sizeof(int), 1, save);
    fwrite(VERSION, 16, 1, save);
    fwrite(sc_sel_storage, 1, SC_SEL_STORAGE_SIZE, save);
    fwrite(&sc_words, sizeof(int), 1, save);
    fwrite(sc_word_collected, sc_words, 1, save);
    fclose(save);
    
    if (!sc_script_cleared_date[sc_index_current]) {
        sc_script_cleared_date[sc_index_current] = tm;
        FILE* cdf = fopen("cleared_date.bin", "wb");
        fwrite(VERSION, 16, 1, cdf);
        fwrite(sc_script_cleared_date, sizeof(time_t), sc_scripts, cdf);
        fclose(cdf);
    }

    sc_save_last = tm;
}
