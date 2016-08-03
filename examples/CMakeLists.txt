include_directories(../src/lib)
set(SIMPLE_FSM_SRCS
        simple_fsm.c
        simple_fsm_states.c
        simple_fsm.h
        simple_fsm_states.h
        )
add_library(simple_fsm ${SIMPLE_FSM_SRCS})
target_link_libraries(simple_fsm mr_fsm)