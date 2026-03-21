#ifdef HAVE_ROCM_SMI
#include "include/AmdInfo.hpp"
#include "rocm_smi/rocm_smi.h"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>

AmdInfo::AmdInfo()
{
  status = rsmi_init(0);
  check_rocm_smi(status, "rsmi_init");
}

AmdInfo::~AmdInfo()
{
  rsmi_shut_down();
}

void AmdInfo::check_rocm_smi(rsmi_status_t status, std::string msg)
{
  if (status != RSMI_STATUS_SUCCESS) {
    std::cerr << "Error: " << msg << " - " << status << "\n";
    exit(0);
  }    
}

AmdData AmdInfo::calculate_rocm_smi()
{
  AmdData data;

  status = rsmi_num_monitor_devices(&num_devices);
  check_rocm_smi(status, "rsmi_num_monitor_devices");

  for (uint32_t i = 0; i < num_devices; i++)
  {
    char name[256];
    status = rsmi_dev_name_get(i, name, sizeof(name));
    check_rocm_smi(status, "rsmi_dev_name_get");

    uint32_t bussy = 0;
    rsmi_dev_busy_percent_get(i, &bussy);

    uint64_t total = 0, used = 0, free = 0;
    rsmi_dev_memory_total_get(i, RSMI_MEM_TYPE_VRAM, &total);
    rsmi_dev_memory_usage_get(i, RSMI_MEM_TYPE_VRAM, &used);
    free = total - used;

    data.name = name;
    data.tot_vram = total / (1024.0 * 1024.0 * 1024.0);
    data.used_vram = used / (1024.0 * 1024.0 * 1024.0);
    data.free_vram = free / (1024.0 * 1024.0 * 1024.0);
    data.gpu_util = bussy;
    data.mem_util = (data.tot_vram ? (data.used_vram / data.tot_vram) * 100 : (0));
    data.index = i;
  }

  return data;
}
#endif
