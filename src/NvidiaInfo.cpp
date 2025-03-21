#include "include/NvidiaInfo.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>

NvidiaInfo::NvidiaInfo() {}

NvidiaInfo::~NvidiaInfo() {}

void NvidiaInfo::checkNvml(nvmlReturn_t result, const char* msg)
{
  if (result != NVML_SUCCESS)
  {
    std::cerr << "Error: " << msg << " - " << nvmlErrorString(result) << "\n";
    exit(0);
  }
}

NvidiaData NvidiaInfo::calculateNvml()
{
  NvidiaData data;

  nvmlReturn_t result;
  result = nvmlInit();
  checkNvml(result, "Initializing NVML");

  nvmlDevice_t device;
  result = nvmlDeviceGetHandleByIndex(0, &device);
  checkNvml(result, "Getting GPU handle");

  std::uint16_t gpuUtil, memUtil;
  result = nvmlDeviceGetUtilizationRates(device, &utililization);
  checkNvml(result, "Getting GPU utilization");

  data.gpuUtil = utililization.gpu;
  data.memUtil = utililization.memory;

  nvmlMemory_t memory;
  result = nvmlDeviceGetMemoryInfo(device, &memory);
  checkNvml(result, "Getting GPU memory info");

  data.totVram = memory.total / (1024.0 * 1024 * 1024);
  data.usedVram = memory.used / (1024.0 * 1024 * 1024);
  data.freeVram = memory.free / (1024.0 * 1024 * 1024);

  return data;
}    
