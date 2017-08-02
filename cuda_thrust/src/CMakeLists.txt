cuda_add_executable(cuda_thrust cuda_main.cu )
cuda_add_library(cuda_thrust_lib helpers.cu)
target_link_libraries(cuda_thrust cuda_thrust_lib)
