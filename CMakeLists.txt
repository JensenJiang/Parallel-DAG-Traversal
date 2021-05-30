cmake_minimum_required(VERSION 2.8)
project(Parallel_DAG_Traversal)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

add_executable(serial src/serial_main.cpp src/config.h src/buildDAG.cpp src/LoserTree.cpp)
add_executable(completor src/competor.cpp src/buildDAG.h src/buildDAG.cpp)
add_executable(graph_ge tools/graph_ge.c)
add_executable(parallel src/parallel_main.cpp src/buildDAG.cpp src/LoserTree.cpp)