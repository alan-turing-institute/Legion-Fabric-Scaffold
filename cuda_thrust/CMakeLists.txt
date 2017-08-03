list(APPEND CUDA_NVCC_FLAGS -arch sm_35;-std=c++11;--expt-extended-lambda)
include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
