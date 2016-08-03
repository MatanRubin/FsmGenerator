#ifndef MR_FSM_STATE_H
#define MR_FSM_STATE_H

typedef enum mr_fsm_input_type {
    MR_FSM_INPUT_NONE,
    MR_FSM_INPUT_INT,
    MR_FSM_INPUT_STRING,
    MR_FSM_INPUT_PATTERN, /* future */
    MR_FSM_INPUT_BLOB, /* future */
    MR_FSM_INPUT_LAST
} mr_fsm_input_type_e;


typedef struct mr_fsm_state mr_fsm_state_t;
typedef mr_fsm_state_t *(*next_state_f)(mr_fsm_input_type_e input_type, void *input);
typedef void (*on_entry_f)();
typedef void (*exec_f)(mr_fsm_input_type_e input_type, void *input);
typedef void (*on_exit_f)();

typedef struct mr_fsm_state {
    const char *name;
    next_state_f next_state;
    on_entry_f on_entry;
    exec_f exec;
    on_exit_f on_exit;
} mr_fsm_state_t;

#endif
