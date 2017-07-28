cmake_minimum_required(VERSION 2.8)
project(TuringScaffold)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(MPI)
include_directories(${MPI_CXX_INCLUDE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_C_COMPILE_FLAGS}")

include_directories(src/simple_mpi)
include_directories(lib)

add_subdirectory(src/simple_mpi)
add_subdirectory(test)
