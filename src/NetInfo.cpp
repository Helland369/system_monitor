#include "include/NetInfo.hpp"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <vector>


NetInfo::NetInfo() {}

NetInfo::~NetInfo() {}

IpData NetInfo::get_ip_address()
{
  struct ifaddrs *ifaddr, *ifa;
  char addr[INET_ADDRSTRLEN];
  IpData data;
  
  if (getifaddrs(&ifaddr) == -1)
  {
    perror("getifaddres");
  }

  for (auto* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next)
  {
    if (!ifa->ifa_addr) continue;
    if (ifa->ifa_addr->sa_family == AF_INET) // only ipv4
    {
      void* tmpAddr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
      inet_ntop(AF_INET, tmpAddr, addr, INET_ADDRSTRLEN);

      data.addr = addr;

      if (ifa->ifa_name)
      {
        data.name = ifa->ifa_name;
      }
      else
      {
        data.name = "Unknown";
      }
    }
  }

  freeifaddrs(ifaddr);
  return data;
}

std::vector<NetStats> NetInfo::get_network_stats()
{
  std::vector<NetStats> stats;
  std::ifstream file("/proc/net/dev");
  std::string line;

  if (!file.is_open())
  {
    std::cerr << "Failed to open /proc/net/dev" << "\n";
    return {};
  }
  
  // skipping header lines
  std::getline(file, line);
  std::getline(file, line);

  while (std::getline(file, line))
  {
    std::istringstream ss(line);
    std::string iface;
    std::getline(ss, iface, ':');
    iface.erase(0, iface.find_first_not_of(" "));
    
    uint64_t rx_bytes, tx_bytes, temp;
    ss >> rx_bytes;

    // skipping rx fields
    for (int i = 0; i < 7; i++) ss >> temp;
    ss >> tx_bytes;

    stats.push_back({iface, rx_bytes, tx_bytes});
  }

  return stats;;
}   
