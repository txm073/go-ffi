cmake_minimum_required(VERSION 3.11)
project(go-ffi)
set(CMAKE_CXX_STANDARD 17)

add_executable(go-ffi main.cpp cpp/http.cpp cpp/zip.cpp)
# add_executable(go-ffi test.cpp)