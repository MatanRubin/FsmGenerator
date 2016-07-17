#ifndef MR_MUTEX_H
#define MR_MUTEX_H

#include <pthread.h>

#define MR_MUTEX_MAGIC 0xffaabb12


typedef	struct mr_mutex {
	int magic;
	pthread_mutex_t mtx;
	const char *name;
} mr_mutex_t;

void mr_mutex_init(mr_mutex_t *mtx, const char *name);
void mr_mutex_destroy(mr_mutex_t *mtx);
void mr_mutex_lock(mr_mutex_t *mtx);
void mr_mutex_unlock(mr_mutex_t *mtx);

#endif
