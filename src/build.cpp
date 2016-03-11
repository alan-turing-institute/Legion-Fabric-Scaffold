#include "build.h"

#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ios>

void Builder::EnumerateDevices(){
  cl_uint numPlatforms;
  cl_uint numDevices;

  result = clGetPlatformIDs(0, NULL, &numPlatforms);
  platforms.resize(numPlatforms);
  result |= clGetPlatformIDs(numPlatforms, &platforms[0], NULL);

  std::cout << "Found " << numPlatforms << " platforms." << std::endl;

  if(result != CL_SUCCESS) throw ClException(1) ;

  result = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
  deviceIDs.resize(numDevices);
  result |= clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices,
    &deviceIDs[0], NULL);

  std::cout << "Found " << numDevices << " devices." << std::endl;

  if(result != CL_SUCCESS) throw ClException(2);

}

cl_device_type Builder::GetDeviceType(cl_uint selectedDevice){

  cl_device_type deviceType;

  result = clGetDeviceInfo(deviceIDs[selectedDevice],
    CL_DEVICE_TYPE, sizeof(cl_device_type),
    &deviceType, NULL);

  if(result != CL_SUCCESS)
  {
        throw ClException(3);
  }

  return deviceType;
}

cl_uint Builder::GetDeviceCores(cl_uint selectedDevice){
  cl_uint core_count;

  result = clGetDeviceInfo(deviceIDs[selectedDevice],
    CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint),
    &core_count, NULL);

  if(result != CL_SUCCESS)
  {
        throw ClException(4);
  }

  return core_count;
}

std::string Builder::GetDeviceName(cl_uint selectedDevice){
  size_t deviceNameLength;
  result |= clGetDeviceInfo(deviceIDs[selectedDevice], CL_DEVICE_NAME,
    0, NULL, &deviceNameLength
  );

  char * deviceNameBuffer = new char[deviceNameLength];

  result |= clGetDeviceInfo(deviceIDs[selectedDevice], CL_DEVICE_NAME,
    deviceNameLength, deviceNameBuffer, NULL);

  std::string deviceName(deviceNameBuffer);
  delete[] deviceNameBuffer;

  if(result != CL_SUCCESS)
  {
      throw ClException(5);
  }

  return deviceName; // RVO default move semantics
}

void Builder::DescribeDevice(){

  std::string deviceTypeString;

  cl_device_type deviceType = GetDeviceType(selected_device_index);

  switch(deviceType)
  {
      case CL_DEVICE_TYPE_CPU:
          deviceTypeString = "Processor";
          break;
      case CL_DEVICE_TYPE_GPU:
          deviceTypeString = "Graphics card";
          break;
      case CL_DEVICE_TYPE_ACCELERATOR:
          deviceTypeString = "Accelerator";
          break;
      default:
          deviceTypeString = "NONE";
          break;
  }

  std::cout << "Selected Device: "
            << GetDeviceName(selected_device_index)
            << " ( " << deviceTypeString << " with "
            << GetDeviceCores(selected_device_index) << " cores )"
            << std::endl;

}

void Builder::SelectDevice(cl_uint selectedDevice){

  selected_device_index = selectedDevice;
  selected_device_id = deviceIDs[selectedDevice];

  cl_command_queue_properties commandQueueProperties = CL_QUEUE_PROFILING_ENABLE;
  context = clCreateContext(properties, 1, &selected_device_id,
    &onOpenCLError, NULL, &result);

  if(result != CL_SUCCESS) throw ClException(6);

  // build command queue
  commands = clCreateCommandQueue(context,
    selected_device_id,
    commandQueueProperties, &result);
  if(result != CL_SUCCESS) throw ClException(7);

}

cl_program Builder::CreateProgram(const std::string &kernel_path){

   // Slurp kernel file
   std::ifstream kernel_file(kernel_path);

   if (kernel_file.fail()) throw std::ios_base::failure("Missing file");

   std::stringstream buffer;
   buffer << kernel_file.rdbuf() << std::flush;
   program_buffers.push_back(buffer.str());

   const char * buffer_contents = program_buffers.back().c_str();


   std::cout << "Creating program: " << kernel_path << std::endl;
   cl_program program = clCreateProgramWithSource(context, 1,
     &buffer_contents, NULL, &result);
   if(result != CL_SUCCESS) exit(8);
   return program;

}


