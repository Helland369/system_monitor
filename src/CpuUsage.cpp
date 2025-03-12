#include "CpuUsage.hpp"

#include <fstream>
#include <sstream>

CpuData CpuUsage::get_cpu_data()
{
  std::ifstream file("/proc/stat"); // open file
  std::string line; // store some thing like this; cpu 1074181 43 264124 24623195 9219 96703 47641 0 0 0 // messured in jiffies
  CpuData data;

  if (file.is_open())
  {
    std::getline(file, line);    // read file
    std::istringstream ss(line); // input string stream from line, this make it possible to break it down.
    std::string cpu; // store the "cpu" part we get from /proc/stat
    // stram extraction; break down the read string to variables
    ss >> cpu >> data.user >> data.nice >> data.system >> data.idle >>
          data.iowait >> data.irq >> data.softirq >> data.steal;
  }

  return data;
}

double CpuUsage::calculate_cpu_usage(const CpuData &prev, const CpuData &curr)
{
  unsigned long long prevIdle = prev.idle + prev.iowait;
  unsigned long long currIdle = curr.idle + curr.iowait;

  unsigned long long prevTotal = prev.user + prev.nice + prev.system + prev.idle + prev.irq + prev.softirq + prev.steal;

  unsigned long long currTotal = curr.user + curr.nice + curr.system + curr.idle + curr.irq + curr.softirq + curr.steal;

  double totalDelta = currTotal - prevTotal;
  double idleDelta = currIdle - prevIdle;

  return (1.0 - (idleDelta / totalDelta)) * 100.0;
}

// TODO use this instead of the above functions
std::vector<CpuData> CpuUsage::get_cpu_tread_data()
{
  std::ifstream file("/proc/stat");
  std::string line;
  std::vector<CpuData> cpu_data;

  if (file.is_open())
  {
    while (std::getline(file, line)) {
      
      std::istringstream ss(line);
      //std::string thread;
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

//double CpuUsage::calculate_cpu_thread_usage(const CpuData &prev, const CpuData &curr) {}
