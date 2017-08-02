add_executable(simple_mpi parallel_main.cpp )
add_library(simple_mpi_lib Model.cpp)
target_link_libraries(simple_mpi simple_mpi_lib  ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES})
