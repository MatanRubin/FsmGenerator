#include <stdlib.h>

#include "mr_fsm.h"
#include "simple_fsm.h"
#include "simple_fsm_states.h"

void simple_fsm_init(simple_fsm_t *fsm)
{
	unsigned int n_states = 2;
	mr_fsm_init(fsm);
	fsm->name = "simple_fsm";
	fsm->n_states = n_states;
	fsm->states = (simple_fsm_state_t **)malloc(n_states * sizeof(simple_fsm_state_t *));
	if (!fsm->states)
	{
		abort();
	}
	fsm->states[0] = &S1;
	fsm->states[1] = &S2;
	fsm->current_state = &S1;
}

void simple_fsm_destroy(simple_fsm_t *fsm)
{
	free(fsm->states);
	mr_fsm_destroy(fsm);
}

void simple_fsm_start(simple_fsm_t *fsm)
{
	mr_fsm_start(fsm);
}

void simple_fsm_start_at(simple_fsm_t *fsm, simple_fsm_state_t *state)
{
	mr_fsm_start_at(fsm, state);
}

void simple_fsm_stop(simple_fsm_t *fsm)
{
	mr_fsm_stop(fsm);
}

simple_fsm_state_t *simple_fsm_get_state(simple_fsm_t *fsm)
{
	return mr_fsm_get_state(fsm);
}

void simple_fsm_input_int(simple_fsm_t *fsm, int input)
{
	mr_fsm_input(fsm, MR_FSM_INPUT_INT, &input);
}

void simple_fsm_input_string(simple_fsm_t *fsm, const char *input)
{
	mr_fsm_input(fsm, MR_FSM_INPUT_STRING, (void *)input);
}

void simple_fsm_print(const simple_fsm_t *fsm)
{
	mr_fsm_print(fsm);
}

