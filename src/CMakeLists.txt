set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99")

enable_testing()
include_directories(include)
add_subdirectory(lib)
add_subdirectory(test)