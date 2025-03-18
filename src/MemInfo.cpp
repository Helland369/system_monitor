#include "MemInfo.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>


MemInfo::MemInfo()
{
  
}

MemInfo::~MemInfo()
{

}

MemData MemInfo::get_mem_data()
{
  std::ifstream file("/proc/meminfo");
  std::string line;
  MemData data;

  if (file.is_open())
  {
    for (int i = 0; i < 3; i++)
    {
      if (std::getline(file, line))
      {
        std::istringstream ss(line);
        std::string name;
        std::uint64_t value;
        std::string units;

        ss >> name >> value >> units;

        if (name == "MemTotal:")
        {
          data.memTotal = value;
        }
        else if (name == "MemFree:")
        {
          data.memFree = value;
        }
        else if (name == "MemAvailable:")
        {
          data.memAvailable = value;
        }
      }
    }
  }
  return data;
}
