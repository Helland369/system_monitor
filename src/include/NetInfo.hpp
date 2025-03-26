#include <string>

class IpData
{
public:
  std::string name, addr;
}; 

class NetInfo
{
private:
public:
  NetInfo();
  ~NetInfo();

  IpData get_ip_address();
};
