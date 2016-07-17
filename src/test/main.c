#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include "../lib/mr_fsm.h"
#include "../lib/mr_common.h"

extern

int main(int argc, char **argv) {
    mr_fsm_t fsm;

    mr_set_debug_mode(true);
    printf("starting!\n");
    mr_fsm_init(&fsm, "simple_fsm");
    mr_fsm_print(&fsm);

    mr_fsm_start(&fsm);
    mr_fsm_input(&fsm, 0);
    mr_fsm_input(&fsm, 1);
    mr_fsm_input(&fsm, 1);
    mr_fsm_input(&fsm, 1);
    mr_fsm_input(&fsm, 0);
    mr_fsm_input(&fsm, 0);
    mr_fsm_input(&fsm, 1);
    mr_fsm_input(&fsm, 1);
    mr_fsm_input(&fsm, 0);
    mr_fsm_input(&fsm, 1);
    mr_fsm_input(&fsm, 0);
    mr_fsm_input(&fsm, 1);

    assert(strcmp(mr_fsm_get_state(&fsm)->name, "S2") == 0);
    mr_fsm_stop(&fsm);

    mr_fsm_destroy(&fsm);
    printf("finished!\n");
}