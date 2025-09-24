#include "lib.hpp"

int main() {
    Map<STRING, STRING, std::string, std::string> m = {
        { "key1", "value1" }, 
        { "key2", "value2" }
    };
    std::string s = m.get("key1");
    printf("s = %s\n", s.c_str());
    return 0;
}