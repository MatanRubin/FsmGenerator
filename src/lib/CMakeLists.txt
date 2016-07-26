set(CINFRA_SRCS
        mr_common.c
        mr_mutex.c
        mr_thread.c
        mr_cond.c
        mr_cond.h
        )
add_library(mr_infra ${CINFRA_SRCS})

set(MR_FSM_SRCS
        mr_fsm.c
        mr_fsm.h
        mr_fsm_states.h
        )
add_library(mr_fsm ${MR_FSM_SRCS})
target_link_libraries(mr_fsm mr_infra)

#set(SOURCES
#        simple_fsm.ag.c
#        simple_fsm.ag.h
#        simple_fsm_states.ag.c
#        simple_fsm_states.ag.h
#        )
#add_library(simple_fsm ${SOURCES})
#target_link_libraries(simple_fsm mr_fsm mr_infra)
