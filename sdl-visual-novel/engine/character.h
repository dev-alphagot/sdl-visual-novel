#pragma once

#define CHAR_CAPACITY 16

#define CHAR_BASE_MAX_PATH 64
#define CHAR_EMOTION_MAX_PATH 16
#define CHAR_MAX_EMOTION 16

#define CHAR_INDEX_BUFFER_LENGTH 65536

typedef struct {
	int movement_multiplier;
	char path[CHAR_EMOTION_MAX_PATH];
} emotion_t;

typedef struct {
	int id;
	char path[CHAR_BASE_MAX_PATH];
	emotion_t emotions[CHAR_MAX_EMOTION];
} character_t;

void char_init(void);
