#pragma once

#include <cstdint>

class MemData
{
public:
  std::uint64_t mem_total, mem_free, mem_available;
};

class MemInfo
{
private:
public:
  MemInfo();
  ~MemInfo();

  MemData get_mem_data();
};
