@echo off
cd lib
call go build -buildmode=c-archive -o goapi.lib
cd ..
call g++ main.cpp -o main.exe -Llib -lgoapi
call main.exe