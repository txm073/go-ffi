#pragma once

#include "../lib/goapi.h"
#include <string>
#include <stdexcept>

namespace go {

template <type_t GoKeyType, typename CppKeyType, 
          type_t GoValueType, typename CppValueType>
class Map {
private:
  go_handle_t handle;
  inline void checkRet(int ret) {
    if (ret != SUCCESS) {
      std::string msg = "go hashmap error: " + std::to_string(ret);
      throw std::runtime_error(msg.c_str());
    }
  }

public:
  typedef struct {
    CppKeyType key;
    CppValueType value;
  } PairType;

  inline Map() {
    handle = go_createMap();
  }

  inline Map(std::initializer_list<PairType> items) {
    handle = go_createMap();
    for (auto &pair : items)
    {
      set(pair.key, pair.value);
    }
  }

  inline ~Map() {
    go_deleteMap(handle);
  }

  inline void get(const CppKeyType &key, CppValueType* val) {
    int ret = go_getMapValue(
      handle, GoKeyType, GoValueType, (void*)&key, (void*)val
    )
    checkRet(ret);  
  }

  inline void set(const CppKeyType &key, const CppValueType &value) {
    int ret;
    switch (GoKeyType) {
      case STRING:
        ret = go_setMapValue(
            handle,
            GoKeyType,
            GoValueType,
            (void *)key.c_str(),
            (void *)value.c_str());
        checkRet(ret);
        break;
      default:
        break;
    }
  }

  inline go_handle_t getHandle() {
    return handle;
  }
};

}