#include <cstdint>
#include <string>
#include <vector>

#ifdef HAVE_ROCM_SMI
#include <rocm_smi/rocm_smi.h>
#endif
#ifdef HAVE_NVML
#include <nvml.h>
#endif

struct GpuData
{
  std::string name;
  double      gpu_util, mem_util, tot_vram, free_vram, used_vram;
  uint32_t    index;
};

class GpuInfo
{
private:
#ifdef HAVE_ROCM_SMI
   void                 check_rsmi(rsmi_status_t status, const std::string &msg);
  std::vector<GpuData> calculate_amd_data();
#endif
#ifdef HAVE_NVML
  void                 check_nvml(nvmlReturn_t status, const std::string &msg);
  std::vector<GpuData> calculate_nvidia_data();
#endif
  uint32_t num_devices{0};

public:
  GpuInfo();
  ~GpuInfo();

  std::vector<GpuData> get_gpu_data();
};
