#pragma once

#define FUPDATE_CAPACITY 16

typedef enum {
	FUPDATE_CAPACITY_EXCEEDED = 1,
	FUPDATE_INVALID_WORKER,
	FUPDATE_INVALID_LIFE,
	FUPDATE_INVALID_ID
} err_fupdate_t;

typedef void (*fupdate_worker_t)(void);

typedef struct {
	int life;
	fupdate_worker_t worker;
} fupdate_t;

err_fupdate_t fupdate_add(int life, fupdate_worker_t worker);

err_fupdate_t fupdate_remove(int id);

void fupdate_update(void);
