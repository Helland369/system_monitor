#include "include/NetInfo.hpp"
#include <cstdio>
#include <string>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


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
