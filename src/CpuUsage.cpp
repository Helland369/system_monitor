#include "CpuUsage.hpp"

#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

CpuUsage::CpuUsage() {}

CpuUsage::~CpuUsage() {}

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
      auto x = curr[i];
      auto y = prev[i];
      CpuPercentage percentage;
      percentage.name = curr[i].label;

      std::uint64_t prevIdle = y.idle + y.iowait;
      std::uint64_t currIdle = x.idle + x.iowait;

      std::uint64_t prevTot = y.user + y.nice + y.system + y.idle + y.iowait + y.irq + y.softirq + y.steal;

      std::uint64_t currTot = x.user + x.nice + x.system + x.idle + x.iowait + x.irq + x.softirq + x.steal;

      double totalDelta = currTot - prevTot;
      double idleDelta = currIdle - prevIdle;

      // TODO S = C
      percentage.percentageUsed = (1.0 - (idleDelta / totalDelta)) * 100.0;

      percentages.push_back(percentage);
  }
  return percentages;
}

std::string CpuUsage::get_cpu_name()
{
  std::ifstream file("/proc/cpuinfo");
  std::string line;
  std::string cpuName;

  if (file.is_open())
  {
    while (std::getline(file, line))
    {
      if (line.find("model name") != std::string::npos)
      {
        size_t pos = line.find(":");
        if (pos != std::string::npos)
        {
          cpuName = line.substr(pos + 2);
          break;
        }
      }
    }
  }
  return cpuName;
}    
