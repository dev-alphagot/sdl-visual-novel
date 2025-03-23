#include "fupdate.h"

#include <stddef.h>

static fupdate_t fupds[FUPDATE_CAPACITY];

static int fupdate_able_index(void) {
	for (int i = 0; i < FUPDATE_CAPACITY; i++) {
		if (!fupds[i].worker) return i;
	}

	return -1;
}

// 성공 시 (인덱스 * -1)을 반환.
err_fupdate_t fupdate_add(int life, fupdate_worker_t worker) {
	if (!worker) return FUPDATE_INVALID_WORKER;
	if (life <= 0) return FUPDATE_INVALID_LIFE;

	int index = fupdate_able_index();

	if (index < 0) return FUPDATE_CAPACITY_EXCEEDED;

	fupdate_t fu = { life, worker };

	fupds[index] = fu;

	return -index;
}

err_fupdate_t fupdate_remove(int id) {
	if (id < 0 || id >= FUPDATE_CAPACITY) return FUPDATE_INVALID_ID;
	
	fupds[id].worker = NULL;

	return 0;
}

void fupdate_update(void) {
	for (int i = 0; i < FUPDATE_CAPACITY; i++) {
		if (!fupds[i].worker) continue;
		if ((fupds[i].life--) <= 0) {
			fupdate_remove(i);
			continue;
		}

		fupds[i].worker();
	}
}
