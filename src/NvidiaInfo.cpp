#include "include/NvidiaInfo.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>

NvidiaInfo::NvidiaInfo() {}

NvidiaInfo::~NvidiaInfo() {}

void NvidiaInfo::check_nvml(nvmlReturn_t result, const char* msg)
{
  if (result != NVML_SUCCESS)
  {
    std::cerr << "Error: " << msg << " - " << nvmlErrorString(result) << "\n";
    exit(0);
  }
}

NvidiaData NvidiaInfo::calculate_nvml()
{
  NvidiaData data;

  nvmlReturn_t result;
  result = nvmlInit();
  check_nvml(result, "Initializing NVML");

  nvmlDevice_t device;
  result = nvmlDeviceGetHandleByIndex(0, &device);
  check_nvml(result, "Getting GPU handle");

  std::uint16_t gpuUtil, memUtil;
  result = nvmlDeviceGetUtilizationRates(device, &utililization);
  check_nvml(result, "Getting GPU utilization");

  data.gpuUtil = utililization.gpu;
  data.memUtil = utililization.memory;

  nvmlMemory_t memory;
  result = nvmlDeviceGetMemoryInfo(device, &memory);
  check_nvml(result, "Getting GPU memory info");

  data.totVram = memory.total / (1024.0 * 1024 * 1024);
  data.usedVram = memory.used / (1024.0 * 1024 * 1024);
  data.freeVram = memory.free / (1024.0 * 1024 * 1024);

  return data;
}

std::string NvidiaInfo::get_nvidia_gpu_name()
{
  char name[NVML_DEVICE_NAME_BUFFER_SIZE];

  nvmlReturn_t result;
  result = nvmlInit();
  check_nvml(result, "Initializing NVML");

  nvmlDevice_t device;
  result = nvmlDeviceGetHandleByIndex(0, &device);
  check_nvml(result, "Getting GPU handle");

  result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
  check_nvml(result, "Getting GPU name");

  std::string Name = name;

  return Name;
}    
