#pragma once

#include <cstdint>
#include <string>
#include <vector>

class IpData
{
public:
  std::string name, addr;
};

class NetStats
{
public:
  std::string iface;
  uint64_t rx_bytes, tx_bytes;
};

class NetInfo
{
private:
public:
  NetInfo();
  ~NetInfo();

  IpData get_ip_address();

  std::vector<NetStats> get_network_stats();
};
