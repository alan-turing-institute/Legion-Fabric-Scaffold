cmake_minimum_required(VERSION 2.8)
project(TuringScaffold)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(MPI)
include_directories(${MPI_CXX_INCLUDE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_C_COMPILE_FLAGS}")

include_directories(lib)

add_subdirectory(simple_mpi)

find_package(CUDA)

if(CUDA_FOUND)
  add_subdirectory(cuda_thrust)
endif()
