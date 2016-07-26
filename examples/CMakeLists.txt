include_directories(../src/lib)
set(SIMPLE_FSM_SRCS
        simple_fsm.ag.c
        simple_fsm_states.ag.c
        simple_fsm.ag.h
        simple_fsm_states.ag.h
        )
add_library(simple_fsm ${SIMPLE_FSM_SRCS})
target_link_libraries(simple_fsm mr_fsm)