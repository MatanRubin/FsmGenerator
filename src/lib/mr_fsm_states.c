#include <stdio.h>
#include <stdlib.h>

#include "mr_fsm_states.h"
#include "mr_fsm.h"

mr_fsm_state_t *S1_next_state(mr_fsm_input_type_e input_type, void *input);
mr_fsm_state_t *S2_next_state(mr_fsm_input_type_e input_type, void *input);
void S1_on_entry();
void S1_exec(mr_fsm_input_type_e input_type, void *input);
void S1_on_exit();
void S2_on_entry();
void S2_exec(mr_fsm_input_type_e input_type, void *input);
void S2_on_exit();

mr_fsm_state_t S1 = {
        .name = "S1",
        .next_state = S1_next_state,
        .on_entry = S1_on_entry,
        .exec = S1_exec,
        .on_exit = S1_on_exit,
};

mr_fsm_state_t S2 = {
        .name = "S2",
        .next_state = S2_next_state,
        .on_entry = S2_on_entry,
        .exec = S2_exec,
        .on_exit = S2_on_exit,
};

void S1_on_entry() {
    printf("entring state 'S1'\n");
}

void S1_exec(mr_fsm_input_type_e input_type, void *input) {
    int in = *(int *)input;
    printf("executing state 'S1'. input=%d\n", in);
}

void S1_on_exit() {
    printf("exiting state 'S1'\n");
}

void S2_on_entry() {
    printf("entring state 'S2'\n");
}

void S2_exec(mr_fsm_input_type_e input_type, void *input) {
    int in = *(int *)input;
    printf("executing state 'S2'. input=%d\n", in);
}

void S2_on_exit() {
    printf("exiting state 'S2'\n");
}


mr_fsm_state_t *S1_next_state(mr_fsm_input_type_e input_type, void *input) {
    if (input_type == MR_FSM_INPUT_INT) {
        int int_input = *(int *)input;
        switch (int_input) {
            case 0: return &S2;
            case 1: return &S1;
            default: abort();
        }
    }

    return 0;
}

mr_fsm_state_t *S2_next_state(mr_fsm_input_type_e input_type, void *input) {
    if (input_type == MR_FSM_INPUT_INT) {
        int int_input = *(int *)input;
        switch (int_input) {
            case 0: return &S1;
            case 1: return &S2;
            default: abort();
        }
    }

    return 0;
}
