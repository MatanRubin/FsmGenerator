#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include "../../examples/simple_fsm.ag.h"
#include "../lib/mr_common.h"

extern

int main(int argc, char **argv) {
    simple_fsm_t fsm;

    mr_set_debug_mode(false);
    printf("starting!\n");
    simple_fsm_init(&fsm);
    simple_fsm_print(&fsm);

    simple_fsm_start(&fsm);
    simple_fsm_input(&fsm, 0);
    simple_fsm_input(&fsm, 1);
    simple_fsm_input(&fsm, 1);
    simple_fsm_input(&fsm, 1);
    simple_fsm_input(&fsm, 0);
    simple_fsm_input(&fsm, 0);
    simple_fsm_input(&fsm, 1);
    simple_fsm_input(&fsm, 1);
    simple_fsm_input(&fsm, 0);
    simple_fsm_input(&fsm, 1);
    simple_fsm_input(&fsm, 0);
    simple_fsm_input(&fsm, 1);

    assert(strcmp(simple_fsm_get_state(&fsm)->name, "S2") == 0);
    simple_fsm_stop(&fsm);

    simple_fsm_destroy(&fsm);
    printf("finished!\n");
}