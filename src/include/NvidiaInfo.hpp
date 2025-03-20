#pragma once

#include <nvml.h>

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

  void checkNvml(nvmlReturn_t result, const char* msg);
  NvidiaData calculateNvml();
};
