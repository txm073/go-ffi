// #include "cpp/array.hpp"
// #include "cpp/zip.hpp"
#include "lib.hpp"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <stdexcept>

void testStringArray() {
  go::Array<GO_STRING, buffer_t> arr;
  buffer_t s;
  s.data = (char*)"Hello World!";
  s.len = strlen(s.data);
  arr.append(s);
  printf("Length: %d\n", arr.size());
  buffer_t elem;
  arr.get(0, &elem);
  printf("Element 0: %s\n", elem.data);
  free(elem.data);
  elem.data = (char*)"updated";
  elem.len = strlen(elem.data);
  arr.set(0, &elem);
  printf("Element 0: %s\n", elem.data);
}

void testIntArray() {
  go::Array<GO_INT32, int> arr2 = { 1, 2, 3, 4, 5 };
  arr2.set(3, 13);
  arr2.insert(1, 20);
  arr2.remove(5);
  printf("Length: %d\n", arr2.size());
  for (int i = 0; i < arr2.size(); ++i) {
      printf("arr[%d] = %d\n", i, arr2.get(i));
  }    
}

void testHttp() {
  hn::http::Session session;
  auto resp = session.get(
    "http://localhost:5000/test", 
    {
      {"param1", "value1"}, 
      {"param2", "value2"}, 
      {"redirect", "https://github.com"}
    },
    {
      {"Content-Type", "application/json"}
    }
  );
  printf("resp status: %d\n", resp.code());
  printf("content: %s\n", resp.text());

  auto resp2 = session.get(
    "http://localhost:5000/test", 
    {
      {"param1", "value1"}, 
      {"param2", "value2"}, 
      {"redirect", "https://github.com"}
    },
    {
      {"Content-Type", "application/json"}
    }
  );
  printf("resp2 status: %d\n", resp2.code());
  printf("content: %s\n", resp2.text());
  printf("done\n");
}

void testZip() {
  std::ifstream f("test.zip");
  if (!f.is_open()) {
    printf("Error: failed to open file\n");
    return;
  }
  f.seekg(0, std::ios::end);
  size_t len = f.tellg();
  f.seekg(0);
  GoSlice slice;
  slice.data = malloc(len);
  slice.len = len;
  slice.cap = len;
  f.read((char*)slice.data, len);
  go_handle_t handle = go_Archive_create(slice);
  if (handle == INVALID_HANDLE) {
    printf("error\n");
    return;
  }
  GoString path;
  path.p = (char*)"cpp/array.hpp";
  path.n = strlen(path.p);
  int64_t size = go_Archive_getFileSize(handle, path);
  GoSlice buffer;
  buffer.len = size;
  buffer.cap = size;
  buffer.data = malloc(size);
  int ret = go_Archive_extract(handle, path, buffer);
  free(buffer.data);
  go_Archive_delete(handle);

  // GoSlice contents = go_Archive_extract(handle, path);
  // printf("data: %s\n", (const char*)contents.data);
  // go_testZip(slice);  
}

void test() {
  using namespace hn::utils;
  ZipArchive archive;
  archive.open("test.zip");
  // archive.extractAll("extracted");
  archive.extractFileToDisk("lib/libgoapi.so", "dynamiclib.so");
  // auto res = archive.extractFile("lib/shared.h");
  // printf("%s\n", res.data());

}

int main() {
  try {
    loadGoLibrary();
    testHttp();
    freeGoLibrary();
  } catch (std::runtime_error& e) {
    printf("error: %s\n", e.what());
  }
  return 0;
}
