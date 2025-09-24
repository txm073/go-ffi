#pragma once

#include "lib/goapi.h"
#include <string>
#include <stdexcept>

template <type_t KeyTypeID, type_t ValueTypeID, typename KeyType, typename ValueType> 
class Map {
private:
    handle_t handle;
    inline void checkRet(int ret) {
        if (ret != SUCCESS) {
            std::string msg = "code: " + std::to_string(ret);
            throw std::runtime_error(msg.c_str());
        }
    }

public:
    typedef struct {
        KeyType key;
        ValueType value;
    } PairType;

    inline Map() {
        handle = createMap();
    }

    inline Map(std::initializer_list<PairType> items) {
        handle = createMap();
        for (auto& pair: items) {
            set(pair.key, pair.value);
        }
    }

    inline ~Map() {
        deleteMap(handle);
    }

    inline ValueType get(const KeyType& key) {
        int ret;
        switch (KeyTypeID) {
            case STRING: 
                buffer_t val;
                ret = getMapValue(
                    handle, 
                    KeyTypeID, 
                    ValueTypeID, 
                    (void*)key.c_str(),
                    (void*)&val
                );
                checkRet(ret);
                return std::string(val.data, val.len);
            default:
                break;
        }
        throw std::runtime_error("error");
    }

    inline void set(const KeyType& key, const ValueType& value) {
        int ret;
        switch (KeyTypeID) {
            case STRING: 
                ret = setMapValue(
                    handle, 
                    KeyTypeID, 
                    ValueTypeID, 
                    (void*)key.c_str(),
                    (void*)value.c_str()
                );
                checkRet(ret);
                break;
            default:
                break;
        }
    }    
};