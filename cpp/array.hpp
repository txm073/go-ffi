#pragma once

#include "../lib/goapi.h"
#include <string>
#include <stdexcept>

namespace go {

template <type_t GoType, typename CppType> 
class Array {
private:
  int length = 0;
  go_handle_t handle;
  inline void checkRet(int ret) {
    if (ret != SUCCESS) {
      std::string msg = "go array error: " + std::to_string(ret);
      throw std::runtime_error(msg.c_str());
    }
  }

  inline void checkIndex(int index) {
    if (index < 0 || index >= length) {
      std::string msg = "go array error: index " + std::to_string(index) + " is out of bounds";
      throw std::runtime_error(msg.c_str());
    }
  }

public:
  inline Array() {
    handle = go_createSlice();
  }

  inline Array(std::initializer_list<CppType> elems) {
    handle = go_createSlice();
    for (auto& elem: elems) 
      append(elem);
  }

  inline ~Array() {
    go_deleteSlice(handle);
  }

  inline void append(const CppType& elem) {
    insert(length, elem);
  }

  inline void insert(int index, CppType elem) {
    int ret = go_insertElement(handle, GoType, index, (void*)&elem);
    checkRet(ret);
    length++;
  }

  inline void remove(int index) {
    checkIndex(index);
    int ret = go_insertElement(handle, GoType, index, nullptr);
    checkRet(ret);
  }

  inline void set(int index, CppType* elem) {
    checkIndex(index);
    int ret = go_setElement(handle, GoType, index, (void*)elem);
    return checkRet(ret);
  }

  inline void set(int index, CppType elem) {
    set(index, &elem);
  }

  inline void get(int index, CppType* outElem) {
    checkIndex(index);
    int ret = go_getElement(handle, GoType, index, (void*)outElem);
    checkRet(ret);  
  }

  inline CppType get(int index) {
    CppType ret;
    get(index, &ret);
    return ret;
  }

  inline int size() {
    return go_getSliceLength(handle);
  }

  inline go_handle_t getHandle() { 
    return handle; 
  }
};

}
//a
