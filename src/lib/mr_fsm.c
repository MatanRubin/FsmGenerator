#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "mr_fsm_states.h"

#include "mr_fsm.h"
#include "mr_mutex.h"
#include "mr_cond.h"

#define MR_FSM_MAGIC 0xaabbefef
#define MR_FSM_DESTROYED_MAGIC 0xaabbefef

/* internal functions */
static void assert_initialized(const mr_fsm_t *fsm) {
    if (fsm->magic != MR_FSM_MAGIC) {
        printf("[FTL] fsm not initialized!\n");
        abort();
    }
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// public functions

void mr_fsm_init(mr_fsm_t *fsm) {
    if (fsm->magic == MR_FSM_MAGIC) {
        printf("[FTL] already initialized!\n");
        abort();
    }

    fsm->name = NULL;
    fsm->n_states = 0;
    fsm->states = NULL;
    fsm->current_state = NULL;
    fsm->ready_for_input = false;
    mr_mutex_init(&fsm->input_mutex, "input_mutex");
    mr_cond_init(&fsm->input_cond, "input_cond");
    fsm->should_run = false;
    fsm->magic = MR_FSM_MAGIC;
}

void mr_fsm_destroy(mr_fsm_t *fsm) {
    assert_initialized(fsm);
    mr_mutex_destroy(&fsm->input_mutex);
    mr_cond_destroy(&fsm->input_cond);
    fsm->magic = MR_FSM_DESTROYED_MAGIC;
}

typedef struct mr_fsm_exec_args {
    mr_fsm_t *fsm;
    mr_mutex_t *ready_mtx;
    mr_cond_t *ready_cond;
    bool *ready;
} mr_fsm_exec_args_t;

void *mr_fsm_execution_loop(void *args) {
    mr_fsm_state_t *next_state;
    mr_fsm_exec_args_t *exec_args = (mr_fsm_exec_args_t *)args;
    mr_fsm_t *fsm = exec_args->fsm;

    // let the main thread know the fsm is ready for input. happens only once
    if (*exec_args->ready == false) {
        mr_mutex_lock(exec_args->ready_mtx);
        *(exec_args->ready) = true;
        mr_mutex_unlock(exec_args->ready_mtx);
        mr_cond_signal(exec_args->ready_cond);
    }

    while (fsm->should_run) {
        void *current_input = NULL;

        fsm->current_state->on_entry();

        mr_mutex_lock(&fsm->input_mutex);
        fsm->ready_for_input = true;
        // signal that we're ready for input
        mr_mutex_unlock(&fsm->input_mutex);
        mr_cond_signal(&fsm->input_cond);
        // wait for input

        mr_mutex_lock(&fsm->input_mutex);
        while (fsm->ready_for_input) {
            mr_cond_wait(&fsm->input_cond, &fsm->input_mutex);
        }
        switch (fsm->current_input_type) {
            case MR_FSM_INPUT_STRING:
                current_input = fsm->current_input_str;
                printf("[DBG] cur_state='%s' input='%s' ",
                       fsm->current_state->name, fsm->current_input_str);
                break;
            case MR_FSM_INPUT_INT:
                current_input = &fsm->current_input_int;
                printf("[DBG] cur_state='%s' input=%d ",
                       fsm->current_state->name, fsm->current_input_int);
                break;
            case MR_FSM_INPUT_PATTERN:
            case MR_FSM_INPUT_BLOB:
            case MR_FSM_INPUT_LAST:
            case MR_FSM_INPUT_NONE:
                abort();
        }
        fsm->current_state->exec(fsm->current_input_type, current_input);

        fsm->current_state->on_exit();
        next_state = fsm->current_state->next_state(fsm->current_input_type,
                                                    current_input);
        printf("next_state='%s'\n", fsm->current_state->name);
        fsm->current_state = next_state;
        mr_mutex_unlock(&fsm->input_mutex);
    }

    mr_mutex_unlock(&fsm->input_mutex);
    return NULL;
}

void mr_fsm_start(mr_fsm_t *fsm) {
    int err;
    bool ready;
    mr_mutex_t ready_mtx;
    mr_cond_t ready_cond;
    mr_fsm_exec_args_t args = {
            .fsm = fsm,
            .ready_mtx = &ready_mtx,
            .ready_cond = &ready_cond,
            .ready = &ready,
    };

    mr_mutex_init(&ready_mtx, "ready_mtx");
    mr_cond_init(&ready_cond, "ready_cond");
    printf("[INF] starting fsm name='%s'\n", fsm->name);
    fsm->should_run = true;
    mr_mutex_lock(&ready_mtx);
    ready = false;
    err = pthread_create(&fsm->exec_thread, NULL, mr_fsm_execution_loop, &args);
    if (err) {
        goto fatal;
    }
    while (ready == false) {
        printf("[DBG] waiting for fsm to be ready\n");
        mr_cond_wait(&ready_cond, &ready_mtx);
    }

    assert(ready);
    printf("[DBG] FSM READY, STARTING...\n");

fatal:
    return;
}

void mr_fsm_start_at(mr_fsm_t *fsm, mr_fsm_state_t *state) {
    assert_initialized(fsm);
    fsm->current_state = state;
    return mr_fsm_start(fsm);
}

void mr_fsm_stop(mr_fsm_t *fsm) {
    printf("[INF] stopping fsm name='%s'\n", fsm->name);
    fsm->should_run = false;
    pthread_join(fsm->exec_thread, NULL);
}

mr_fsm_state_t *mr_fsm_get_state(mr_fsm_t *fsm) {
    return fsm->current_state;
}

void mr_fsm_input(mr_fsm_t *fsm, mr_fsm_input_type_e input_type, void *input) {
    mr_mutex_lock(&fsm->input_mutex);
    while (!fsm->ready_for_input) {
        mr_cond_wait(&fsm->input_cond, &fsm->input_mutex);
    }

    fsm->current_input_type = input_type;
    switch (input_type) {
        case MR_FSM_INPUT_INT:
            fsm->current_input_int = *(int *)input;
            break;
        case MR_FSM_INPUT_STRING:
            strncpy(fsm->current_input_str, input, MR_FSM_INPUT_STRING_MAX_LEN);
            break;
        case MR_FSM_INPUT_NONE: // fall through all unspupported types
        case MR_FSM_INPUT_PATTERN:
        case MR_FSM_INPUT_BLOB:
        case MR_FSM_INPUT_LAST:
            abort();
    }

    fsm->ready_for_input = false;
    mr_mutex_unlock(&fsm->input_mutex);

    // signal that input is ready
    mr_cond_signal(&fsm->input_cond);
}

void mr_fsm_print(const mr_fsm_t *fsm) {
    int i;

    assert_initialized(fsm);

    printf("fsm={ name='%s' n_states=%u\n", fsm->name, fsm->n_states);
    for (i = 0; i < fsm->n_states; i++) {
        mr_fsm_state_t *state = fsm->states[i];
        printf("\tstate={name='%s'}\n", state->name);
    }
    printf("}\n");
}







