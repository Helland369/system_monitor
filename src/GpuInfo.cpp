#include "include/GpuInfo.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

GpuInfo::GpuInfo()
{
#ifdef HAVE_ROCM_SMI
  auto status = rsmi_init(0);
  check_rsmi(status, "rocm_smi");
#endif
#ifdef HAVE_NVML
  auto status = nvmlInit();
  check_nvml(status, "nvmlInit");
#endif
}

GpuInfo::~GpuInfo()
{
#ifdef HAVE_ROCM_SMI
  rsmi_shut_down();
#endif
#ifdef HAVE_NVML
  nvmlShutdown();
#endif
}

#ifdef HAVE_ROCM_SMI
void GpuInfo::check_rsmi(rsmi_status_t status, const std::string &msg)
{
  if (status != RSMI_STATUS_SUCCESS)
  {
    throw std::runtime_error("Error: " + msg + " - " + std::to_string(status) + "\n");
  }
}

std::vector<GpuData> GpuInfo::calculate_amd_data()
{
  std::vector<GpuData> data_vec;
  auto status = rsmi_num_monitor_devices(&num_devices);
  check_rsmi(status, "rsmi_num_monitor_devices");

  for (uint32_t i = 0; i < num_devices; i++)
  {
    GpuData data{};
    char    name[256];
    status = rsmi_dev_name_get(i, name, sizeof(name));
    check_rsmi(status, "rsmi_dev_name_get");

    uint32_t busy = 0;
    status = rsmi_dev_busy_percent_get(i, &busy);
    check_rsmi(status, "rsmi_dev_busy_percent_get");

    uint64_t total = 0, used = 0, free = 0;
    status = rsmi_dev_memory_total_get(i, RSMI_MEM_TYPE_VRAM, &total);
    check_rsmi(status, "rsmi_dev_memory_total_get");
    status = rsmi_dev_memory_usage_get(i, RSMI_MEM_TYPE_VRAM, &used);
    check_rsmi(status, "rsmi_dev_memory_usage_get");
    free = total - used;

    data.name = name;
    data.tot_vram = total / (1024.0 * 1024.0 * 1024.0);
    data.used_vram = used / (1024.0 * 1024.0 * 1024.0);
    data.free_vram = free / (1024.0 * 1024.0 * 1024.0);
    data.gpu_util = busy;
    data.mem_util = (data.tot_vram ? (data.used_vram / data.tot_vram) * 100 : 0);
    data.index = i;

    data_vec.push_back(data);
  }
  return data_vec;
}
#endif

#ifdef HAVE_NVML
void GpuInfo::check_nvml(nvmlReturn_t status, const std::string &msg)
{
  if (status != NVML_SUCCESS)
  {
    throw std::runtime_error("Error: " + msg + " - " + nvmlErrorString(status) + "\n");
  }
}

std::vector<GpuData> GpuInfo::calculate_nvidia_data()
{
  std::vector<GpuData> data_vec;
  nvmlDevice_t         device;
  auto status = nvmlDeviceGetCount(&num_devices);
  check_nvml(status, "nvmlDeviceGetCount");

  for (uint32_t i = 0; i < num_devices; i++)
  {
    GpuData data{};
    status = nvmlDeviceGetHandleByIndex(i, &device);
    check_nvml(status, "nvmlDeviceGetHandleByIndex");

    nvmlUtilization_t util;
    status = nvmlDeviceGetUtilizationRates(device, &util);
    check_nvml(status, "nvmlDeviceGetUtilizationRates");

    data.gpu_util = util.gpu;
    data.mem_util = util.memory;

    nvmlMemory_t mem;
    status = nvmlDeviceGetMemoryInfo(device, &mem);
    check_nvml(status, "nvmlDeviceGetMemoryInfo");

    data.tot_vram = mem.total / (1024.0 * 1024.0 * 1024.0);
    data.used_vram = mem.used / (1024.0 * 1024.0 * 1024.0);
    data.free_vram = mem.free / (1024.0 * 1024.0 * 1024.0);

    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    status = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
    check_nvml(status, "nvmlDeviceGetName");

    data.name = name;
    data.index = i;

    data_vec.push_back(data);
  }
  return data_vec;
}
#endif

std::vector<GpuData> GpuInfo::get_gpu_data()
{
  std::vector<GpuData> vec;
#ifdef HAVE_ROCM_SMI
  auto amd = calculate_amd_data();
  vec.insert(vec.end(), amd.begin(), amd.end());
#endif

#ifdef HAVE_NVML
  auto nvidia = calculate_nvidia_data();
  vec.insert(vec.end(), nvidia.begin(), nvidia.end());
#endif
  return vec;
}
