#ifndef SIMPLE_FSM_H
#define SIMPLE_FSM_H

#include "mr_fsm.h"
#include "simple_fsm_states.h"

typedef mr_fsm_t simple_fsm_t;

void simple_fsm_init(simple_fsm_t *fsm);
void simple_fsm_destroy(simple_fsm_t *fsm);
void simple_fsm_start(simple_fsm_t *fsm);
void simple_fsm_start_at(simple_fsm_t *fsm, simple_fsm_state_t *state);
void simple_fsm_stop(simple_fsm_t *fsm);
simple_fsm_state_t *simple_fsm_get_state(simple_fsm_t *fsm);
void simple_fsm_input_int(simple_fsm_t *fsm, int input);
void simple_fsm_input_string(simple_fsm_t *fsm, const char *input);
void simple_fsm_print(const simple_fsm_t *fsm);

#endif
