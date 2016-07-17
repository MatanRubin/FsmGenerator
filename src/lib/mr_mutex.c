#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include "mr_common.h"
#include "mr_mutex.h"

extern bool debug_mode_enabled;

void mr_mutex_init(mr_mutex_t *mtx, const char *name) {
	int err;

	if (mtx->magic == MR_MUTEX_MAGIC) {
		mr_panic();
	}

	err = pthread_mutex_init(&mtx->mtx, PTHREAD_MUTEX_DEFAULT);
	if (err) {
		mr_panic();
	}

	mtx->name = name ? name : "";
	mtx->magic = MR_MUTEX_MAGIC;
}

void mr_mutex_destroy(mr_mutex_t *mtx) {
	int err;

	if (mtx->magic != MR_MUTEX_MAGIC) {
		mr_panic();
	}

	pthread_mutex_destroy(&mtx->mtx);
	mtx->magic = MR_DESTROYED_MAGIC;
}

void mr_mutex_lock(mr_mutex_t *mtx) {
	int err;

	if (mtx->magic != MR_MUTEX_MAGIC) {
		mr_panic();
	}

    if (debug_mode_enabled) {
        printf("[DBG] locking mutex name='%s'\n", mtx->name);
    }
	err = pthread_mutex_lock(&mtx->mtx);
	if (err) {
		mr_panic();
	}
}

void mr_mutex_unlock(mr_mutex_t *mtx) {
	int err;

	if (mtx->magic != MR_MUTEX_MAGIC) {
		mr_panic();
	}

    if (debug_mode_enabled) {
        printf("[DBG] unlocking mutex name='%s'\n", mtx->name);
    }
	err = pthread_mutex_unlock(&mtx->mtx);
	if (err) {
		mr_panic();
	}
}
