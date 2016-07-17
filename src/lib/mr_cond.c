#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include "mr_common.h"
#include "mr_cond.h"
#include "mr_mutex.h"

extern bool debug_mode_enabled;

void mr_cond_init(mr_cond_t *cond, const char *name) {
    int err;

    if (cond->magic == MR_COND_MAGIC) {
        mr_panic();
    }

    err = pthread_cond_init(&cond->cond, NULL);
    if (err) {
        mr_panic();
    }

    cond->name = name ? name : "";
    cond->magic = MR_COND_MAGIC;
}

void mr_cond_destroy(mr_cond_t *cond) {
    int err;

    if (cond->magic != MR_COND_MAGIC) {
        mr_panic();
    }

    err = pthread_cond_destroy(&cond->cond);
    if (err) {
        mr_panic();
    }
    cond->magic = MR_DESTROYED_MAGIC;
}

void mr_cond_wait(mr_cond_t *cond, mr_mutex_t *mtx) {
    int err;

    if (cond->magic != MR_COND_MAGIC) {
        mr_panic();
    }

    if (debug_mode_enabled) {
        printf("[DBG] waiting on cond. name='%s'\n", cond->name);
    }
    err = pthread_cond_wait(&cond->cond, &mtx->mtx);
    if (err) {
        mr_panic();
    }
}

void mr_cond_signal(mr_cond_t *cond) {
    int err;

    if (cond->magic != MR_COND_MAGIC) {
        mr_panic();
    }

    if (debug_mode_enabled) {
        printf("[DBG] signaling on cond. name='%s'\n", cond->name);
    }
    err = pthread_cond_signal(&cond->cond);
    if (err) {
        mr_panic();
    }
}
