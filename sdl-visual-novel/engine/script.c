#define _CRT_SECURE_NO_WARNINGS

#include "script.h"

#include <stdio.h>

#include "character.h"
#include "texthold.h"

#include "../screens/ingame.h"

static FILE* script;

static character_t chr;

static int delay = 0;

int sc_exec(void) {
	while (delay == 0) {
		opcode_t opc;

		if (feof(script)) return -1;
		fread(&opc, 1, 1, script);

		switch (opc) {

        case CHAR_SET:
            break;
        case BG_CROSSFADE:
            break;
        case BGM_PLAY:
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
            break;
        case COMPARE_JUMP:
            break;
        case WAIT:
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
		}
	}

	return 0;
}
