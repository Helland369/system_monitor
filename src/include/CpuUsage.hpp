#pragma once

#include <string>
#include <vector>


class CpuData
{
public:
  std::string label;
  unsigned long long user, nice, system, idle, iowait, irq, softirq,
        steal;
};

class CpuUsage
{

private:

public:
  CpuData get_cpu_data();

  std::vector<CpuData> get_cpu_tread_data();
  
  double calculate_cpu_usage(const CpuData &prev, const CpuData &curr);

  double calculate_cpu_thread_usage(const CpuData &prev, const CpuData &curr);
};
