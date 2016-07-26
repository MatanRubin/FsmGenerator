include_directories(../lib)
add_executable(FsmGenerator main.c)
target_link_libraries(FsmGenerator simple_fsm)
