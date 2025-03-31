#include "include/FileSystem.hpp"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
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

void FileSystem::print_filesystem(const std::string& path)
{
  struct statvfs stat;

  if (statvfs(path.c_str(), &stat) != 0)
  {
    perror(("statvfs error for: " + path).c_str());
    return;
  }

  uint64_t tot = stat.f_blocks * stat.f_frsize;
  uint64_t free = stat.f_bavail * stat.f_frsize;
  uint64_t used = tot - free;

  std::cout << "mnt point: " << path << "\n"
  << "tot: " << tot << "\n"
  << "free: " << free << "\n"
  << "used: " << used << "\n";
}
