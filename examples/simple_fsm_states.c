#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mr_fsm_states.h"

// S1 functions forward declarations
mr_fsm_state_t *S1_next_state(mr_fsm_input_type_e input_type, void *input);
void S1_on_entry();
void S1_exec(mr_fsm_input_type_e input_type, void *input);
void S1_on_exit();

// S2 functions forward declarations
mr_fsm_state_t *S2_next_state(mr_fsm_input_type_e input_type, void *input);
void S2_on_entry();
void S2_exec(mr_fsm_input_type_e input_type, void *input);
void S2_on_exit();

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// FSM States

mr_fsm_state_t S1 = 
{
	.name = "S1",
	.next_state = S1_next_state,
	.on_entry = S1_on_entry,
	.exec = S1_exec,
	.on_exit = S1_on_exit,
};

mr_fsm_state_t S2 = 
{
	.name = "S2",
	.next_state = S2_next_state,
	.on_entry = S2_on_entry,
	.exec = S2_exec,
	.on_exit = S2_on_exit,
};

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// S1 default implementation functions

void S1_on_entry()
{
	printf("called function: S1_on_entry\n");
}

void S1_exec(mr_fsm_input_type_e input_type, void *input)
{
	printf("called function: S1_exec\n");
}

void S1_on_exit()
{
	printf("called function: S1_on_exit\n");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// S2 default implementation functions

void S2_on_entry()
{
	printf("called function: S2_on_entry\n");
}

void S2_exec(mr_fsm_input_type_e input_type, void *input)
{
	printf("called function: S2_exec\n");
}

void S2_on_exit()
{
	printf("called function: S2_on_exit\n");
}

// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
// State transition functions

mr_fsm_state_t *S1_next_state(mr_fsm_input_type_e input_type, void *input)
{
	if (input_type == MR_FSM_INPUT_INT)
	{
		int int_input = *(int *)input;
		switch (int_input)
		{
			case 0: return &S2;
			case 1: return &S1;
			default: abort();
		}
	} else if (input_type == MR_FSM_INPUT_STRING) {
	    char *str_input = (char *)input;
	    if (strcmp(str_input, "hello") == 0) {
			return &S2;
		} else if (strcmp(str_input, "goodbye") == 0) {
			return &S1;
		}
	}
	return 0;
}

mr_fsm_state_t *S2_next_state(mr_fsm_input_type_e input_type, void *input)
{
	if (input_type == MR_FSM_INPUT_INT)
	{
		int int_input = *(int *)input;
		switch (int_input)
		{
			case 0: return &S1;
			case 1: return &S2;
			default: abort();
		}
	}

	if (input_type == MR_FSM_INPUT_STRING) {
		char *str_input = (char *)input;
		if (strcmp(str_input, "hello") == 0) {
			return &S1;
		} else if (strcmp(str_input, "goodbye") == 0) {
			return &S2;
		}
	}
	return 0;
}

