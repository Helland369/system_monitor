#include "include/FileSystem.hpp"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <sys/statvfs.h>

FileSystem::FileSystem() {}

FileSystem::~FileSystem() {}

std::vector<MountInfo> FileSystem::get_mounted_file_system()
{
  std::vector<MountInfo> mount;
  std::ifstream file("/proc/mounts");
  std::string line;

  while (std::getline(file, line))
  {
    std::istringstream ss(line);
    MountInfo mi;

    if (ss >> mi.device >> mi.mountPoint >> mi.fsType)
    {
      mount.push_back(mi);
    }
  }

  return mount;
}
