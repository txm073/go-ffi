#pragma once

// TODO: implement zipping

#include <string>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include "../lib/shared.h"

namespace hn::utils {

class ZipArchive {
private:
  go_handle_t handle;
  bool opened = false;
  char* zipBuffer;
  size_t zipLength;
  void checkOpen();
  void checkReturn(int32_t ret);
public:
  ZipArchive();
  ~ZipArchive();
  void open(const std::string& path);
  void openBuffer(std::vector<char>& buffer);
  void openBuffer(char* buffer, size_t length);
  void save();
  std::vector<char> saveBuffer();
  std::vector<char> read(const std::string& filePath);
  void extractAll(const std::string& dst = "");
  std::vector<char> extractFile(const std::string& filePath);
  void extractFileToDisk(const std::string& filePath, const std::string& dstPath);
  void write(const std::string& filePath, std::vector<char>& data);
  void write(const std::string& filePath, char* data, size_t length);
  void writeFile(const std::string& filePath, const std::string& srcPath);
  void writeDir(const std::string& dirPath, const std::string& srcDir);
};

}
