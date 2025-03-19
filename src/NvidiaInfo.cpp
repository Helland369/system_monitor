#include "include/NvidiaInfo.hpp"
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

void NvidiaInfo::printNvml()
{
  nvmlReturn_t result;
  result = nvmlInit();
  checkNvml(result, "Initializing NVML");

  nvmlDevice_t device;
  result = nvmlDeviceGetHandleByIndex(0, &device);
  checkNvml(result, "Getting GPU handle");

  unsigned int gpuUtil, memUtil;
  result = nvmlDeviceGetUtilizationRates(device, &utililization);
  checkNvml(result, "Getting GPU utilization");

  gpuUtil = utililization.gpu;
  memUtil = utililization.memory;

  nvmlMemory_t memory;
  result = nvmlDeviceGetMemoryInfo(device, &memory);
  checkNvml(result, "Getting GPU memory info");

  std::cout << "GPU util: " << gpuUtil << "%\n"
  << "Mem util: " << memUtil << "%\n"
  << "Tot Vram: " << memory.total / (1024 * 1024) << "MB\n"
  << "Used Vram: " << memory.used / (1024 * 1024) << "MB\n"
  << "Free Vram: " << memory.free / (1024 * 1024) << "MB\n";
}    
