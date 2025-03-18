#pragma once

#include <cstdint>

class MemData
{
public:
  std::uint64_t memTotal, memFree, memAvailable;
};   

class MemInfo
{
private:
public:

  MemInfo();
  ~MemInfo();

  MemData get_mem_data();
};
