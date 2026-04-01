#include "include/FileSystem.hpp"
#include <sys/statvfs.h>
#include <fstream>
#include <sstream>

FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{
}

std::vector<MountInfo> FileSystem::get_mounted_file_system()
{
  std::vector<MountInfo> mount;
  std::ifstream          file("/proc/mounts");
  std::string            line;

  while (std::getline(file, line))
  {
    std::istringstream ss(line);
    MountInfo          mi;

    if (ss >> mi.device >> mi.mount_point >> mi.fs_type)
    {
      mount.push_back(mi);
    }
  }

  return mount;
}
