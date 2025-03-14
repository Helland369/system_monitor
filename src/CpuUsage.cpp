#include "CpuUsage.hpp"

#include <cstddef>
#include <fstream>
#include <sstream>
#include <vector>

std::vector<CpuData> CpuUsage::get_cpu_tread_data()
{
  std::ifstream file("/proc/stat");
  std::string line;
  std::vector<CpuData> cpu_data;

  if (file.is_open())
  {
    while (std::getline(file, line)) {
      
      std::istringstream ss(line);
      CpuData data;

      ss >> data.label >> data.user >> data.nice >> data.system >> data.idle >> data.iowait >> data.irq >> data.softirq >> data.steal;

      if (data.label.find("cpu") != std::string::npos)
      {
        cpu_data.push_back(data);
      }
    }
  }

  return cpu_data;
}

std::vector<CpuPercentage> CpuUsage::calculate_cpu_thread_usage(std::vector<CpuData> &curr, std::vector<CpuData> &prev)
{
  std::vector<CpuPercentage> percentages;

  if (curr.size() != prev.size())
  {
    return percentages; // return empty object if size is not equal.
  }
  
  for (size_t i = 0; i < curr.size(); i++)
  {
      CpuPercentage percentage;
      percentage.name = curr[i].label;

      unsigned long long prevIdle = prev[i].idle + prev[i].iowait;
      unsigned long long currIdle = curr[i].idle + curr[i].iowait;

      unsigned long long prevTot = prev[i].user + prev[i].nice + prev[i].system + prev[i].idle + prev[i].iowait + prev[i].irq + prev[i].softirq + prev[i].steal;

      unsigned long long currTot = curr[i].user + curr[i].nice + curr[i].system + curr[i].idle + curr[i].iowait + curr[i].irq + curr[i].softirq + curr[i].steal;

      double totalDelta = currTot - prevTot;
      double idleDelta = currIdle - prevIdle;

      percentage.persentageUsed = (1.0 - (idleDelta / totalDelta)) * 100.0;

      percentages.push_back(percentage);
  }
  return percentages;
}
