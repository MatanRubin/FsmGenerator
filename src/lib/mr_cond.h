#ifndef MR_COND_H
#define MR_COND_H

#include <pthread.h>
#include "mr_mutex.h"

#define MR_COND_MAGIC 0xddbbaa55


typedef	struct mr_cond {
    int magic;
    pthread_cond_t cond;
    const char *name;
} mr_cond_t;

void mr_cond_init(mr_cond_t *cond, const char *name);
void mr_cond_destroy(mr_cond_t *cond);
void mr_cond_wait(mr_cond_t *cond, mr_mutex_t *mtx);
void mr_cond_signal(mr_cond_t *cond);


#endif
