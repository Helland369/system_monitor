#include <nvml.h>

class NvidiaInfo
{
private:
  unsigned int x = 1;
  nvmlUtilization_t utililization;
public:
  
  NvidiaInfo();
  ~NvidiaInfo();

  void checkNvml(nvmlReturn_t result, const char* msg);
  void printNvml();
};
