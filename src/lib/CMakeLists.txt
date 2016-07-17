set(CINFRA_SRCS
        mr_common.c
        mr_mutex.c
        mr_thread.c mr_cond.c mr_cond.h)
add_library(mr_infra ${CINFRA_SRCS})

set(SOURCES mr_fsm.c mr_fsm.h mr_fsm_states.c mr_fsm_states.h)
add_library(mr_fsm ${SOURCES})
target_link_libraries(mr_fsm mr_infra)