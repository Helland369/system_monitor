#pragma once

#include <string>
#include <vector>


class CpuData
{
public:
  std::string label;
  unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest ,guest_nice;
};

class CpuPercentage
{
  public:
  std::string name;
  double persentageUsed;
};   

class CpuUsage
{

private:

public:
  CpuData get_cpu_data();

  std::vector<CpuData> get_cpu_tread_data();
  
  double calculate_cpu_usage(const CpuData &prev, const CpuData &curr);

  std::vector<CpuPercentage> calculate_cpu_thread_usage(std::vector<CpuData> &curr, std::vector<CpuData> &prev);
};
