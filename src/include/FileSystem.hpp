#pragma once

#include <string>
#include <vector>

struct MountInfo
{
  std::string device, mount_point, fs_type;
};

class FileSystem
{
private:
public:
  FileSystem();
  ~FileSystem();

  std::vector<MountInfo> get_mounted_file_system();
};
