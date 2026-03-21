#pragma once
#ifdef HAVE_ROCM_SMI
#include <cstdint>
#include <string>

#include <rocm_smi/rocm_smi.h>

struct AmdData
{
  std::string name;
  double gpu_util, mem_util, tot_vram, free_vram, used_vram;
  uint32_t index;
};

class AmdInfo
{
private:
  rsmi_status_t status;
  uint32_t num_devices;
public:

  AmdInfo();
  ~AmdInfo();
  
  void check_rocm_smi(rsmi_status_t status, std::string msg);
  
  AmdData calculate_rocm_smi();
};

#endif
