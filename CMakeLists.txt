cmake_minimum_required(VERSION 3.8)
project(Parallel_DAG_Traversal)

set(CMAKE_CXX_STANDARD 11)

find_package(openmp REQUIRED)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


set(SOURCE_FILES src/serial_main.cpp src/config.h src/buildDAG.cpp)

add_executable(Parallel_DAG_Traversal ${SOURCE_FILES})
add_executable(completor src/competor.cpp src/buildDAG.h src/buildDAG.cpp)
add_executable(graph_ge tools/graph_ge.c)