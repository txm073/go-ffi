#include "zip.hpp"
#include "../lib/goapi.h"

#include <stdexcept>
#include <fstream>

namespace hn::utils {

ZipArchive::ZipArchive() {
  handle = INVALID_HANDLE;
}

ZipArchive::~ZipArchive() {
  if (opened) { 
    printf("freeing buffer\n");
    free(zipBuffer);
  }
  if (handle != INVALID_HANDLE) {
    printf("deleting handle\n");
    go_Archive_delete(handle);
  }
}

void ZipArchive::checkOpen() {
  if (opened) 
    throw std::runtime_error("zip file is already open");
}

void ZipArchive::checkReturn(int32_t ret) {
  if (ret < 0) {
    std::string msg = "zip error: " + std::to_string(ret);
    throw std::runtime_error(msg.c_str());
  }
}

void ZipArchive::open(const std::string& path) {
  checkOpen();
  std::ifstream f(path);
  if (!f.is_open()) {
    throw std::runtime_error("failed to open file");
  }
  f.seekg(0, std::ios::end);
  zipLength = f.tellg();
  f.seekg(0); 
  zipBuffer = (char*)malloc(zipLength * sizeof(char));
  f.read(zipBuffer, zipLength); 
  openBuffer(zipBuffer, zipLength);
}

void ZipArchive::openBuffer(std::vector<char>& buffer) {
  checkOpen();
  openBuffer(buffer.data(), buffer.size());
}

void ZipArchive::openBuffer(char* buffer, size_t length) {
  checkOpen();
  GoSlice goBuffer;
  goBuffer.len = length;
  goBuffer.cap = length;
  goBuffer.data = (void*)buffer;
  handle = go_Archive_create(goBuffer); 
  if (handle == INVALID_HANDLE) 
    throw std::runtime_error("failed to open zip file");
  opened = true;
}

void ZipArchive::save() {
  
}

std::vector<char> ZipArchive::saveBuffer() {
  return {};
}

void ZipArchive::extractAll(const std::string& dst) {
  GOSTRING(goDst, dst.c_str());
  checkReturn(go_Archive_extractAll(handle, goDst)); 
}

std::vector<char> ZipArchive::extractFile(const std::string& filePath) {
  GOSTRING(goFilePath, filePath.c_str());
  int64_t size = go_Archive_getFileSize(handle, goFilePath);
  if (size < 0) { 
    std::string msg = "could not stat file: " + filePath;
    throw std::runtime_error(msg.c_str());
  }
  std::vector<char> vec;
  vec.resize(size);
  GoSlice goBuffer;
  goBuffer.len = size;
  goBuffer.cap = size;
  goBuffer.data = (void*)vec.data();
  checkReturn(go_Archive_extract(handle, goFilePath, goBuffer));
  vec[size-1] = '\0';
  return vec;
}

void ZipArchive::extractFileToDisk(const std::string& filePath, const std::string& dstPath) {
  GOSTRING(goFilePath, filePath.c_str());
  GOSTRING(goDstPath, dstPath.c_str());
  checkReturn(go_Archive_extractToDisk(handle, goFilePath, goDstPath));
}

void ZipArchive::write(const std::string& filePath, std::vector<char>& data) {

}

void ZipArchive::write(const std::string& filePath, char* data, size_t length) {

}

void ZipArchive::writeFile(const std::string& filePath, const std::string& srcPath) {

}

void writeDir(const std::string& dirPath, const std::string& srcDir) {

}

}
