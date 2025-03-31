#pragma once

#include <string>
#include <vector>

class MountInfo
{
public:
  std::string device, mountPoint, fsType;
};

class FileSystem
{
private:
public:
  
  FileSystem();
  ~FileSystem();

  std::vector<MountInfo> get_mounted_file_system();

  void print_filesystem(const std::string& path);
};