void Builder::Program(const std::string &path){
  cl_program program = CreateProgram(path);
  programs.push_back(program);
}

void Builder::Header(const std::string &path, const std::string &name){
  cl_program program = CreateProgram(path);
  headers.push_back(program);
  header_names.push_back(name);
}

void Builder::BuildLog(cl_program program){
  size_t log_length;

  clGetProgramBuildInfo(program, selected_device_id, CL_PROGRAM_BUILD_LOG,
    0, NULL, &log_length
  );

  char build_log[log_length];

  clGetProgramBuildInfo(program, selected_device_id, CL_PROGRAM_BUILD_LOG,
      log_length, build_log, NULL);

  std::cout << "Build log" << std::endl;
  std::cout << build_log << std::endl;
  throw ClException(5);
}

cl_kernel Builder::CreateKernel(const std::string &kernel_name){

    std::vector<const char* > headers_buffer(headers.size());
    std::transform(header_names.begin(),
                   header_names.end(),
                   headers_buffer.begin(),
                   [](std::string &s) { return s.c_str(); });
    std::cout << "Compiling programs" << std::endl;
    for (auto program: programs) {
      result = clCompileProgram(program, 0, NULL, NULL, headers.size(),
        &headers[0], &headers_buffer[0], NULL, NULL);

      if(result != CL_SUCCESS) BuildLog(program);
    }

    std::cout << "Linking programs" << std::endl;
    cl_program program = clLinkProgram(context, 0, NULL, NULL, programs.size(),
                         &programs[0],
                          NULL, NULL, &result);
    if(result != CL_SUCCESS) BuildLog(program);
    std::cout << "Linked programs" << std::endl;
    cl_kernel chol_gpu;

    chol_gpu = clCreateKernel(program, kernel_name.c_str(), &result);
    if(result != CL_SUCCESS) throw ClException(9);
    std::cout << "Created kernel" << std::endl;
    return chol_gpu;
}

void Builder::DispatchKernel(cl_kernel kernel, size_t work_count){
  std::cout << "Launching kernel" << std::endl;
  cl_event  kernelExecEvent;

  result = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &work_count, NULL,
     0, NULL, &kernelExecEvent);

  clWaitForEvents(1, &kernelExecEvent);
  if(result != CL_SUCCESS) throw ClException(10);

  std::cout << "Kernel completed in " << Profile(kernelExecEvent)
            << " ms" << std::endl;
}

cl_double Builder::Profile(cl_event event){
  cl_ulong start = 0, end = 0;

  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START,
    sizeof(cl_ulong), &start, NULL);
  clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END,
    sizeof(cl_ulong), &end, NULL);

  return (cl_double)(end - start)*(cl_double)(1e-09);
}

cl_mem Builder::AllocateDeviceMemory(cl_mem_flags flags, size_t count,
  void * source_data){
    std::cout << "Allocating device memory" << std::endl;
    cl_mem data = clCreateBuffer(context, flags, count, source_data, &result);
    if(result != CL_SUCCESS) throw ClException(11);
    return data;
}

void Builder::DeviceToHost(cl_mem device_mem, void* host_mem, size_t count){
  std::cout << "Copying from device to host" << std::endl;
  cl_event readResultsEvent;
  clEnqueueReadBuffer(commands, device_mem, CL_TRUE, 0,
    count, host_mem, 0, NULL, &readResultsEvent);

  clWaitForEvents(1, &readResultsEvent);

  if(result != CL_SUCCESS) throw ClException(12);
  std::cout << "Device to Host copy took " << Profile(readResultsEvent)
            << " ms" << std::endl;
}

Builder::Builder():
  properties(NULL),
  platforms(0),
  deviceIDs(0)
{
  EnumerateDevices();


}
