#pragma once

#include <nvml.h>
#include <string>

class NvidiaData
{
public:
  double gpuUtil, memUtil, totVram, usedVram, freeVram;
};   


class NvidiaInfo
{
private:
  unsigned int x = 1;
  nvmlUtilization_t utililization;
public:
  
  NvidiaInfo();
  ~NvidiaInfo();

  void check_nvml(nvmlReturn_t result, const char* msg);

  NvidiaData calculate_nvml();

  std::string get_nvidia_gpu_name();
};
