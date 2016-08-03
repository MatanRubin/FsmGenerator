#ifndef MR_FSM_H
#define MR_FSM_H

#define MR_FSM_INPUT_STRING_MAX_LEN 255

#include <pthread.h>
#include <stdbool.h>
#include "mr_fsm_states.h"
#include "mr_mutex.h"
#include "mr_cond.h"

typedef struct mr_fsm {
    unsigned int magic;
    const char *name;
    mr_fsm_state_t **states;
    pthread_t exec_thread;
    unsigned int n_states;
    mr_fsm_state_t *current_state;
    bool ready_for_input;
    mr_fsm_input_type_e current_input_type;
    int current_input_int;
    char current_input_str[MR_FSM_INPUT_STRING_MAX_LEN];
    mr_mutex_t input_mutex;
    mr_cond_t input_cond;
    bool should_run;
} mr_fsm_t;

void mr_fsm_init(mr_fsm_t *fsm);
void mr_fsm_destroy(mr_fsm_t *fsm);
void mr_fsm_start(mr_fsm_t *fsm);
void mr_fsm_start_at(mr_fsm_t *fsm, mr_fsm_state_t *state);
void mr_fsm_stop(mr_fsm_t *fsm);
mr_fsm_state_t *mr_fsm_get_state(mr_fsm_t *fsm);
void mr_fsm_input(mr_fsm_t *fsm, mr_fsm_input_type_e input_type, void *input);
void mr_fsm_print(const mr_fsm_t *fsm);

#endif
